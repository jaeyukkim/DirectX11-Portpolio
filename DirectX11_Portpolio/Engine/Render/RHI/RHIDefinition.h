#pragma once
#include "HeaderCollection.h"



enum class ERHIInterfaceType
{
	Hidden,
	Null,
	D3D11,
	D3D12,
	Vulkan,
	Metal,
	Agx,
	OpenGL,
};

enum EShaderFrequency : UINT8
{
	SF_Vertex			= 0,
	SF_Mesh				= 1,
	SF_Amplification	= 2,
	SF_Pixel			= 3,
	SF_Geometry			= 4,
	SF_Compute			= 5,
	SF_RayGen			= 6,
	SF_RayMiss			= 7,
	SF_RayHitGroup		= 8,
	SF_RayCallable		= 9,
	SF_Hull            = 10,
	SF_Domain          = 11,

	SF_NumFrequencies = 12,

	SF_NumGraphicsFrequencies = 7, // VS, PS, GS, HS, DS, Mesh, Amplification
    SF_NumStandardFrequencies = 8, // VS, PS, GS, HS, DS, Compute, Mesh, Amplification

	SF_NumBits			= 5,
};
static_assert(SF_NumFrequencies <= (1 << SF_NumBits), "SF_NumFrequencies will not fit on SF_NumBits");

inline bool IsValidGraphicsFrequency(EShaderFrequency InShaderFrequency)
{
	switch (InShaderFrequency)
	{
	case SF_Vertex:        return true;
#if PLATFORM_SUPPORTS_MESH_SHADERS
	case SF_Mesh:          return true;
	case SF_Amplification: return true;
#endif
	case SF_Pixel:         return true;
	case SF_Geometry:      return true;
	}
	return false;
}

inline bool IsComputeShaderFrequency(EShaderFrequency ShaderFrequency)
{
	switch (ShaderFrequency)
	{
	case SF_Compute:
	case SF_RayGen:
	case SF_RayMiss:
	case SF_RayHitGroup:
	case SF_RayCallable:
		return true;
	}
	return false;
}

enum { MAX_TEXTURE_MIP_COUNT = 15 };
enum { MAX_MESH_LOD_COUNT = 8 };

enum
{
	MaxVertexElementCount = 17,
	MaxVertexElementCount_NumBits = 5,
};
static_assert(MaxVertexElementCount <= (1 << MaxVertexElementCount_NumBits), "MaxVertexElementCount will not fit on MaxVertexElementCount_NumBits");

enum { ShaderArrayElementAlignBytes = 16 };

enum
{
	MaxSimultaneousRenderTargets = 8,
	MaxSimultaneousRenderTargets_NumBits = 3,
};
static_assert(MaxSimultaneousRenderTargets <= (1 << MaxSimultaneousRenderTargets_NumBits), "MaxSimultaneousRenderTargets will not fit on MaxSimultaneousRenderTargets_NumBits");

enum { MaxSimultaneousUAVs = 8 };


enum ESamplerFilter
{
	SF_Point,
	SF_Bilinear,
	SF_Trilinear,
	SF_AnisotropicPoint,
	SF_AnisotropicLinear,

	ESamplerFilter_Num,
	ESamplerFilter_NumBits = 3,
};
static_assert(ESamplerFilter_Num <= (1 << ESamplerFilter_NumBits), "ESamplerFilter_Num will not fit on ESamplerFilter_NumBits");

enum ESamplerAddressMode
{
	AM_Wrap,
	AM_Clamp,
	AM_Mirror,
	/** Not supported on all platforms */
	AM_Border,

	ESamplerAddressMode_Num,
	ESamplerAddressMode_NumBits = 2,
};
static_assert(ESamplerAddressMode_Num <= (1 << ESamplerAddressMode_NumBits), "ESamplerAddressMode_Num will not fit on ESamplerAddressMode_NumBits");

enum ESamplerCompareFunction
{
	SCF_Never,
	SCF_Less
};

enum ERasterizerFillMode
{
	FM_Point,
	FM_Wireframe,
	FM_Solid,

	ERasterizerFillMode_Num,
	ERasterizerFillMode_NumBits = 2,
};
static_assert(ERasterizerFillMode_Num <= (1 << ERasterizerFillMode_NumBits), "ERasterizerFillMode_Num will not fit on ERasterizerFillMode_NumBits");

enum ERasterizerCullMode
{
	CM_None,
	CM_CW,
	CM_CCW,

	ERasterizerCullMode_Num,
	ERasterizerCullMode_NumBits = 2,
};
static_assert(ERasterizerCullMode_Num <= (1 << ERasterizerCullMode_NumBits), "ERasterizerCullMode_Num will not fit on ERasterizerCullMode_NumBits");

enum class ERasterizerDepthClipMode : UINT8
{
	DepthClip,
	DepthClamp,

	Num,
	NumBits = 1,
};
static_assert(UINT32(ERasterizerDepthClipMode::Num) <= (1U << UINT32(ERasterizerDepthClipMode::NumBits)), "ERasterizerDepthClipMode::Num will not fit on ERasterizerDepthClipMode::NumBits");

enum EColorWriteMask
{
	CW_RED   = 0x01,
	CW_GREEN = 0x02,
	CW_BLUE  = 0x04,
	CW_ALPHA = 0x08,

	CW_NONE  = 0,
	CW_RGB   = CW_RED | CW_GREEN | CW_BLUE,
	CW_RGBA  = CW_RED | CW_GREEN | CW_BLUE | CW_ALPHA,
	CW_RG    = CW_RED | CW_GREEN,
	CW_BA    = CW_BLUE | CW_ALPHA,

	EColorWriteMask_NumBits = 4,
};

enum ECompareFunction
{
	CF_Less,
	CF_LessEqual,
	CF_Greater,
	CF_GreaterEqual,
	CF_Equal,
	CF_NotEqual,
	CF_Never,
	CF_Always,

	ECompareFunction_Num,
	ECompareFunction_NumBits = 3,

	/*
	// Utility enumerations
	CF_DepthNearOrEqual		= (((UINT32)ERHIZBuffer::IsInverted != 0) ? CF_GreaterEqual : CF_LessEqual),
	CF_DepthNear			= (((UINT32)ERHIZBuffer::IsInverted != 0) ? CF_Greater : CF_Less),
	CF_DepthFartherOrEqual	= (((UINT32)ERHIZBuffer::IsInverted != 0) ? CF_LessEqual : CF_GreaterEqual),
	CF_DepthFarther			= (((UINT32)ERHIZBuffer::IsInverted != 0) ? CF_Less : CF_Greater),*/
};
static_assert(ECompareFunction_Num <= (1 << ECompareFunction_NumBits), "ECompareFunction_Num will not fit on ECompareFunction_NumBits");

enum EStencilMask
{
	SM_Default,
	SM_255,
	SM_1,
	SM_2,
	SM_4,
	SM_8,
	SM_16,
	SM_32,
	SM_64,
	SM_128,
	SM_Count
};

enum EStencilOp
{
	SO_Keep,
	SO_Zero,
	SO_Replace,
	SO_SaturatedIncrement,
	SO_SaturatedDecrement,
	SO_Invert,
	SO_Increment,
	SO_Decrement,

	EStencilOp_Num,
	EStencilOp_NumBits = 3,
};
static_assert(EStencilOp_Num <= (1 << EStencilOp_NumBits), "EStencilOp_Num will not fit on EStencilOp_NumBits");

enum EBlendOperation
{
	BO_Add,
	BO_Subtract,
	BO_Min,
	BO_Max,
	BO_ReverseSubtract,

	EBlendOperation_Num,
	EBlendOperation_NumBits = 3,
};
static_assert(EBlendOperation_Num <= (1 << EBlendOperation_NumBits), "EBlendOperation_Num will not fit on EBlendOperation_NumBits");

enum EBlendFactor
{
	BF_Zero,
	BF_One,
	BF_SourceColor,
	BF_InverseSourceColor,
	BF_SourceAlpha,
	BF_InverseSourceAlpha,
	BF_DestAlpha,
	BF_InverseDestAlpha,
	BF_DestColor,
	BF_InverseDestColor,
	BF_ConstantBlendFactor,
	BF_InverseConstantBlendFactor,
	BF_Source1Color,
	BF_InverseSource1Color,
	BF_Source1Alpha,
	BF_InverseSource1Alpha,

	EBlendFactor_Num,
	EBlendFactor_NumBits = 4,
};
static_assert(EBlendFactor_Num <= (1 << EBlendFactor_NumBits), "EBlendFactor_Num will not fit on EBlendFactor_NumBits");


enum EVertexElementType
{
	VET_None,
	VET_Float1,
	VET_Float2,
	VET_Float3,
	VET_Float4,
	VET_PackedNormal,	// FPackedNormal
	VET_UByte4,
	VET_UByte4N,
	VET_Color,
	VET_Short2,
	VET_Short4,
	VET_Short2N,		// 16 bit word normalized to (value/32767.0,value/32767.0,0,0,1)
	VET_Half2,			// 16 bit float using 1 bit sign, 5 bit exponent, 10 bit mantissa 
	VET_Half4,
	VET_Short4N,		// 4 X 16 bit word, normalized 
	VET_UShort2,
	VET_UShort4,
	VET_UShort2N,		// 16 bit word normalized to (value/65535.0,value/65535.0,0,0,1)
	VET_UShort4N,		// 4 X 16 bit word unsigned, normalized 
	VET_URGB10A2N,		// 10 bit r, g, b and 2 bit a normalized to (value/1023.0f, value/1023.0f, value/1023.0f, value/3.0f)
	VET_UInt,
	VET_MAX,

	VET_NumBits = 5,
};
static_assert(VET_MAX <= (1 << VET_NumBits), "VET_MAX will not fit on VET_NumBits");



/** The USF binding type for a resource in a shader. */
enum class EShaderCodeResourceBindingType : UINT8
{
	Invalid,

	SamplerState,

	// Texture1D: not used in the renderer.
	// Texture1DArray: not used in the renderer.
	Texture2D,
	Texture2DArray,
	Texture2DMS,
	Texture3D,
	// Texture3DArray: not used in the renderer.
	TextureCube,
	TextureCubeArray,
	TextureMetadata,

	Buffer,
	StructuredBuffer,
	ByteAddressBuffer,
	RaytracingAccelerationStructure,

	// RWTexture1D: not used in the renderer.
	// RWTexture1DArray: not used in the renderer.
	RWTexture2D,
	RWTexture2DArray,
	RWTexture3D,
	// RWTexture3DArray: not used in the renderer.
	RWTextureCube,
	// RWTextureCubeArray: not used in the renderer.
	RWTextureMetadata,

	RWBuffer,
	RWStructuredBuffer,
	RWByteAddressBuffer,

	RasterizerOrderedTexture2D,

	MAX
};

/** The base type of a value in a shader parameter structure. */
enum EUniformBufferBaseType : UINT8
{
	UBMT_INVALID,

	// Invalid type when trying to use bool, to have explicit error message to programmer on why
	// they shouldn't use bool in shader parameter structures.
	UBMT_BOOL,

	// Parameter types.
	UBMT_UINT32,
	UBMT_UINT64,
	UBMT_FLOAT32,

	// RHI resources not tracked by render graph.
	UBMT_TEXTURE,
	UBMT_SRV,
	UBMT_UAV,
	UBMT_SAMPLER,

	// Resources tracked by render graph.
	UBMT_RDG_TEXTURE,
	UBMT_RDG_TEXTURE_ACCESS,
	UBMT_RDG_TEXTURE_ACCESS_ARRAY,
	UBMT_RDG_TEXTURE_SRV,
	UBMT_RDG_TEXTURE_UAV,
	UBMT_RDG_BUFFER_ACCESS,
	UBMT_RDG_BUFFER_ACCESS_ARRAY,
	UBMT_RDG_BUFFER_SRV,
	UBMT_RDG_BUFFER_UAV,
	UBMT_RDG_UNIFORM_BUFFER,

	// Nested structure.
	UBMT_NESTED_STRUCT,

	// Structure that is nested on C++ side, but included on shader side.
	UBMT_INCLUDED_STRUCT,

	// GPU Indirection reference of struct, like is currently named Uniform buffer.
	UBMT_REFERENCED_STRUCT,

	// Structure dedicated to setup render targets for a rasterizer pass.
	UBMT_RENDER_TARGET_BINDING_SLOTS,

	EUniformBufferBaseType_Num,
	EUniformBufferBaseType_NumBits = 5,
};
static_assert(EUniformBufferBaseType_Num <= (1 << EUniformBufferBaseType_NumBits), "EUniformBufferBaseType_Num will not fit on EUniformBufferBaseType_NumBits");


enum class EPrimitiveTopologyType : UINT8
{
	Triangle,
	Patch,
	Line,
	Point,
	//Quad,

	Num,
	NumBits = 2,
};
static_assert((UINT32)EPrimitiveTopologyType::Num <= (1 << (UINT32)EPrimitiveTopologyType::NumBits), "EPrimitiveTopologyType::Num will not fit on EPrimitiveTopologyType::NumBits");

enum EPrimitiveType
{
	PT_TriangleList,
	PT_TriangleStrip,
	PT_LineList,
	PT_QuadList,
	PT_PointList,
	PT_RectList,
	PT_Num,
	PT_NumBits = 3
};
static_assert(PT_Num <= (1 << 8), "EPrimitiveType doesn't fit in a byte");
static_assert(PT_Num <= (1 << PT_NumBits), "PT_NumBits is too small");


enum class EBufferUsageFlags : UINT32
{
	None                    = 0,
	
	Static                  = 1 << 0,
	Dynamic                 = 1 << 1,
	Volatile                = 1 << 2,
	UnorderedAccess         = 1 << 3,
	ByteAddressBuffer       = 1 << 4,
	SourceCopy              = 1 << 5,
	DrawIndirect            = 1 << 7,
	ShaderResource          = 1 << 8,
	KeepCPUAccessible       = 1 << 9,
	FastVRAM                = 1 << 10,
	Shared                  = 1 << 12,
	AccelerationStructure   = 1 << 13,
	VertexBuffer            = 1 << 14,
	IndexBuffer             = 1 << 15,
	StructuredBuffer        = 1 << 16,
	MultiGPUAllocate		= 1 << 17,
	MultiGPUGraphIgnore		= 1 << 18,
	RayTracingScratch = (1 << 19) | UnorderedAccess,
	NullResource = 1 << 20,
	AnyDynamic = (Dynamic | Volatile),
};
ENUM_CLASS_FLAGS(EBufferUsageFlags);

#define BUF_None                   EBufferUsageFlags::None
#define BUF_Static                 EBufferUsageFlags::Static
#define BUF_Dynamic                EBufferUsageFlags::Dynamic
#define BUF_Volatile               EBufferUsageFlags::Volatile
#define BUF_UnorderedAccess        EBufferUsageFlags::UnorderedAccess
#define BUF_ByteAddressBuffer      EBufferUsageFlags::ByteAddressBuffer
#define BUF_SourceCopy             EBufferUsageFlags::SourceCopy
#define BUF_StreamOutput           EBufferUsageFlags::StreamOutput
#define BUF_DrawIndirect           EBufferUsageFlags::DrawIndirect
#define BUF_ShaderResource         EBufferUsageFlags::ShaderResource
#define BUF_KeepCPUAccessible      EBufferUsageFlags::KeepCPUAccessible
#define BUF_FastVRAM               EBufferUsageFlags::FastVRAM
#define BUF_Transient              EBufferUsageFlags::Transient
#define BUF_Shared                 EBufferUsageFlags::Shared
#define BUF_AccelerationStructure  EBufferUsageFlags::AccelerationStructure
#define BUF_RayTracingScratch	   EBufferUsageFlags::RayTracingScratch
#define BUF_VertexBuffer           EBufferUsageFlags::VertexBuffer
#define BUF_IndexBuffer            EBufferUsageFlags::IndexBuffer
#define BUF_StructuredBuffer       EBufferUsageFlags::StructuredBuffer
#define BUF_AnyDynamic             EBufferUsageFlags::AnyDynamic
#define BUF_MultiGPUAllocate       EBufferUsageFlags::MultiGPUAllocate
#define BUF_MultiGPUGraphIgnore    EBufferUsageFlags::MultiGPUGraphIgnore
#define BUF_NullResource           EBufferUsageFlags::NullResource


enum ERHIResourceType : UINT8
{
	RRT_None,

	RRT_SamplerState,
	RRT_RasterizerState,
	RRT_DepthStencilState,
	RRT_BlendState,
	RRT_VertexDeclaration,
	RRT_VertexShader,
	RRT_HullShader,
	RRT_DomainShader,
	RRT_MeshShader,
	RRT_AmplificationShader,
	RRT_PixelShader,
	RRT_GeometryShader,
	RRT_RayTracingShader,
	RRT_ComputeShader,
	RRT_GraphicsPipelineState,
	RRT_ComputePipelineState,
	RRT_RayTracingPipelineState,
	RRT_BoundShaderState,
	RRT_UniformBufferLayout,
	RRT_UniformBuffer,
	RRT_Buffer,
	RRT_Texture,
	// @todo: texture type unification - remove these
	RRT_Texture2D,
	RRT_Texture2DArray,
	RRT_Texture3D,
	RRT_TextureCube,
	// @todo: texture type unification - remove these
	RRT_TextureReference,
	RRT_TimestampCalibrationQuery,
	RRT_GPUFence,
	RRT_RenderQuery,
	RRT_RenderQueryPool,
	RRT_ComputeFence,
	RRT_Viewport,
	RRT_UnorderedAccessView,
	RRT_ShaderResourceView,
	RRT_RayTracingAccelerationStructure,
	RRT_StagingBuffer,
	RRT_CustomPresent,
	RRT_ShaderLibrary,
	RRT_PipelineBinaryLibrary,

	RRT_Num
};


enum class ETextureDimension : UINT8
{
	Texture2D,
	Texture2DArray,
	Texture3D,
	TextureCube,
	TextureCubeArray
};


enum class ETextureCreateFlags : UINT64
{
    None                              = 0,

    RenderTargetable                  = 1ull << 0,
    ResolveTargetable                 = 1ull << 1,
    DepthStencilTargetable            = 1ull << 2,
    ShaderResource                    = 1ull << 3,
    SRGB                              = 1ull << 4,
    CPUWritable                       = 1ull << 5,
    NoTiling                          = 1ull << 6,
    VideoDecode                       = 1ull << 7,
    Dynamic                           = 1ull << 8,
    InputAttachmentRead               = 1ull << 9,
    Foveation                         = 1ull << 10,
    Tiling3D                          = 1ull << 11,
    Memoryless                        = 1ull << 12,
    GenerateMipCapable                = 1ull << 13,
    FastVRAMPartialAlloc              = 1ull << 14,
    DisableSRVCreation                = 1ull << 15,
    DisableDCC                        = 1ull << 16,

    UAV                               = 1ull << 17,
    Presentable                       = 1ull << 18,
    CPUReadback                       = 1ull << 19,
    OfflineProcessed                  = 1ull << 20,
    FastVRAM                          = 1ull << 21,
    HideInVisualizeTexture            = 1ull << 22,
    Virtual                           = 1ull << 23,
    TargetArraySlicesIndependently    = 1ull << 24,
    Shared                            = 1ull << 25,
    NoFastClear                       = 1ull << 26,
    DepthStencilResolveTarget         = 1ull << 27,
    Streamable                        = 1ull << 28,
    NoFastClearFinalize               = 1ull << 29,
	Atomic64Compatible                = 1ull << 30,
    ReduceMemoryWithTilingMode        = 1ull << 31,
    AtomicCompatible                  = 1ull << 33,
	External                		  = 1ull << 34,
	MultiGPUGraphIgnore				  = 1ull << 35,
	ReservedResource                  = 1ull << 37,
	ImmediateCommit                   = 1ull << 38,

	ForceIntoNonStreamingMemoryTracking = 1ull << 39,
};


ENUM_CLASS_FLAGS(ETextureCreateFlags);

// Compatibility defines
#define TexCreate_None                           ETextureCreateFlags::None
#define TexCreate_RenderTargetable               ETextureCreateFlags::RenderTargetable
#define TexCreate_ResolveTargetable              ETextureCreateFlags::ResolveTargetable
#define TexCreate_DepthStencilTargetable         ETextureCreateFlags::DepthStencilTargetable
#define TexCreate_ShaderResource                 ETextureCreateFlags::ShaderResource
#define TexCreate_SRGB                           ETextureCreateFlags::SRGB
#define TexCreate_CPUWritable                    ETextureCreateFlags::CPUWritable
#define TexCreate_NoTiling                       ETextureCreateFlags::NoTiling
#define TexCreate_VideoDecode                    ETextureCreateFlags::VideoDecode
#define TexCreate_Dynamic                        ETextureCreateFlags::Dynamic
#define TexCreate_InputAttachmentRead            ETextureCreateFlags::InputAttachmentRead
#define TexCreate_Foveation                      ETextureCreateFlags::Foveation
#define TexCreate_3DTiling                       ETextureCreateFlags::Tiling3D
#define TexCreate_Memoryless                     ETextureCreateFlags::Memoryless
#define TexCreate_GenerateMipCapable             ETextureCreateFlags::GenerateMipCapable
#define TexCreate_FastVRAMPartialAlloc           ETextureCreateFlags::FastVRAMPartialAlloc
#define TexCreate_DisableSRVCreation             ETextureCreateFlags::DisableSRVCreation
#define TexCreate_DisableDCC                     ETextureCreateFlags::DisableDCC
#define TexCreate_UAV                            ETextureCreateFlags::UAV
#define TexCreate_Presentable                    ETextureCreateFlags::Presentable
#define TexCreate_CPUReadback                    ETextureCreateFlags::CPUReadback
#define TexCreate_OfflineProcessed               ETextureCreateFlags::OfflineProcessed
#define TexCreate_FastVRAM                       ETextureCreateFlags::FastVRAM
#define TexCreate_HideInVisualizeTexture         ETextureCreateFlags::HideInVisualizeTexture
#define TexCreate_Virtual                        ETextureCreateFlags::Virtual
#define TexCreate_TargetArraySlicesIndependently ETextureCreateFlags::TargetArraySlicesIndependently
#define TexCreate_Shared                         ETextureCreateFlags::Shared
#define TexCreate_NoFastClear                    ETextureCreateFlags::NoFastClear
#define TexCreate_DepthStencilResolveTarget      ETextureCreateFlags::DepthStencilResolveTarget
#define TexCreate_Streamable                     ETextureCreateFlags::Streamable
#define TexCreate_NoFastClearFinalize            ETextureCreateFlags::NoFastClearFinalize
#define TexCreate_ReduceMemoryWithTilingMode     ETextureCreateFlags::ReduceMemoryWithTilingMode
#define TexCreate_Transient                      ETextureCreateFlags::Transient
#define TexCreate_AtomicCompatible               ETextureCreateFlags::AtomicCompatible
#define TexCreate_External               		 ETextureCreateFlags::External
#define TexCreate_MultiGPUGraphIgnore            ETextureCreateFlags::MultiGPUGraphIgnore
#define TexCreate_ReservedResource               ETextureCreateFlags::ReservedResource
#define TexCreate_ImmediateCommit                ETextureCreateFlags::ImmediateCommit


enum class EClearDepthStencil
{
	Depth,
	Stencil,
	DepthStencil,
};


enum class ERHIDescriptorHeapType : UINT8
{
	Standard,
	Sampler,
	RenderTarget,
	DepthStencil,
	Count,
	Invalid = MAX_uint8
};

struct FRHIDescriptorHandle
{
	FRHIDescriptorHandle() = default;
	FRHIDescriptorHandle(ERHIDescriptorHeapType InType, UINT32 InIndex)
		: Index(InIndex)
		, Type((UINT8)InType)
	{
	}
	FRHIDescriptorHandle(UINT8 InType, UINT32 InIndex)
		: Index(InIndex)
		, Type(InType)
	{
	}

	inline UINT32                 GetIndex() const { return Index; }
	inline ERHIDescriptorHeapType GetType() const { return (ERHIDescriptorHeapType)Type; }
	inline UINT8                  GetRawType() const { return Type; }

	inline bool IsValid() const { return Index != MAX_uint32 && Type != (UINT8)ERHIDescriptorHeapType::Invalid; }

private:
	UINT32    Index{ MAX_uint32 };
	UINT8     Type{ (UINT8)ERHIDescriptorHeapType::Invalid };
};


enum class EResourceTransitionFlags
{
	None                = 0,

	MaintainCompression = 1 << 0, // Specifies that the transition should not decompress the resource, allowing us to read a compressed resource directly in its compressed state.
	Discard				= 1 << 1, // Specifies that the data in the resource should be discarded during the transition - used for transient resource acquire when the resource will be fully overwritten
	Clear				= 1 << 2, // Specifies that the data in the resource should be cleared during the transition - used for transient resource acquire when the resource might not be fully overwritten

	Last = Clear,
	Mask = (Last << 1) - 1
};
ENUM_CLASS_FLAGS(EResourceTransitionFlags);


inline ERHIResourceType GetRHIResourceType(ETextureDimension Dimension)
{
	switch (Dimension)
	{
	case ETextureDimension::Texture2D:
		return ERHIResourceType::RRT_Texture2D;
	case ETextureDimension::Texture2DArray:
		return ERHIResourceType::RRT_Texture2DArray;
	case ETextureDimension::Texture3D:
		return ERHIResourceType::RRT_Texture3D;
	case ETextureDimension::TextureCube:
	case ETextureDimension::TextureCubeArray:
		return ERHIResourceType::RRT_TextureCube;
	}
	return ERHIResourceType::RRT_None;
}

/** Screen Resolution */
struct FScreenResolutionRHI
{
	UINT32	Width;
	UINT32	Height;
	UINT32	RefreshRate;
};



enum class ERenderTargetLoadAction : uint8
{
	// Untouched contents of the render target are undefined. Any existing content is not preserved.
	ENoAction,

	// Existing contents are preserved.
	ELoad,

	// The render target is cleared to the fast clear value specified on the resource.
	EClear,

	Num,
	NumBits = 2,
};
static_assert((uint32)ERenderTargetLoadAction::Num <= (1 << (uint32)ERenderTargetLoadAction::NumBits), "ERenderTargetLoadAction::Num will not fit on ERenderTargetLoadAction::NumBits");


/**
 * Action to take when a render target is unset or at the end of a pass. 
 */
enum class ERenderTargetStoreAction : uint8
{
	// Contents of the render target emitted during the pass are not stored back to memory.
	ENoAction,

	// Contents of the render target emitted during the pass are stored back to memory.
	EStore,

	// Contents of the render target emitted during the pass are resolved using a box filter and stored back to memory.
	EMultisampleResolve,

	Num,
	NumBits = 2,
};
static_assert((uint32)ERenderTargetStoreAction::Num <= (1 << (uint32)ERenderTargetStoreAction::NumBits), "ERenderTargetStoreAction::Num will not fit on ERenderTargetStoreAction::NumBits");


enum EVRSAxisShadingRate : uint8
{
	VRSASR_1X = 0x0,
	VRSASR_2X = 0x1,
	VRSASR_4X = 0x2,
};

enum EVRSShadingRate : uint8
{
	VRSSR_1x1  = (VRSASR_1X << 2) + VRSASR_1X,
	VRSSR_1x2  = (VRSASR_1X << 2) + VRSASR_2X,
	VRSSR_2x1  = (VRSASR_2X << 2) + VRSASR_1X,
	VRSSR_2x2  = (VRSASR_2X << 2) + VRSASR_2X,
	VRSSR_2x4  = (VRSASR_2X << 2) + VRSASR_4X,
	VRSSR_4x2  = (VRSASR_4X << 2) + VRSASR_2X,
	VRSSR_4x4  = (VRSASR_4X << 2) + VRSASR_4X,
	
	VRSSR_Last  = VRSSR_4x4
};

enum EVRSRateCombiner : uint8
{
	VRSRB_Passthrough,
	VRSRB_Override,
	VRSRB_Min,
	VRSRB_Max,
	VRSRB_Sum,
};

enum EVRSImageDataType : uint8
{
	VRSImage_NotSupported,		// Image-based Variable Rate Shading is not supported on the current device/platform.
	VRSImage_Palette,			// Image-based VRS uses a palette of discrete, enumerated values to describe shading rate per tile.
	VRSImage_Fractional,		// Image-based VRS uses a floating point value to describe shading rate in X/Y (e.g. 1.0f is full rate, 0.5f is half-rate, 0.25f is 1/4 rate, etc).
};

