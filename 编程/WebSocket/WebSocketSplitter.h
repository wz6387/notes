
#ifndef WEBSOCKETSPLITTER_H
#define WEBSOCKETSPLITTER_H

#include <string>
#include <stdint.h>
#include <memory>
#include <vector>


class WebSocketHeader {
public:
	typedef std::shared_ptr<WebSocketHeader> Ptr;
	typedef enum {
		CONTINUATION = 0x0,
		TEXT = 0x1,
		BINARY = 0x2,
		RSV3 = 0x3,
		RSV4 = 0x4,
		RSV5 = 0x5,
		RSV6 = 0x6,
		RSV7 = 0x7,
		CLOSE = 0x8,
		PING = 0x9,
		PONG = 0xA,
		CONTROL_RSVB = 0xB,
		CONTROL_RSVC = 0xC,
		CONTROL_RSVD = 0xD,
		CONTROL_RSVE = 0xE,
		CONTROL_RSVF = 0xF
	} Type;
public:

	WebSocketHeader() : m_mask(4) {
		//获取_mask内部buffer的内存地址，该内存是malloc开辟的，地址为随机
		uint64_t ptr = (uint64_t)(&m_mask[0]);
		//根据内存地址设置掩码随机数
		m_mask.assign((uint8_t*)(&ptr), (uint8_t*)(&ptr) + 4);
	}
	virtual ~WebSocketHeader() {}

public:
	bool m_fin;
	uint8_t m_reserved;
	Type m_opcode;
	bool m_mask_flag;
	uint64_t m_payload_len;
	std::vector<uint8_t > m_mask;
};

class WebSocketSplitter : public WebSocketHeader {
public:
	WebSocketSplitter(){
		m_handshake_flag = false;
		m_content_len = 0;
		m_handshake_data_size = 0;
		m_mask_offset = 0;
		m_got_header = false;
		m_payload_offset = 0;
	};
	virtual ~WebSocketSplitter(){};

    /**
     * 添加数据
     * @param data 需要添加的数据
     * @param len 数据长度
     */
    void input(const char *data,uint64_t len);
protected:
    /**
     * 收到请求头
     * @param data 请求头数据
     * @param len 请求头长度
     *
     * @return 请求头后的content长度,
     *  <0 : 代表后面所有数据都是content，此时后面的content将分段通过onRecvContent函数回调出去
     *  0 : 代表为后面数据还是请求头,
     *  >0 : 代表后面数据为固定长度content,此时将缓存content并等到所有content接收完毕一次性通过onRecvContent函数回调出去
     */
	virtual int64_t onRecvHeader(const char *data, uint64_t len) { return 0; }

    /**
     * 收到content分片或全部数据
     * onRecvHeader函数返回>0,则为全部数据
     * @param data content分片或全部数据
     * @param len 数据长度
     */
    void onRecvContent(const char *data,uint64_t len);

    /**
     * 判断数据中是否有包尾
     * @param data 数据指针
     * @param len 数据长度
     * @return nullptr代表未找到包位，否则返回包尾指针
     */
    const char *onSearchPacketTail(const char *data, uint64_t len);

    /**
     * 设置content len
     */
    void setContentLen(int64_t content_len);

    /**
     * 恢复初始设置
     */
     void reset();

     /**
      * 剩余数据大小
      */
     int64_t remainDataSize();
protected:
    std::string  m_handshake_buf;
	bool  m_handshake_flag;
    int64_t m_content_len;
    int64_t m_handshake_data_size;


	/**
   * 输入数据以便解包webSocket数据以及处理粘包问题
   * 可能触发onWebSocketDecodeHeader和onWebSocketDecodePayload回调
   * @param data 需要解包的数据，可能是不完整的包或多个包
   * @param len 数据长度
   */
	void decode(uint8_t *data, uint64_t len);

	/**
	 * 编码一个数据包
	 * 将触发onWebSocketEncodeData回调
	 * @param header 数据头
	 * @param buffer 负载数据
	 */
	void encode(const WebSocketHeader &header, const char* buf ,int len);

protected:
	/**
	 * 收到一个webSocket数据包包头，后续将继续触发onWebSocketDecodePayload回调
	 * @param header 数据包头
	 */
	virtual void onWebSocketDecodeHeader(const WebSocketHeader &header) {};

	/**
	 * 收到webSocket数据包负载
	 * @param header 数据包包头
	 * @param ptr 负载数据指针
	 * @param len 负载数据长度
	 * @param recved 已接收数据长度(包含本次数据长度)，等于header._payload_len时则接受完毕
	 */
	virtual void onWebSocketDecodePayload(const WebSocketHeader &header, const uint8_t *ptr, uint64_t len, uint64_t recved) {};

	/**
	 * 接收到完整的一个webSocket数据包后回调
	 * @param header 数据包包头
	 */
	virtual void onWebSocketDecodeComplete(const WebSocketHeader &header) {};

	/**
	 * websocket数据编码回调
	 * @param ptr 数据指针
	 * @param len 数据指针长度
	 */
	virtual void onWebSocketEncodeData(const char* buf, int len) {};

private:
	void onPayloadData(uint8_t *data, uint64_t len);
private:
	std::string m_remain_data;
	int m_mask_offset;
	bool m_got_header;
	uint64_t m_payload_offset;

};

#endif //WEBSOCKETSPLITTER
