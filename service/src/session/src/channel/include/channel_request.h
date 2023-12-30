/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: channel request
 * Author: sunhong
 * Create: 2022-01-19
 */

#ifndef CHANNEL_REQUEST_H
#define CHANNEL_REQUEST_H

#include <string>
#include "channel_info.h"
#include "cast_engine_common.h"
#include "cast_service_common.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
struct ChannelRequest {
    ChannelRequest() = default;
    ~ChannelRequest() = default;

    ChannelRequest(const ModuleType moduleType, bool isReceiver, const CastLocalDevice &localDeviceInfo,
           const CastInnerRemoteDevice &remoteDeviceInfo, const CastSessionProperty &sessionProperty)
        : moduleType(moduleType),
          isReceiver(isReceiver),
          localDeviceInfo(localDeviceInfo),
          remoteDeviceInfo(remoteDeviceInfo),
          sessionProperty(sessionProperty)
    {
        linkType =
            remoteDeviceInfo.channelType == ChannelType::SOFT_BUS ? ChannelLinkType::SOFT_BUS : ChannelLinkType::TCP;
    };

    bool operator<(const ChannelRequest &request) const
    {
        return connectionId < request.connectionId;
    };

    ChannelLinkType linkType;
    ModuleType moduleType;
    bool isReceiver;
    int sessionId{ INVALID_ID };
    int localPort{ INVALID_PORT };
    int remotePort{ INVALID_PORT };
    int connectionId{ INVALID_ID };
    CastLocalDevice localDeviceInfo;
    CastInnerRemoteDevice remoteDeviceInfo;
    CastSessionProperty sessionProperty;
    std::string fileReceiveRootPath;
};
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // CHANNEL_REQUEST_H
