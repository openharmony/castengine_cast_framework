/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Mirror player implement realization.
 * Author: zhangjingnan
 * Create: 2023-05-29
 */

#include "mirror_player_impl.h"
#include "cast_engine_errors.h"
#include "cast_engine_log.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
DEFINE_CAST_ENGINE_LABEL("Cast-MirrorPlayerImpl");

MirrorPlayerImpl::~MirrorPlayerImpl()
{
    CLOGD("~MirrorPlayerImpl in.");
}

int32_t MirrorPlayerImpl::Play(const std::string &deviceId)
{
    CLOGD("Play in");
    auto session = session_.promote();
    if (!session) {
        CLOGE("session is nullptr");
        return CAST_ENGINE_ERROR;
    }
    return session->Play(deviceId);
}

int32_t MirrorPlayerImpl::Pause(const std::string &deviceId)
{
    CLOGD("Pause in");
    auto session = session_.promote();
    if (!session) {
        CLOGE("session is nullptr");
        return CAST_ENGINE_ERROR;
    }
    return session->Pause(deviceId);
}

int32_t MirrorPlayerImpl::SetSurface(sptr<IBufferProducer> producer)
{
    CLOGD("SetSurface in");
    auto session = session_.promote();
    if (!session) {
        CLOGE("session is nullptr");
        return CAST_ENGINE_ERROR;
    }
    return session->SetSurface(producer);
}

int32_t MirrorPlayerImpl::SetAppInfo(const AppInfo &appInfo)
{
    CLOGI("SetAppInfo in");
    Permission::SaveMirrorAppInfo({appInfo.appUid, appInfo.appTokenId, appInfo.appPid});
    return CAST_ENGINE_SUCCESS;
}

int32_t MirrorPlayerImpl::DeliverInputEvent(const OHRemoteControlEvent &event)
{
    CLOGD("DeliverInputEvent in");
    auto session = session_.promote();
    if (!session) {
        CLOGE("session is nullptr");
        return CAST_ENGINE_ERROR;
    }
    return session->DeliverInputEvent(event);
}

int32_t MirrorPlayerImpl::InjectEvent(const OHRemoteControlEvent &event)
{
    CLOGD("InjectEvent in");
    auto session = session_.promote();
    if (!session) {
        CLOGE("session is nullptr");
        return CAST_ENGINE_ERROR;
    }
    return session->InjectEvent(event);
}

int32_t MirrorPlayerImpl::Release()
{
    auto session = session_.promote();
    if (!session) {
        CLOGE("session is nullptr");
        return CAST_ENGINE_ERROR;
    }
    session->DestroyMirrorPlayer();
    return CAST_ENGINE_SUCCESS;
}

int32_t MirrorPlayerImpl::GetDisplayId(std::string &displayId)
{
    CLOGD("GetDisplayId in");
    auto session = session_.promote();
    if (!session) {
        CLOGE("session is nullptr");
        return CAST_ENGINE_ERROR;
    }
    return session->GetDisplayId(displayId);
}

int32_t MirrorPlayerImpl::ResizeVirtualScreen(uint32_t width, uint32_t height)
{
    CLOGD("ResizeVirtualScreen in");
    auto session = session_.promote();
    if (!session) {
        CLOGE("session is nullptr");
        return CAST_ENGINE_ERROR;
    }
    return session->ResizeVirtualScreen(width, height);
}
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS