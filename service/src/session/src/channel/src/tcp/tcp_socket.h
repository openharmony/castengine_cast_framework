/*
 * Copyright (C) 2023-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * Description: tcp socket.
 * Author: renshuang
 * Create: 2023-09-25
 */

#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
class TcpSocket {
public:
    TcpSocket();
    ~TcpSocket();

    int Bind(const std::string &ip, int port);
    bool Listen(int backlog);
    int Accept();
    bool Connect(const std::string &ip, int port);
    int Send(int fd, const uint8_t *buff, size_t length);
    ssize_t Recv(int fd, uint8_t *buff, size_t length);
    void Close();
    void Shutdown(int fd);
    int GetPeerPort(int fd);
    int GetSocketFd();
    // 设置发送缓冲区大小
    bool SetSendBufferSize(int size);
    // 设置接收缓冲区大小
    bool SetRecvBufferSize(int size);
    // 设置close()关闭连接时的行为，默认是如果有数据残留在发送缓冲区的，系统继续发送这些数据，等待被确认，然后返回
    bool SetLinger(bool active, int seconds);
    // 一方开启KeepAlive功能后，会自动在规定时间内相对方发送心跳包，另一方收到会自动回复，告诉对方我仍然在线
    bool SetKeepAlive();
    bool SetKeepAlive(unsigned idleTime, unsigned numProbes, unsigned probeInterval);
    // 设置SO_REUSEADDR，对应TCP套接字处于TIME_WAIT状态下的socket可以重复绑定使用
    bool SetReuseAddr();

private:
    static constexpr int RANDOM_PORT = 0;
    static constexpr int INVALID_PORT = -1;
    static constexpr int INVALID_SOCKET = -1;
    static constexpr int DEFAULT_VALUE = 0;
    static constexpr int SOCKET_FLAG = 0;
    static constexpr int SOCKET_OFF = 0;
    static constexpr int SOCKET_ON = 1;
    static constexpr int RET_OK = 0;
    static constexpr int RET_ERR = -1;
    static constexpr int STOP_RECEIVE = -2;
    
    bool stopReceive_{ false };
    int GetBindPort();
    int socket_;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif