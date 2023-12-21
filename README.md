# castengine_cast_framework

## Introduction

Provide audio and video broadcasting capabilities with adaptive Cast+Stream, Wi Fi Display, and DLNA protocols, providing a unified interface and normalized experience for north-south developers.

## Directory Structure

```
/foundation/CastEngine/castengine_cast_framework
├── clinet                             # Implementation on the client side
├── common                             # common code
├── etc                                # SA profile file
├── interfaces                         # Inner api
├── sa_profile                         # SA profile
├── service                            # Implementation on the service side
├── LICENSE                            # Certificate file
├── BUILD.gn                           # Compilation Entry
├── test                               # test code
└── bundle.json                        # Component description file

```

## Compilation and Building

```
# Generate the libcast.z.so、libcast_engine_client.z.so、libcast_engine_service.z.so file in the out directory of the product folder through GN compilation.
hb build cast
```

### Usage

For details, see[Sample](https://gitee.com/openharmony/applications_app_samples/tree/master/code/BasicFeature/Media/AVSession)。

## Repositories Involved

[castengine_cast_plus_stream](https://gitee.com/openharmony-sig/castengine_cast_plus_stream)

[castengine_wifi_display](https://gitee.com/openharmony-sig/castengine_wifi_display)

[castengine_dlna](https://gitee.com/openharmony-sig/castengine_dlna)