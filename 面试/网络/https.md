## HTTPS连接过程

https连接主要是多了ssl协商的过程，ssl协商的目的是为了安全的传递**对称加密密钥**。在协商过程中使用的是非对称加密。协商完后，使用对称加密密钥对数据加密发送。为什么不一直用非对称加密，因为非对称加密处理起来更复杂，通信过程中使用这种方式，效率会很低。

![img](https://pic2.zhimg.com/v2-6f22cec067c33f363478c7c82a7c5599_r.jpg)

1. 客户端和服务器的协商过程，主要用来确定**SSL**版本、使用的加密算法、秘钥长度等
2. 协商过程中，服务端会发送**公开秘钥证书**给客户端，客户端用**认证机关**的公开密钥确认证书的有效性，然后取出**公开秘钥**
3. 客户端使用获得到的公开密钥，对客户端生成的通信加密中使用的一种被称为**Pre-master secret**的随机密码串进行加密，发送给服务端
4. 服务端使用**私有秘钥**对发来的数据进行解密，取出**Pre-master secret**
5. 服务端和客户端根据**Pre-master secret**生成master_secret，master_secret的计算公式为PRF(pre_master_secret, "master secret", ClientHello.random **+** ServerHello.random)，其中 ClientHello.random和ServerHello.random在协商过程中服务端和客户端完成交换，再通过该master_secret来产生**对称加密**的key
6. 客户端和服务端使用对称加密的key进行后续数据的发送