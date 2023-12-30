/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: tcp connection.
 * Author: renshuang
 * Create: 2023-09-25
 */

#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "connection.h"
#include "channel.h"
#include "tcp_socket.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class TcpConnection : public Connection, public Channel, public std::enable_shared_from_this<TcpConnection> {
public:
    using Connection::channelRequest_;

    TcpConnection() {};
    ~TcpConnection() override;

    int StartConnection(const ChannelRequest &request, std::shared_ptr<IChannelListener> channelListener) override;
    int StartListen(const ChannelRequest &request, std::shared_ptr<IChannelListener> channelListener) override;
    void CloseConnection() override;
    bool Send(const uint8_t *buf, int bufLen) override;
    
private:
    void ConfigSocket();
    void Connect();
    void Receive(int socket);
    void ReadLooper(int socket);
    void AcceptVideoAndAudio();
    void Accept();
    void SetAudioConnection(int socket);
    void HandleReceivedData(int socket);
    uint32_t GetReceivedDataLength(uint8_t *header);
    void HandleRemoteControlReceivedData(uint32_t dataLength, uint8_t *header, uint8_t *buf);

    static constexpr int RET_ERR = -1;
    static constexpr int RET_OK = 0;
    static constexpr int INVALID_SOCKET = -1;
    static constexpr int STOP_RECEIVE = -2;
    /*
     * 数据包头长度固定为4
     */
    static constexpr unsigned int PACKET_HEADER_LEN = 4;
    /*
     * TCP每次收数据包的最大长度，超过则认为非法
     */
    static constexpr unsigned int ILLEGAL_LENGTH = 10 * 1024 * 1024;
    /*
     * 设置发送缓冲区大小为512K
     */
    static constexpr unsigned int SOCKET_SEND_BUFFER_SIZE = 512 * 1024;
    /*
     * 设置接收缓冲区大小为 10M
     */
    static constexpr unsigned int SOCKET_RECV_BUFFER_SIZE = 10 * 1024 * 1024;
    static constexpr int CONTROL_LENGTH_MASK = 0xFFFF;

    std::atomic<bool> isReceiving_{ false };
    TcpSocket socket_;
    // 连接的客户端套接字
    int remoteSocket_{ INVALID_SOCKET };
    // 音频通道
    std::shared_ptr<TcpConnection> tcpAudioConn_{ nullptr };
    std::mutex connectionMtx_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif