# 音视频投播管理服务框架部件

## 简介

提供自适应Cast+ Stream，Wi-Fi Display，DLNA多种协议的音视频投播能力，为南北向开发者提供统一的接口及归一化的体验。

## 目录

```
/foundation/CastEngine/castengine_cast_framework  # 音视频投播管理服务框架业务代码
├── clinet                             # 客户端实现
├── common                             # 公共引用
├── etc                                # SA描述
├── interfaces                         # 接口文件
├── sa_profile                         # SA profile文件
├── service                            # 服务端实现
├── LICENSE                            # 证书文件
├── BUILD.gn                           # 编译入口
├── test                               # 测试代码
└── bundle.json                        # 部件描述文件
```

## 编译构建

```
# 通过gn编译,在out目录下对应产品的文件夹中生成libcast.z.so、libcast_engine_client.z.so、libcast_engine_service.z.so
hb build cast
```

### 使用说明

提供整体的投播框架，支持其他投屏协议的接入以及投屏协议自适应选择。
北向接入可参考[Sample](https://gitee.com/openharmony/applications_app_samples/tree/master/code/BasicFeature/Media/AVSession)。

## 相关仓

[castengine_cast_plus_stream](https://gitee.com/openharmony-sig/castengine_cast_plus_stream)

[castengine_wifi_display](https://gitee.com/openharmony-sig/castengine_wifi_display)

[castengine_dlna](https://gitee.com/openharmony-sig/castengine_dlna)