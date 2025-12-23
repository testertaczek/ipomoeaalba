#pragma once
/** @file ia/audio.h
 *  @brief TODO docs
 * 
 *  TODO docs
 */
#include <ia/foundation.h>

/* ia_audio_library: ia_audio_adapter, ia_audio_interface */
IA_DECL_LIBRARY(audio);

typedef struct { ia_identifier id; } ia_audio_device;
typedef struct { ia_identifier id; } ia_audio_stream;
typedef struct { ia_identifier id; } ia_audio_buffer;
typedef struct { ia_identifier id; } ia_audio_node;

typedef struct ia_audio_device_info {
    ia_audio_device handle;
    char            name[256];
    ia_flags        flags;
    u32             sample_rate;
    i32             channel_count;
} ia_audio_device_info;

typedef struct ia_audio_stream_info {
    u32 sample_rate;
    i32 channel_count;
    i32 frames_per_buffer;
} ia_audio_stream_info;

/** Output is allocated via drifter. TODO docs */
typedef ia_result (IA_CALL *ia_audio_enumerate_devices_fn)(
    ia_audio_adapter       *adapter,
    i32                    *out_device_count,
    ia_audio_device_info  **out_device_infos);
#define IA_AUDIO_ENUMERATE_DEVICES_FN(X) \
    ia_result IA_CALL _ia_##X##_audio_enumerate_devices( \
        ia_audio_adapter       *adapter, \
        i32                    *out_device_count, \
        ia_audio_device_info  **out_device_infos)

/** TODO docs */
typedef ia_result (IA_CALL *ia_audio_create_stream_fn)(
    ia_audio_adapter           *adapter,
    ia_audio_stream_info const *stream_info,
    ia_audio_stream            *out_stream);
#define IA_AUDIO_CREATE_STREAM_FN(X) \
    ia_result IA_CALL _ia_##X##_audio_create_stream( \
        ia_audio_adapter           *adapter, \
        ia_audio_stream_info const *stream_info, \
        ia_audio_stream            *out_stream)

/** TODO docs */
typedef void (IA_CALL *ia_audio_destroy_stream_fn)(
    ia_audio_adapter   *adapter,
    ia_audio_stream     stream);
#define IA_AUDIO_DESTROY_STREAM_FN(X) \
    void IA_CALL _ia_##X##_audio_destroy_stream( \
        ia_audio_adapter   *adapter, \
        ia_audio_stream     stream)

struct ia_audio_interface {
    ia_interface_header             header;
    ia_audio_enumerate_devices_fn   enumerate_devices;
    ia_audio_create_stream_fn       create_stream;
    ia_audio_destroy_stream_fn      destroy_stream;
};

enum : i32 {
    ia_audio_adapter_dummy = 0,
    ia_audio_adapter_pipewire,
    ia_audio_adapter_jack,
    ia_audio_adapter_alsa,
    ia_audio_adapter_pulseaudio,
    ia_audio_adapter_steinberg_asio,
    ia_audio_adapter_xaudio2,
    ia_audio_adapter_wasapi,
    ia_audio_adapter_aaudio,
    ia_audio_adapter_webaudio,
    ia_audio_adapter_coreaudio,
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef IA_AUDIO_PIPEWIRE
IA_API IA_DECL_LIBRARY_IMPL(audio, pipewire);
#endif /* IA_AUDIO_PIPEWIRE */
#ifdef IA_AUDIO_JACK
IA_API IA_DECL_LIBRARY_IMPL(audio, jack);
#endif /* IA_AUDIO_JACK */
#ifdef IA_AUDIO_ALSA
IA_API IA_DECL_LIBRARY_IMPL(audio, alsa);
#endif /* IA_AUDIO_ALSA */
#ifdef IA_AUDIO_PULSEAUDIO
IA_API IA_DECL_LIBRARY_IMPL(audio, pulse);
#endif /* IA_AUDIO_PULSEAUDIO */
#ifdef IA_AUDIO_STEINBERG_ASIO  
IA_API IA_DECL_LIBRARY_IMPL(audio, steinberg_asio);
#endif /* IA_AUDIO_STEINBERG_ASIO */
#ifdef IA_AUDIO_XAUDIO2
IA_API IA_DECL_LIBRARY_IMPL(audio, xaudio2);
#endif /* IA_AUDIO_XAUDIO2 */
#ifdef IA_AUDIO_WASAPI
IA_API IA_DECL_LIBRARY_IMPL(audio, wasapi);
#endif /* IA_AUDIO_WASAPI */
#ifdef IA_AUDIO_AAUDIO
IA_API IA_DECL_LIBRARY_IMPL(audio, aaudio);
#endif /* IA_AUDIO_AAUDIO */
#ifdef IA_AUDIO_WEBAUDIO
IA_API IA_DECL_LIBRARY_IMPL(audio, webaudio);
#endif /* IA_AUDIO_WEBAUDIO */
#ifdef IA_AUDIO_COREAUDIO
IA_API IA_DECL_LIBRARY_IMPL(audio, coreaudio);
#endif /* IA_AUDIO_COREAUDIO */

/** The dummy audio backend is always available. */
IA_API IA_DECL_LIBRARY_IMPL(audio, dummy);

/** Picks the first working backend in an order optimal for the host platform.
 *  The `dummy` backend is ignored, and if any other backend fails, must be called explicitly. */
IA_API IA_DECL_LIBRARY_IMPL(audio, auto);

#ifdef __cplusplus
}
#endif /* __cplusplus */
