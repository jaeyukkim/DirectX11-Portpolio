#pragma once
#include "HeaderCollection.h"

namespace ERHIFeatureLevel { enum Type : int; }
enum EShaderPlatform : uint16;
enum ECubeFace : uint32;

enum EPixelFormat : uint8;
enum class EPixelFormatChannelFlags : uint8;

enum class EBufferUsageFlags : uint32;
enum class ETextureCreateFlags : uint64;

// Command Lists
//class FRHICommandListBase;
//class FRHIComputeCommandList;
//class FRHICommandList;
//class FRHICommandListImmediate;

//struct FRHIResourceUpdateInfo;
//struct FRHIResourceUpdateBatcher;

struct FSamplerStateInitializerRHI;
struct FRasterizerStateInitializerRHI;
struct FDepthStencilStateInitializerRHI;
class FBlendStateInitializerRHI;

// Resources
class FRHIAmplificationShader;
class FRHIBlendState;
class FRHIBoundShaderState;
class FRHIBuffer;
//class FRHIComputeFence;
//class FRHIComputePipelineState;
//class FRHIComputeShader;
class FRHICustomPresent;
class FRHIDepthStencilState;
class FRHIGeometryShader;
//class FRHIGPUFence;
class FRHIGraphicsPipelineState;
class FRHIMeshShader;
class FRHIPipelineBinaryLibrary;
class FRHIPixelShader;
class FRHIRasterizerState;
//class FRHIRayTracingGeometry;
//class FRHIRayTracingPipelineState;
//class FRHIRayTracingScene;
//class FRHIRayTracingShader;
//class FRHIRenderQuery;
//class FRHIRenderQueryPool;
class FRHIResource;
class FRHISamplerState;
class FRHIShader;
//class FRHIShaderLibrary;
class FRHIShaderResourceView;
class FRHIStagingBuffer;
class FRHITexture;
//class FRHITextureReference;
//class FRHITimestampCalibrationQuery;
//class FRHIUniformBuffer;
class FRHIUnorderedAccessView;
class FRHIVertexDeclaration;
class FRHIVertexShader;
class FRHIViewableResource;
class FRHIViewport;

//struct FRHIUniformBufferLayout;

// Pointers

//using FAmplificationShaderRHIRef       = shared_ptr<FRHIAmplificationShader>;
using FBlendStateRHIRef                = shared_ptr<FRHIBlendState>;
using FBoundShaderStateRHIRef          = shared_ptr<FRHIBoundShaderState>;
using FBufferRHIRef                    = shared_ptr<FRHIBuffer>;
//using FComputeFenceRHIRef              = shared_ptr<FRHIComputeFence>;
//using FComputePipelineStateRHIRef      = shared_ptr<FRHIComputePipelineState>;
//using FComputeShaderRHIRef             = shared_ptr<FRHIComputeShader>;
using FCustomPresentRHIRef             = shared_ptr<FRHICustomPresent>;
using FDepthStencilStateRHIRef         = shared_ptr<FRHIDepthStencilState>;
using FGeometryShaderRHIRef            = shared_ptr<FRHIGeometryShader>;
//using FGPUFenceRHIRef                  = shared_ptr<FRHIGPUFence>;
using FGraphicsPipelineStateRHIRef     = shared_ptr<FRHIGraphicsPipelineState>;
using FMeshShaderRHIRef                = shared_ptr<FRHIMeshShader>;
using FPixelShaderRHIRef               = shared_ptr<FRHIPixelShader>;
using FRasterizerStateRHIRef           = shared_ptr<FRHIRasterizerState>;
//using FRayTracingGeometryRHIRef        = shared_ptr<FRHIRayTracingGeometry>;
//using FRayTracingPipelineStateRHIRef   = shared_ptr<FRHIRayTracingPipelineState>;
//using FRayTracingSceneRHIRef           = shared_ptr<FRHIRayTracingScene>;
//using FRayTracingShaderRHIRef          = shared_ptr<FRHIRayTracingShader>;
//using FRenderQueryPoolRHIRef           = shared_ptr<FRHIRenderQueryPool>;
//using FRenderQueryRHIRef               = shared_ptr<FRHIRenderQuery>;
using FRHIPipelineBinaryLibraryRef     = shared_ptr<FRHIPipelineBinaryLibrary>;
//using FRHIShaderLibraryRef             = shared_ptr<FRHIShaderLibrary>;
using FSamplerStateRHIRef              = shared_ptr<FRHISamplerState>;
using FShaderResourceViewRHIRef        = shared_ptr<FRHIShaderResourceView>;
using FStagingBufferRHIRef             = shared_ptr<FRHIStagingBuffer>;
//using FTextureReferenceRHIRef          = shared_ptr<FRHITextureReference>;
using FTextureRHIRef                   = shared_ptr<FRHITexture>;
//using FTimestampCalibrationQueryRHIRef = shared_ptr<FRHITimestampCalibrationQuery>;
//using FUniformBufferLayoutRHIRef       = shared_ptr<const FRHIUniformBufferLayout>;
//using FUniformBufferRHIRef             = shared_ptr<FRHIUniformBuffer>;
using FUnorderedAccessViewRHIRef       = shared_ptr<FRHIUnorderedAccessView>;
using FVertexDeclarationRHIRef         = shared_ptr<FRHIVertexDeclaration>;
using FVertexShaderRHIRef              = shared_ptr<FRHIVertexShader>;
using FViewportRHIRef                  = shared_ptr<FRHIViewport>;

// Deprecated typenames
using FRHITexture2D                    = FRHITexture;
using FRHITexture2DArray               = FRHITexture;
using FRHITexture3D                    = FRHITexture;
using FRHITextureCube                  = FRHITexture;
using FTexture2DRHIRef                 = FTextureRHIRef;
using FTexture2DArrayRHIRef            = FTextureRHIRef;
using FTexture3DRHIRef                 = FTextureRHIRef;
using FTextureCubeRHIRef               = FTextureRHIRef;
