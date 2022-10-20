#include "WebSocketSplitter.h"
#ifdef WIN32
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#include <string.h>
#endif


#define CHECK_LEN(size) \
do{ \
    if(len - (ptr - data) < size){ \
        if(m_remain_data.empty()){ \
            m_remain_data.assign((char *)data,(const unsigned int)len); \
        } \
        return ; \
    } \
}while(0) \


void WebSocketSplitter::input(const char *data,uint64_t len) {
    const char *ptr = data;

	/*是否完成websocket握手，完成http切换websocket*/
    if(!m_handshake_buf.empty()){
		m_handshake_buf.append(data,(const unsigned int)len);
        data = ptr = m_handshake_buf.data();
        len = m_handshake_buf.size();
    }

    //splitPacket:

    /*确保ptr最后一个字节是0，防止strstr越界
     *由于ZLToolKit确保内存最后一个字节是保留未使用字节并置0，
     *所以此处可以不用再次置0
     *但是上层数据可能来自其他渠道，保险起见还是置0
     */

    char &tail_ref = ((char *) ptr)[len];
    char tail_tmp = tail_ref;
    tail_ref = 0;

    //数据按照请求头处理
    const char *index = nullptr;
	m_handshake_data_size = len;
    while (m_content_len == 0 && m_handshake_data_size > 0 && (index = onSearchPacketTail(ptr, m_handshake_data_size)) != nullptr) {
        if(index == ptr){
            break;
        }
        //_content_len == 0，这是请求头
        const char *header_ptr = ptr;
        int64_t header_size = index - ptr;
        ptr = index;
		m_handshake_data_size = len - (ptr - data);
        m_content_len = onRecvHeader(header_ptr, header_size);
		m_handshake_flag = true;
    }

    if(m_handshake_data_size <= 0){
        //没有剩余数据，清空缓存
		m_handshake_buf.clear();
        return;
    }

    //_content_len < 0;数据按照不固定长度content处理
    onRecvContent(ptr, len);//消费掉所有剩余数据
}

void WebSocketSplitter::setContentLen(int64_t content_len) {
    m_content_len = content_len;
}

void WebSocketSplitter::reset() {
   m_content_len = 0;
   m_handshake_data_size = 0;
   m_handshake_buf.clear();
}

const char *WebSocketSplitter::onSearchPacketTail(const char *data,uint64_t len) {
    auto pos = strstr(data,"\r\n\r\n");
    if(pos == NULL){
        return NULL;
    }
    return  pos + 4;
}

int64_t WebSocketSplitter::remainDataSize() {
    return m_handshake_data_size;
}


void WebSocketSplitter::onRecvContent(const char *data, uint64_t len)
{
	decode((unsigned char*)data, len);
}

void WebSocketSplitter::decode(uint8_t *data, uint64_t len) {
	uint8_t *ptr = data;
	if (!m_got_header) {
		//还没有获取数据头
		if (!m_remain_data.empty()) {
			m_remain_data.append((char *)data, (const unsigned int)len);
			data = ptr = (uint8_t *)m_remain_data.data();
			len = m_remain_data.size();
		}

	begin_decode:
		CHECK_LEN(1);
		m_fin = (*ptr & 0x80) >> 7;
		m_reserved = (*ptr & 0x70) >> 4;
		m_opcode = (WebSocketHeader::Type) (*ptr & 0x0F);
		ptr += 1;

		CHECK_LEN(1);
		m_mask_flag = (*ptr & 0x80) >> 7;
		m_payload_len = (*ptr & 0x7F);
		ptr += 1;

		if (m_payload_len == 126) {
			CHECK_LEN(2);
			m_payload_len = (*ptr << 8) | *(ptr + 1);
			ptr += 2;
		}
		else if (m_payload_len == 127) {
			CHECK_LEN(8);
			m_payload_len = ((uint64_t)ptr[0] << (8 * 7)) |
				((uint64_t)ptr[1] << (8 * 6)) |
				((uint64_t)ptr[2] << (8 * 5)) |
				((uint64_t)ptr[3] << (8 * 4)) |
				((uint64_t)ptr[4] << (8 * 3)) |
				((uint64_t)ptr[5] << (8 * 2)) |
				((uint64_t)ptr[6] << (8 * 1)) |
				((uint64_t)ptr[7] << (8 * 0));
			ptr += 8;
		}
		if (m_mask_flag) {
			CHECK_LEN(4);
			m_mask.assign(ptr, ptr + 4);
			ptr += 4;
		}
		m_got_header = true;
		m_mask_offset = 0;
		m_payload_offset = 0;
		onWebSocketDecodeHeader(*this);
		if (m_payload_len == 0) {
			onWebSocketDecodeComplete(*this);
			m_got_header = false;
			uint64_t remain = len - (ptr - data);
			if (remain > 0) {
				std::string str((char *)ptr, (const unsigned int)remain);
				m_remain_data = str;

				data = ptr = (uint8_t *)m_remain_data.data();
				len = m_remain_data.size();
				goto begin_decode;
			}
		}
	}

	//进入后面逻辑代表已经获取到了webSocket协议头，

	uint64_t remain = len - (ptr - data);
	if (remain > 0) {
		uint64_t payload_slice_len = remain;
		if (payload_slice_len + m_payload_offset > m_payload_len) {
			payload_slice_len = m_payload_len - m_payload_offset;
		}
		m_payload_offset += payload_slice_len;
		onPayloadData(ptr, payload_slice_len);

		if (m_payload_offset == m_payload_len) {
			onWebSocketDecodeComplete(*this);

			//这是下一个包
			remain -= payload_slice_len;
			ptr += payload_slice_len;
			m_got_header = false;

			if (remain > 0) {
				//剩余数据是下一个包，把它的数据放置在缓存中
				std::string str((char *)ptr, (const unsigned int)remain);
				m_remain_data = str;

				data = ptr = (uint8_t *)m_remain_data.data();
				len = m_remain_data.size();
				goto begin_decode;
			}
		}
	}
	m_remain_data.clear();
}

void WebSocketSplitter::onPayloadData(uint8_t *data, uint64_t len) {
	if (m_mask_flag) {
		for (int i = 0; i < len; ++i, ++data) {
			*(data) ^= m_mask[(i + m_mask_offset) % 4];
		}
		m_mask_offset = (m_mask_offset + len) % 4;
	}
	onWebSocketDecodePayload(*this, m_mask_flag ? data - len : data, len, m_payload_offset);
}

void WebSocketSplitter::encode(const WebSocketHeader &header, const char* buffer, int buf_len) {
	std::string ret;
	uint64_t len = buffer ? buf_len : 0;
	uint8_t byte = header.m_fin << 7 | ((header.m_reserved & 0x07) << 4) | (header.m_opcode & 0x0F);
	ret.push_back(byte);

	auto mask_flag = (header.m_mask_flag && header.m_mask.size() >= 4);
	byte = mask_flag << 7;

	if (len < 126) {
		byte |= len;
		ret.push_back(byte);
	}
	else if (len <= 0xFFFF) {
		byte |= 126;
		ret.push_back(byte);

		auto len_low = htons((u_short)len);
		ret.append((char *)&len_low, 2);
	}
	else {
		byte |= 127;
		ret.push_back(byte);

		uint32_t len_high = htonl(len >> 32);
		uint32_t len_low = htonl(len & 0xFFFFFFFF);
		ret.append((char *)&len_high, 4);
		ret.append((char *)&len_low, 4);
	}
	if (mask_flag) {
		ret.append((char *)header.m_mask.data(), 4);
	}

	if (len > 0) {
		if (mask_flag) {
			uint8_t *ptr = (uint8_t*)buffer;
			for (int i = 0; i < len; ++i, ++ptr) {
				*(ptr) ^= header.m_mask[i % 4];
			}
		}
		ret.append(buffer, buf_len);
	}
	onWebSocketEncodeData(ret.c_str(), ret.length());
}
