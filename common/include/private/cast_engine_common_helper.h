/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: supply a helper to write/read common cast engine structures through ipc
 * Author: zhangge
 * Create: 2022-06-15
 */

#ifndef CAST_ENGINE_COMMON_HELPER_H
#define CAST_ENGINE_COMMON_HELPER_H

#include "cast_engine_common.h"
#include "oh_remote_control_event.h"

namespace OHOS {
namespace CastEngine {
bool WriteCastRemoteDevice(Parcel &parcel, const CastRemoteDevice &device);
std::unique_ptr<CastRemoteDevice> ReadCastRemoteDevice(Parcel &parcel);
bool ReadCastRemoteDevice(Parcel &parcel, CastRemoteDevice &device);

bool WriteMediaInfo(MessageParcel &parcel, const MediaInfo &mediaInfo);
std::unique_ptr<MediaInfo> ReadMediaInfo(MessageParcel &parcel);

bool WriteMediaInfoHolder(MessageParcel &parcel, const MediaInfoHolder &mediaInfoHolder);
std::unique_ptr<MediaInfoHolder> ReadMediaInfoHolder(MessageParcel &parcel);

bool WriteCastLocalDevice(Parcel &parcel, const CastLocalDevice &device);
std::unique_ptr<CastLocalDevice> ReadCastLocalDevice(Parcel &parcel);

bool WriteCastSessionProperty(Parcel &parcel, const CastSessionProperty &property);
std::unique_ptr<CastSessionProperty> ReadCastSessionProperty(Parcel &parcel);

bool WritePropertyContainer(Parcel &parcel, const PropertyContainer &device);
std::unique_ptr<PropertyContainer> ReadPropertyContainer(Parcel &parcel);

bool WriteAuthInfo(Parcel &parcel, const AuthInfo &authInfo);
std::unique_ptr<AuthInfo> ReadAuthInfo(Parcel &parcel);

bool WriteRemoteControlEvent(Parcel &parcel, const OHRemoteControlEvent &event);
std::unique_ptr<OHRemoteControlEvent> ReadRemoteControlEvent(Parcel &parcel);

bool WriteDeviceStateInfo(Parcel &parcel, const DeviceStateInfo &stateInfo);
std::unique_ptr<DeviceStateInfo> ReadDeviceStateInfo(Parcel &parcel);

bool WriteEvent(Parcel &parcel, const EventId &eventId, const std::string &jsonParam);
bool ReadEvent(Parcel &parcel, int32_t &eventId, std::string &jsonParam);

void SetDataCapacity(MessageParcel &parcel, const FileFdMap &fileList, uint32_t tokenSize);
bool WriteFileList(MessageParcel &parcel, const FileFdMap &fileList);
bool ReadFileList(MessageParcel &parcel, FileFdMap &fileList);

bool WriteRcvFdFileMap(MessageParcel &parcel, const RcvFdFileMap &rcvFdFileMap);
bool ReadRcvFdFileMap(MessageParcel &parcel, RcvFdFileMap &rcvFdFileMap);
} // namespace CastEngine
} // namespace OHOS

#endif
