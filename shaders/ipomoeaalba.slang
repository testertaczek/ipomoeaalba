#pragma once

#if !defined(IA_GPU_TABLE_SET_BINDING)
#define IA_GPU_TABLE_SET_BINDING 0
#define IA_STORAGE_BUFFER_BINDING 0
#define IA_STORAGE_TEXTURE_BINDING 1
#define IA_SAMPLED_TEXTURE_BINDING 2
#define IA_SAMPLER_BINDING 3
#define IA_DEVICE_ADDRESS_BUFFER_BINDING 4
#define IA_ACCELERATION_STRUCTURE_BINDING 5
#endif
#define IA_PUSH_CONSTANT(T)                 [[vk::push_constant]] struct T

#define _IA_DECL_STORAGE_BUFFERS            [[vk::binding(IA_STORAGE_BUFFER_BINDING, 0)]]
#define _IA_DECL_STORAGE_TEXTURES           [[vk::binding(IA_STORAGE_TEXTURE_BINDING, 0)]]
#define _IA_DECL_SAMPLED_TEXTURES           [[vk::binding(IA_SAMPLED_TEXTURE_BINDING, 0)]]
#define _IA_DECL_SAMPLERS                   [[vk::binding(IA_SAMPLER_BINDING, 0)]]
#define _IA_DECL_ACCELERATION_STRUCUTRES    [[vk::binding(IA_ACCELERATION_STRUCTURE_BINDING, 0)]]
#define _IA_DECL_BUFFER_ADDRESSES           [[vk::binding(IA_DEVICE_ADDRESS_BUFFER_BINDING, 0)]]

#define _IA_DECL_VEC_TYPES(SLANG_TYPE, IA_TYPE) \
    typedef SLANG_TYPE IA_TYPE; \
    typedef SLANG_TYPE ## 2 IA_TYPE ## x2; \
    typedef SLANG_TYPE ## 3 IA_TYPE ## x3; \
    typedef SLANG_TYPE ## 4 IA_TYPE ## x4; \

#define _IA_DECL_MAT_TYPES(SLANG_TYPE, IA_TYPE) \
    typedef matrix<SLANG_TYPE, 2, 2> IA_TYPE ## m2x2; \
    typedef matrix<SLANG_TYPE, 3, 2> IA_TYPE ## m2x3; \
    typedef matrix<SLANG_TYPE, 4, 2> IA_TYPE ## m2x4; \
    typedef matrix<SLANG_TYPE, 2, 3> IA_TYPE ## m3x2; \
    typedef matrix<SLANG_TYPE, 3, 3> IA_TYPE ## m3x3; \
    typedef matrix<SLANG_TYPE, 4, 3> IA_TYPE ## m3x4; \
    typedef matrix<SLANG_TYPE, 2, 4> IA_TYPE ## m4x2; \
    typedef matrix<SLANG_TYPE, 3, 4> IA_TYPE ## m4x3; \
    typedef matrix<SLANG_TYPE, 4, 4> IA_TYPE ## m4x4;

#define _IA_DECL_VEC_MAT_8_16_32_64_TYPES(SLANG_TYPE, IA_TYPE) \
    _IA_DECL_VEC_TYPES(SLANG_TYPE ## 8_t,  IA_TYPE ## 8)  \
    _IA_DECL_VEC_TYPES(SLANG_TYPE ## 16_t, IA_TYPE ## 16) \
    _IA_DECL_VEC_TYPES(SLANG_TYPE ## 32_t, IA_TYPE ## 32) \
    _IA_DECL_VEC_TYPES(SLANG_TYPE ## 64_t, IA_TYPE ## 64) \
    _IA_DECL_MAT_TYPES(SLANG_TYPE ## 8_t,  IA_TYPE ## 8)  \
    _IA_DECL_MAT_TYPES(SLANG_TYPE ## 16_t, IA_TYPE ## 16) \
    _IA_DECL_MAT_TYPES(SLANG_TYPE ## 32_t, IA_TYPE ## 32) \
    _IA_DECL_MAT_TYPES(SLANG_TYPE ## 64_t, IA_TYPE ## 64)

_IA_DECL_VEC_MAT_8_16_32_64_TYPES(uint, u)
_IA_DECL_VEC_MAT_8_16_32_64_TYPES(int, i)
_IA_DECL_VEC_TYPES(half, f16)
_IA_DECL_VEC_TYPES(float, f32)
_IA_DECL_VEC_TYPES(double, f64)
_IA_DECL_MAT_TYPES(half, f16)
_IA_DECL_MAT_TYPES(float, f32)
_IA_DECL_MAT_TYPES(double, f64)

#define IA_GFX_ID_INDEX_BITS        (20)
#define IA_GFX_ID_INDEX_MASK        ((u64(1) << IA_GFX_ID_INDEX_BITS) - u64(1))
#define IA_GFX_ID_INDEX_OFFSET      (0)
#define IA_GFX_ID_VERSION_BITS      (44)
#define IA_GFX_ID_VERSION_MASK      ((u64(1) << IA_GFX_ID_VERSION_BITS) - u64(1))
#define IA_GFX_ID_VERSION_OFFSET    IA_GFX_ID_INDEX_BITS

_IA_DECL_STORAGE_BUFFERS            __DynamicResource               ia_buffers[];
_IA_DECL_STORAGE_BUFFERS            coherent __DynamicResource      ia_buffers_coherent[];
_IA_DECL_STORAGE_TEXTURES           __DynamicResource               ia_storage_textures[];
_IA_DECL_STORAGE_TEXTURES           coherent __DynamicResource      ia_storage_textures_coherent[];
_IA_DECL_SAMPLED_TEXTURES           __DynamicResource               ia_sampled_textures[];
_IA_DECL_SAMPLED_TEXTURES           coherent __DynamicResource      ia_sampled_textures_coherent[];
_IA_DECL_SAMPLERS                   SamplerState                    ia_samplers[];
#if defined(IA_RAY_TRACING)        
_IA_DECL_ACCELERATION_STRUCTURES    RaytracingAccelerationStructure ia_acceleration_structures[];
#endif                             
_IA_DECL_BUFFER_ADDRESSES           StructuredBuffer<u64>           ia_buffer_addresses;

struct ia_buffer_id {
    u64 value;

    u32 index() 
    { return u32(IA_GFX_ID_INDEX_MASK & value); }

    u64 version() 
    { return (value >> IA_GFX_ID_VERSION_OFFSET); }

    bool is_empty() 
    { return value == 0; }

    u64 device_address() 
    { return ia_buffer_addresses[index()]; }
};

struct ia_texture_view_id {
    u64 value;

    u32 index() 
    { return u32(IA_GFX_ID_INDEX_MASK & value); }

    u64 version() 
    { return (value >> IA_GFX_ID_VERSION_OFFSET); }

    bool is_empty() 
    { return value == 0; }
};

struct ia_texture_view_index {
    u32 value;

    u32 index() 
    { return u32(IA_GFX_ID_INDEX_MASK & value); }

    bool is_empty() 
    { return value == 0; }
};

struct ia_sampler_id {
    u64 value;

    u32 index() 
    { return u32(IA_GFX_ID_INDEX_MASK & value); }

    u64 version() 
    { return (value >> IA_GFX_ID_VERSION_OFFSET); }

    bool is_empty() 
    { return value == 0; }

    SamplerState get() 
    { return ia_samplers[index()]; }
};

#if defined(IA_RAY_TRACING)
_IA_DECL_ACCELERATION_STRUCTURES RaytracingAccelerationStructure ia_ray_tracing_bvh_table[];

struct ia_bvh_id {
    u64 value;

    u32 index()
    { return u32(IA_GFX_ID_INDEX_MASK & value); }

    u64 version()
    { return (value >> IA_GFX_ID_VERSION_OFFSET); }

    bool is_empty()
    { return value == 0; }

    RaytracingAccelerationStructure get()
    { return ia_ray_tracing_bvh_table[index()]; }
};

extension RaytracingAccelerationStructure {
    static RaytracingAccelerationStructure get(ia_bvh_id i) 
    { return ia_ray_tracing_bvh_table[i.index()]; }
};
#endif // IA_RAY_TRACING

extension ByteAddressBuffer {
    static ByteAddressBuffer get(ia_buffer_id i) 
    { return ia_buffers[i.index()]; }

    static ByteAddressBuffer get_coherent(ia_buffer_id i) 
    { return ia_buffers_coherent[i.index()]; }
};

extension RWByteAddressBuffer {
    static RWByteAddressBuffer get(ia_buffer_id i) 
    { return ia_buffers[i.index()]; }

    static RWByteAddressBuffer get_coherent(ia_buffer_id i) 
    { return ia_buffers_coherent[i.index()]; }
};

extension SamplerState {
    static SamplerState get(ia_sampler_id i) 
    { return ia_samplers[i.index()]; }
};

#define _IA_DECL_TEX_EXTENSION(TYPE, TABLE)                             \
__generic<T : ITexelElement> extension TYPE<T> {                        \
    static TYPE<T> get(ia_texture_view_id i)                            \
    { return TABLE[i.index()]; }                                        \
                                                                        \
    static TYPE<T> get_coherent(ia_texture_view_id i)                   \ 
    { return TABLE ## _coherent[i.index()]; }                           \
                                                                        \
    static TYPE<T> get(ia_texture_view_idx i)                           \
    { return TABLE[i.index()]; }                                        \
                                                                        \
    static TYPE<T> get_coherent(ia_texture_view_idx i)                  \
    { return TABLE ## _coherent[i.index()]; }                           \
}                                                                       \
__generic<T : ITexelElement>                                            \
struct TYPE ## _id {                                                    \
    ia_texture_view_id handle;                                          \
                                                                        \
    TYPE<T> get()                                                       \
    { return TYPE<T>::get(handle);                                      \
                                                                        \
    TYPE<T> get_coherent()                                              \
    { return TYPE<T>::get_coherent(handle); }                           \
};                                                                      \
__generic<T : ITexelElement>                                            \
struct TYPE ## _idx {                                                   \
    ia_texture_view_idx index;                                          \
                                                                        \
    TYPE<T> get() { return TYPE<T>::get(index); }                       \
    TYPE<T> get_coherent() { return TYPE<T>::get_coherent(index); }     \
};                                                                      \

_IA_DECL_TEX_EXTENSION(RWTexture1D, ia_storage_textures)
_IA_DECL_TEX_EXTENSION(RWTexture2D, ia_storage_textures)
_IA_DECL_TEX_EXTENSION(RWTexture3D, ia_storage_textures)
_IA_DECL_TEX_EXTENSION(RWTexture1DArray, ia_storage_textures)
_IA_DECL_TEX_EXTENSION(RWTexture2DArray, ia_storage_textures)

_IA_DECL_TEX_EXTENSION(Texture1D, ia_sampled_textures)
_IA_DECL_TEX_EXTENSION(Texture2D, ia_sampled_textures)
_IA_DECL_TEX_EXTENSION(Texture3D, ia_sampled_textures)
_IA_DECL_TEX_EXTENSION(Texture1DArray, ia_sampled_textures)
_IA_DECL_TEX_EXTENSION(Texture2DArray, ia_sampled_textures)
_IA_DECL_TEX_EXTENSION(TextureCube, ia_sampled_textures)
_IA_DECL_TEX_EXTENSION(TextureCubeArray, ia_sampled_textures)
_IA_DECL_TEX_EXTENSION(Texture2DMS, ia_sampled_textures)
