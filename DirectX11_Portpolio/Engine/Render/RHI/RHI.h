#pragma once
#include "HeaderCollection.h"

struct FVertexElement
{
	UINT8 StreamIndex;
	UINT8 Offset;
	EVertexElementType Type;
	UINT8 AttributeIndex;
	UINT16 Stride;
	/**
	 * Whether to use instance index or vertex index to consume the element.  
	 * eg if bUseInstanceIndex is 0, the element will be repeated for every instance.
	 */
	UINT16 bUseInstanceIndex;

	FVertexElement() {}
	FVertexElement(UINT8 InStreamIndex,UINT8 InOffset,EVertexElementType InType,UINT8 InAttributeIndex,UINT16 InStride,bool bInUseInstanceIndex = false):
		StreamIndex(InStreamIndex),
		Offset(InOffset),
		Type(InType),
		AttributeIndex(InAttributeIndex),
		Stride(InStride),
		bUseInstanceIndex(bInUseInstanceIndex)
	{}

	bool operator==(const FVertexElement& Other) const
	{
		return (StreamIndex		== Other.StreamIndex &&
				Offset			== Other.Offset &&
				Type			== Other.Type &&
				AttributeIndex	== Other.AttributeIndex &&
				Stride			== Other.Stride &&
				bUseInstanceIndex == Other.bUseInstanceIndex);
	}


	string ToString() const;
	void FromString(const string& Src);

};

struct FSamplerStateInitializerRHI
{
    FSamplerStateInitializerRHI() {}
    FSamplerStateInitializerRHI(
        ESamplerFilter InFilter,
        ESamplerAddressMode InAddressU = AM_Wrap,
        ESamplerAddressMode InAddressV = AM_Wrap,
        ESamplerAddressMode InAddressW = AM_Wrap,
        float InMipBias = 0,
        INT32 InMaxAnisotropy = 0,
        float InMinMipLevel = 0,
        float InMaxMipLevel = FLT_MAX,
        UINT32 InBorderColor = 0,
        /** Only supported in D3D11 */
        ESamplerCompareFunction InSamplerComparisonFunction = SCF_Never
        )
    :	Filter(InFilter)
    ,	AddressU(InAddressU)
    ,	AddressV(InAddressV)
    ,	AddressW(InAddressW)
    ,	MipBias(InMipBias)
    ,	MinMipLevel(InMinMipLevel)
    ,	MaxMipLevel(InMaxMipLevel)
    ,	MaxAnisotropy(InMaxAnisotropy)
    ,	BorderColor(InBorderColor)
    ,	SamplerComparisonFunction(InSamplerComparisonFunction)
    {
    }
    ESamplerFilter Filter = SF_Point;
    ESamplerAddressMode AddressU = AM_Wrap;
    ESamplerAddressMode AddressV = AM_Wrap;
    ESamplerAddressMode AddressW = AM_Wrap;
    float MipBias = 0.0f;
    float MinMipLevel = 0.0f;
    float MaxMipLevel = FLT_MAX;
    INT32 MaxAnisotropy = 0;
    UINT32 BorderColor = 0;
    ESamplerCompareFunction SamplerComparisonFunction = SCF_Never;


    friend UINT32 GetTypeHash(const FSamplerStateInitializerRHI& Initializer);
    friend bool operator== (const FSamplerStateInitializerRHI& A, const FSamplerStateInitializerRHI& B);
};

struct FRasterizerStateInitializerRHI
{
    ERasterizerFillMode FillMode = FM_Point;
    ERasterizerCullMode CullMode = CM_None;
    float DepthBias = 0.0f;
    float SlopeScaleDepthBias = 0.0f;
    ERasterizerDepthClipMode DepthClipMode = ERasterizerDepthClipMode::DepthClip;
    bool bAllowMSAA = false;
    bool bEnableLineAA = false;

    FRasterizerStateInitializerRHI() = default;

    FRasterizerStateInitializerRHI(ERasterizerFillMode InFillMode, ERasterizerCullMode InCullMode, bool bInAllowMSAA, bool bInEnableLineAA)
        : FillMode(InFillMode)
        , CullMode(InCullMode)
        , bAllowMSAA(bInAllowMSAA)
        , bEnableLineAA(bInEnableLineAA)
    {
    }

    FRasterizerStateInitializerRHI(ERasterizerFillMode InFillMode, ERasterizerCullMode InCullMode, float InDepthBias, float InSlopeScaleDepthBias, ERasterizerDepthClipMode InDepthClipMode, bool bInAllowMSAA, bool bInEnableLineAA)
        : FillMode(InFillMode)
        , CullMode(InCullMode)
        , DepthBias(InDepthBias)
        , SlopeScaleDepthBias(InSlopeScaleDepthBias)
        , DepthClipMode(InDepthClipMode)
        , bAllowMSAA(bInAllowMSAA)
        , bEnableLineAA(bInEnableLineAA)
    {
    }

    friend UINT32 GetTypeHash(const FRasterizerStateInitializerRHI& Initializer);
    friend bool operator== (const FRasterizerStateInitializerRHI& A, const FRasterizerStateInitializerRHI& B);
};

struct FDepthStencilStateInitializerRHI
{
	bool bEnableDepthWrite;
	ECompareFunction DepthTest;

	bool bEnableFrontFaceStencil;
	ECompareFunction FrontFaceStencilTest;
	EStencilOp FrontFaceStencilFailStencilOp;
	EStencilOp FrontFaceDepthFailStencilOp;
	EStencilOp FrontFacePassStencilOp;
	bool bEnableBackFaceStencil;
	ECompareFunction BackFaceStencilTest;
	EStencilOp BackFaceStencilFailStencilOp;
	EStencilOp BackFaceDepthFailStencilOp;
	EStencilOp BackFacePassStencilOp;
	UINT8 StencilReadMask;
	UINT8 StencilWriteMask;

	FDepthStencilStateInitializerRHI(
		bool bInEnableDepthWrite = true,
		ECompareFunction InDepthTest = CF_LessEqual,
		bool bInEnableFrontFaceStencil = false,
		ECompareFunction InFrontFaceStencilTest = CF_Always,
		EStencilOp InFrontFaceStencilFailStencilOp = SO_Keep,
		EStencilOp InFrontFaceDepthFailStencilOp = SO_Keep,
		EStencilOp InFrontFacePassStencilOp = SO_Keep,
		bool bInEnableBackFaceStencil = false,
		ECompareFunction InBackFaceStencilTest = CF_Always,
		EStencilOp InBackFaceStencilFailStencilOp = SO_Keep,
		EStencilOp InBackFaceDepthFailStencilOp = SO_Keep,
		EStencilOp InBackFacePassStencilOp = SO_Keep,
		UINT8 InStencilReadMask = 0xFF,
		UINT8 InStencilWriteMask = 0xFF
		)
	: bEnableDepthWrite(bInEnableDepthWrite)
	, DepthTest(InDepthTest)
	, bEnableFrontFaceStencil(bInEnableFrontFaceStencil)
	, FrontFaceStencilTest(InFrontFaceStencilTest)
	, FrontFaceStencilFailStencilOp(InFrontFaceStencilFailStencilOp)
	, FrontFaceDepthFailStencilOp(InFrontFaceDepthFailStencilOp)
	, FrontFacePassStencilOp(InFrontFacePassStencilOp)
	, bEnableBackFaceStencil(bInEnableBackFaceStencil)
	, BackFaceStencilTest(InBackFaceStencilTest)
	, BackFaceStencilFailStencilOp(InBackFaceStencilFailStencilOp)
	, BackFaceDepthFailStencilOp(InBackFaceDepthFailStencilOp)
	, BackFacePassStencilOp(InBackFacePassStencilOp)
	, StencilReadMask(InStencilReadMask)
	, StencilWriteMask(InStencilWriteMask)
	{}
	
	

	 friend UINT32 GetTypeHash(const FDepthStencilStateInitializerRHI& Initializer);
	 friend bool operator== (const FDepthStencilStateInitializerRHI& A, const FDepthStencilStateInitializerRHI& B);
	
	 string ToString() const;
	 void FromString(const string& Src);
	
};

class FBlendStateInitializerRHI
{
public:

	struct FRenderTarget
	{
		enum
		{
			NUM_STRING_FIELDS = 7
		};
		EBlendOperation ColorBlendOp;
		EBlendFactor ColorSrcBlend;
		EBlendFactor ColorDestBlend;
		EBlendOperation AlphaBlendOp;
		EBlendFactor AlphaSrcBlend;
		EBlendFactor AlphaDestBlend;
		EColorWriteMask ColorWriteMask;
		
		FRenderTarget(
			EBlendOperation InColorBlendOp = BO_Add,
			EBlendFactor InColorSrcBlend = BF_One,
			EBlendFactor InColorDestBlend = BF_Zero,
			EBlendOperation InAlphaBlendOp = BO_Add,
			EBlendFactor InAlphaSrcBlend = BF_One,
			EBlendFactor InAlphaDestBlend = BF_Zero,
			EColorWriteMask InColorWriteMask = CW_RGBA
			)
		: ColorBlendOp(InColorBlendOp)
		, ColorSrcBlend(InColorSrcBlend)
		, ColorDestBlend(InColorDestBlend)
		, AlphaBlendOp(InAlphaBlendOp)
		, AlphaSrcBlend(InAlphaSrcBlend)
		, AlphaDestBlend(InAlphaDestBlend)
		, ColorWriteMask(InColorWriteMask)
		{}
		
		
		
		 string ToString() const;

	};

	FBlendStateInitializerRHI() {}

	FBlendStateInitializerRHI(const FRenderTarget& InRenderTargetBlendState, bool bInUseAlphaToCoverage = false)
	:	bUseIndependentRenderTargetBlendStates(false)
	,	bUseAlphaToCoverage(bInUseAlphaToCoverage)
	{
		RenderTargets[0] = InRenderTargetBlendState;
	}

	template<UINT32 NumRenderTargets>
	FBlendStateInitializerRHI(const std::array<FRenderTarget,NumRenderTargets>& InRenderTargetBlendStates, bool bInUseAlphaToCoverage = false)
	:	bUseIndependentRenderTargetBlendStates(NumRenderTargets > 1)
	,	bUseAlphaToCoverage(bInUseAlphaToCoverage)
	{
		static_assert(NumRenderTargets <= MaxSimultaneousRenderTargets, "Too many render target blend states.");

		for(UINT32 RenderTargetIndex = 0;RenderTargetIndex < NumRenderTargets;++RenderTargetIndex)
		{
			RenderTargets[RenderTargetIndex] = InRenderTargetBlendStates[RenderTargetIndex];
		}
	}

	std::array<FRenderTarget,MaxSimultaneousRenderTargets> RenderTargets;
	bool bUseIndependentRenderTargetBlendStates;
	bool bUseAlphaToCoverage;
	

	 friend bool operator== (const FBlendStateInitializerRHI::FRenderTarget& A, const FBlendStateInitializerRHI::FRenderTarget& B);
	 friend bool operator== (const FBlendStateInitializerRHI& A, const FBlendStateInitializerRHI& B);
	
	 string ToString() const;
	 void FromString(const string& Src);
};

struct FViewportBounds
{
	float	TopLeftX;
	float	TopLeftY;
	float	Width;
	float	Height;
	float	MinDepth;
	float	MaxDepth;

	FViewportBounds() {}

	FViewportBounds(float InTopLeftX, float InTopLeftY, float InWidth, float InHeight, float InMinDepth = 0.0f, float InMaxDepth = 1.0f)
		:TopLeftX(InTopLeftX), TopLeftY(InTopLeftY), Width(InWidth), Height(InHeight), MinDepth(InMinDepth), MaxDepth(InMaxDepth)
	{
	}
};

