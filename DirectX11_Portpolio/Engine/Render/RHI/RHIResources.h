#pragma once
#include "PixelFormat.h"
#include "RHIAccess.h"
#include "RHITransition.h"
#include "Libraries/Utility/Crc.h"


class FRHIResource
{
public:
	FRHIResource(ERHIResourceType InResourceType);
	virtual ~FRHIResource();

private:
	const ERHIResourceType ResourceType;
};

enum class EClearBinding
{
	ENoneBound, //no clear color associated with this target.  Target will not do hardware clears on most platforms
	EColorBound, //target has a clear color bound.  Clears will use the bound color, and do hardware clears.
	EDepthStencilBound, //target has a depthstencil value bound.  Clears will use the bound values and do hardware clears.
};

class FExclusiveDepthStencil
{
public:
	enum Type
	{
		// don't use those directly, use the combined versions below
		// 4 bits are used for depth and 4 for stencil to make the hex value readable and non overlapping
		DepthNop = 0x00,
		DepthRead = 0x01,
		DepthWrite = 0x02,
		DepthMask = 0x0f,
		StencilNop = 0x00,
		StencilRead = 0x10,
		StencilWrite = 0x20,
		StencilMask = 0xf0,

		// use those:
		DepthNop_StencilNop = DepthNop + StencilNop,
		DepthRead_StencilNop = DepthRead + StencilNop,
		DepthWrite_StencilNop = DepthWrite + StencilNop,
		DepthNop_StencilRead = DepthNop + StencilRead,
		DepthRead_StencilRead = DepthRead + StencilRead,
		DepthWrite_StencilRead = DepthWrite + StencilRead,
		DepthNop_StencilWrite = DepthNop + StencilWrite,
		DepthRead_StencilWrite = DepthRead + StencilWrite,
		DepthWrite_StencilWrite = DepthWrite + StencilWrite,
	};

private:
	Type Value;

public:
	// constructor
	FExclusiveDepthStencil(Type InValue = DepthNop_StencilNop)
		: Value(InValue)
	{
	}

	inline bool IsUsingDepthStencil() const
	{
		return Value != DepthNop_StencilNop;
	}
	inline bool IsUsingDepth() const
	{
		return (ExtractDepth() != DepthNop);
	}
	inline bool IsUsingStencil() const
	{
		return (ExtractStencil() != StencilNop);
	}
	inline bool IsDepthWrite() const
	{
		return ExtractDepth() == DepthWrite;
	}
	inline bool IsDepthRead() const
	{
		return ExtractDepth() == DepthRead;
	}
	inline bool IsStencilWrite() const
	{
		return ExtractStencil() == StencilWrite;
	}
	inline bool IsStencilRead() const
	{
		return ExtractStencil() == StencilRead;
	}

	inline bool IsAnyWrite() const
	{
		return IsDepthWrite() || IsStencilWrite();
	}

	inline void SetDepthWrite()
	{
		Value = (Type)(ExtractStencil() | DepthWrite);
	}
	inline void SetStencilWrite()
	{
		Value = (Type)(ExtractDepth() | StencilWrite);
	}
	inline void SetDepthStencilWrite(bool bDepth, bool bStencil)
	{
		Value = DepthNop_StencilNop;

		if (bDepth)
		{
			SetDepthWrite();
		}
		if (bStencil)
		{
			SetStencilWrite();
		}
	}
	bool operator==(const FExclusiveDepthStencil& rhs) const
	{
		return Value == rhs.Value;
	}

	bool operator != (const FExclusiveDepthStencil& RHS) const
	{
		return Value != RHS.Value;
	}

	inline bool IsValid(FExclusiveDepthStencil& Current) const
	{
		Type Depth = ExtractDepth();

		if (Depth != DepthNop && Depth != Current.ExtractDepth())
		{
			return false;
		}

		Type Stencil = ExtractStencil();

		if (Stencil != StencilNop && Stencil != Current.ExtractStencil())
		{
			return false;
		}

		return true;
	}

	inline void GetAccess(ERHIAccess& DepthAccess, ERHIAccess& StencilAccess) const
	{
		DepthAccess = ERHIAccess::None;

		// SRV access is allowed whilst a depth stencil target is "readable".
		constexpr ERHIAccess DSVReadOnlyMask =
			ERHIAccess::DSVRead;

		// If write access is required, only the depth block can access the resource.
		constexpr ERHIAccess DSVReadWriteMask =
			ERHIAccess::DSVRead |
			ERHIAccess::DSVWrite;

		if (IsUsingDepth())
		{
			DepthAccess = IsDepthWrite() ? DSVReadWriteMask : DSVReadOnlyMask;
		}

		StencilAccess = ERHIAccess::None;

		if (IsUsingStencil())
		{
			StencilAccess = IsStencilWrite() ? DSVReadWriteMask : DSVReadOnlyMask;
		}
	}

	template <typename TFunction>
	inline void EnumerateSubresources(TFunction Function) const
	{
		if (!IsUsingDepthStencil())
		{
			return;
		}

		ERHIAccess DepthAccess = ERHIAccess::None;
		ERHIAccess StencilAccess = ERHIAccess::None;
		GetAccess(DepthAccess, StencilAccess);

		// Same depth / stencil state; single subresource.
		if (DepthAccess == StencilAccess)
		{
			Function(DepthAccess, FRHITransitionInfo::kAllSubresources);
		}
		// Separate subresources for depth / stencil.
		else
		{
			if (DepthAccess != ERHIAccess::None)
			{
				Function(DepthAccess, FRHITransitionInfo::kDepthPlaneSlice);
			}
			if (StencilAccess != ERHIAccess::None)
			{
				Function(StencilAccess, FRHITransitionInfo::kStencilPlaneSlice);
			}
		}
	}

	/**
	* Returns a new FExclusiveDepthStencil to be used to transition a depth stencil resource to readable.
	* If the depth or stencil is already in a readable state, that particular component is returned as Nop,
	* to avoid unnecessary subresource transitions.
	*/
	inline FExclusiveDepthStencil GetReadableTransition() const
	{
		FExclusiveDepthStencil::Type NewDepthState = IsDepthWrite()
			? FExclusiveDepthStencil::DepthRead
			: FExclusiveDepthStencil::DepthNop;

		FExclusiveDepthStencil::Type NewStencilState = IsStencilWrite()
			? FExclusiveDepthStencil::StencilRead
			: FExclusiveDepthStencil::StencilNop;

		return (FExclusiveDepthStencil::Type)(NewDepthState | NewStencilState);
	}

	/**
	* Returns a new FExclusiveDepthStencil to be used to transition a depth stencil resource to readable.
	* If the depth or stencil is already in a readable state, that particular component is returned as Nop,
	* to avoid unnecessary subresource transitions.
	*/
	inline FExclusiveDepthStencil GetWritableTransition() const
	{
		FExclusiveDepthStencil::Type NewDepthState = IsDepthRead()
			? FExclusiveDepthStencil::DepthWrite
			: FExclusiveDepthStencil::DepthNop;

		FExclusiveDepthStencil::Type NewStencilState = IsStencilRead()
			? FExclusiveDepthStencil::StencilWrite
			: FExclusiveDepthStencil::StencilNop;

		return (FExclusiveDepthStencil::Type)(NewDepthState | NewStencilState);
	}

	uint32 GetIndex() const
	{
		// Note: The array to index has views created in that specific order.

		// we don't care about the Nop versions so less views are needed
		// we combine Nop and Write
		switch (Value)
		{
		case DepthWrite_StencilNop:
		case DepthNop_StencilWrite:
		case DepthWrite_StencilWrite:
		case DepthNop_StencilNop:
			return 0; // old DSAT_Writable

		case DepthRead_StencilNop:
		case DepthRead_StencilWrite:
			return 1; // old DSAT_ReadOnlyDepth

		case DepthNop_StencilRead:
		case DepthWrite_StencilRead:
			return 2; // old DSAT_ReadOnlyStencil

		case DepthRead_StencilRead:
			return 3; // old DSAT_ReadOnlyDepthAndStencil
		}
		// should never happen
		Check(0);
		return -1;
	}
	static const uint32 MaxIndex = 4;

private:
	inline Type ExtractDepth() const
	{
		return (Type)(Value & DepthMask);
	}
	inline Type ExtractStencil() const
	{
		return (Type)(Value & StencilMask);
	}
};

struct FClearValueBinding
{
	struct DSVAlue
	{
		float Depth;
		uint32 Stencil;
	};

	FClearValueBinding()
		: ColorBinding(EClearBinding::EColorBound)
	{
		Value.Color[0] = 0.0f;
		Value.Color[1] = 0.0f;
		Value.Color[2] = 0.0f;
		Value.Color[3] = 0.0f;
	}

	FClearValueBinding(EClearBinding NoBinding)
		: ColorBinding(NoBinding)
	{
		Check(ColorBinding == EClearBinding::ENoneBound);

		Value.Color[0] = 0.0f;
		Value.Color[1] = 0.0f;
		Value.Color[2] = 0.0f;
		Value.Color[3] = 0.0f;

		Value.DSValue.Depth = 0.0f;
		Value.DSValue.Stencil = 0;
	}

	explicit FClearValueBinding(const Color& InClearColor)
		: ColorBinding(EClearBinding::EColorBound)
	{
		Value.Color[0] = InClearColor.R();
		Value.Color[1] = InClearColor.G();
		Value.Color[2] = InClearColor.B();
		Value.Color[3] = InClearColor.A();
	}

	explicit FClearValueBinding(float DepthClearValue, uint32 StencilClearValue = 0)
		: ColorBinding(EClearBinding::EDepthStencilBound)
	{
		Value.DSValue.Depth = DepthClearValue;
		Value.DSValue.Stencil = StencilClearValue;
	}

	Color GetClearColor() const
	{
		Check(ColorBinding == EClearBinding::EColorBound);
		return Color(Value.Color[0], Value.Color[1], Value.Color[2], Value.Color[3]);
	}

	void GetDepthStencil(float& OutDepth, uint32& OutStencil) const
	{
		Check(ColorBinding == EClearBinding::EDepthStencilBound);
		OutDepth = Value.DSValue.Depth;
		OutStencil = Value.DSValue.Stencil;
	}

	bool operator==(const FClearValueBinding& Other) const
	{
		if (ColorBinding == Other.ColorBinding)
		{
			if (ColorBinding == EClearBinding::EColorBound)
			{
				return
					Value.Color[0] == Other.Value.Color[0] &&
					Value.Color[1] == Other.Value.Color[1] &&
					Value.Color[2] == Other.Value.Color[2] &&
					Value.Color[3] == Other.Value.Color[3];

			}
			if (ColorBinding == EClearBinding::EDepthStencilBound)
			{
				return
					Value.DSValue.Depth == Other.Value.DSValue.Depth &&
					Value.DSValue.Stencil == Other.Value.DSValue.Stencil;
			}
			return true;
		}
		return false;
	}

	friend inline uint32 GetTypeHash(FClearValueBinding const& Binding)
	{
		uint32 Hash = GetTypeHash(Binding.ColorBinding);

		if (Binding.ColorBinding == EClearBinding::EColorBound)
		{
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.Color[0]));
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.Color[1]));
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.Color[2]));
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.Color[3]));
		}
		else if (Binding.ColorBinding == EClearBinding::EDepthStencilBound)
		{
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.DSValue.Depth  ));
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.DSValue.Stencil));
		}

		return Hash;
	}

	EClearBinding ColorBinding;

	union ClearValueType
	{
		float Color[4];
		DSVAlue DSValue;
	} Value;

	// common clear values
	static const FClearValueBinding None;
	static const FClearValueBinding Black;
	static const FClearValueBinding BlackMaxAlpha;
	static const FClearValueBinding White;
	static const FClearValueBinding Transparent;
	static const FClearValueBinding DepthOne;
	static const FClearValueBinding DepthZero;
	static const FClearValueBinding DepthNear;
	static const FClearValueBinding DepthFar;	
	static const FClearValueBinding Green;
	static const FClearValueBinding DefaultNormal8Bit;
};


class FRHISamplerState : public FRHIResource 
{
public:
	FRHISamplerState() : FRHIResource(RRT_SamplerState) {}
	virtual bool IsImmutable() const { return false; }
	virtual FRHIDescriptorHandle GetBindlessHandle() const { return FRHIDescriptorHandle(); }
};

class FRHIRasterizerState : public FRHIResource
{
public:
	FRHIRasterizerState() : FRHIResource(RRT_RasterizerState) {}
	virtual bool GetInitializer(struct FRasterizerStateInitializerRHI& Init) { return false; }
};

class FRHIDepthStencilState : public FRHIResource
{
public:
	FRHIDepthStencilState() : FRHIResource(RRT_DepthStencilState) {}
	virtual bool GetInitializer(struct FDepthStencilStateInitializerRHI& Init) { return false; }
};

class FRHIBlendState : public FRHIResource
{
public:
	FRHIBlendState() : FRHIResource(RRT_BlendState) {}
	virtual bool GetInitializer(class FBlendStateInitializerRHI& Init) { return false; }
};

typedef array<struct FVertexElement,MaxVertexElementCount> FVertexDeclarationElementList;

class FRHIVertexDeclaration : public FRHIResource
{
public:
	FRHIVertexDeclaration() : FRHIResource(RRT_VertexDeclaration) {}
	virtual bool GetInitializer(FVertexDeclarationElementList& Init) { return false; }
	virtual UINT32 GetPrecachePSOHash() const { return 0; }
};

class FRHIBoundShaderState : public FRHIResource
{
public:
	FRHIBoundShaderState() : FRHIResource(RRT_BoundShaderState) {}
};

class FRHIShader : public FRHIResource
{
public:
	
	const TCHAR* GetShaderName() const { return TEXT("<unknown>"); }
	
	
	FRHIShader() = delete;
	FRHIShader(ERHIResourceType InResourceType, EShaderFrequency InFrequency)
		: FRHIResource(InResourceType)
		, Frequency(InFrequency)
	{
	}

	inline EShaderFrequency GetFrequency() const
	{
		return Frequency;
	}

private:

	EShaderFrequency Frequency;
};

class FRHIGraphicsShader : public FRHIShader
{
public:
	explicit FRHIGraphicsShader(ERHIResourceType InResourceType, EShaderFrequency InFrequency)
		: FRHIShader(InResourceType, InFrequency) {}
};

class FRHIVertexShader : public FRHIGraphicsShader
{
public:
	FRHIVertexShader() : FRHIGraphicsShader(RRT_VertexShader, SF_Vertex) {}
};

class FRHIHullShader : public FRHIGraphicsShader
{
public:
	FRHIHullShader() : FRHIGraphicsShader(RRT_HullShader, SF_Hull) {}
};

class FRHIDomainShader : public FRHIGraphicsShader
{
public:
	FRHIDomainShader() : FRHIGraphicsShader(RRT_DomainShader, SF_Domain) {}
};

class FRHIGeometryShader : public FRHIGraphicsShader
{
public:
	FRHIGeometryShader() : FRHIGraphicsShader(RRT_GeometryShader, SF_Geometry) {}
};

class FRHIMeshShader : public FRHIGraphicsShader
{
public:
	FRHIMeshShader() : FRHIGraphicsShader(RRT_MeshShader, SF_Mesh) {}
};

class FRHIAmplificationShader : public FRHIGraphicsShader
{
public:
	FRHIAmplificationShader() : FRHIGraphicsShader(RRT_AmplificationShader, SF_Amplification) {}
};

class FRHIPixelShader : public FRHIGraphicsShader
{
public:
	FRHIPixelShader() : FRHIGraphicsShader(RRT_PixelShader, SF_Pixel) {}
};

class FRHIRayTracingShader : public FRHIShader
{
public:
	explicit FRHIRayTracingShader(EShaderFrequency InFrequency) : FRHIShader(RRT_RayTracingShader, InFrequency) {}

	UINT32 RayTracingPayloadType = 0; // This corresponds to the ERayTracingPayloadType enum associated with the shader
	UINT32 RayTracingPayloadSize = 0; // The (maximum) size of the payload associated with this shader
};

class FRHIRayGenShader : public FRHIRayTracingShader
{
public:
	FRHIRayGenShader() : FRHIRayTracingShader(SF_RayGen) {}
};

class FRHIRayMissShader : public FRHIRayTracingShader
{
public:
	FRHIRayMissShader() : FRHIRayTracingShader(SF_RayMiss) {}
};

class FRHIRayCallableShader : public FRHIRayTracingShader
{
public:
	FRHIRayCallableShader() : FRHIRayTracingShader(SF_RayCallable) {}
};

class FRHIRayHitGroupShader : public FRHIRayTracingShader
{
public:
	FRHIRayHitGroupShader() : FRHIRayTracingShader(SF_RayHitGroup) {}
};

class FRHIGraphicsPipelineState : public FRHIResource 
{
public:
	FRHIGraphicsPipelineState() : FRHIResource(RRT_GraphicsPipelineState) {}
	
};

class FRHIViewableResource : public FRHIResource
{
public:
	// TODO (RemoveUnknowns) remove once FRHIBufferCreateDesc contains initial access.
	void SetTrackedAccess_Unsafe(ERHIAccess Access)
	{
		TrackedAccess = Access;
	}

	string GetName() const
	{
		return Name;
	}


protected:
	FRHIViewableResource(ERHIResourceType InResourceType, ERHIAccess InAccess)
		: FRHIResource(InResourceType)
		, TrackedAccess(InAccess)
	{}

	void TakeOwnership(FRHIViewableResource& Other)
	{
		TrackedAccess = Other.TrackedAccess;
	}

	void ReleaseOwnership()
	{
		TrackedAccess = ERHIAccess::Unknown;
	}

	string Name;

private:
	ERHIAccess TrackedAccess;

	//friend class FRHIComputeCommandList;
	//friend class IRHIComputeContext;
};


struct FRHIBufferDesc
{
	uint32 Size{};
	uint32 Stride{};
	EBufferUsageFlags Usage{};

	FRHIBufferDesc() = default;
	FRHIBufferDesc(uint32 InSize, uint32 InStride, EBufferUsageFlags InUsage)
		: Size  (InSize)
		, Stride(InStride)
		, Usage (InUsage)
	{}

	static FRHIBufferDesc Null()
	{
		return FRHIBufferDesc(0, 0, BUF_NullResource);
	}

	bool IsNull() const
	{
		if (EnumHasAnyFlags(Usage, BUF_NullResource))
		{
			// The null resource descriptor should have its other fields zeroed, and no additional flags.
			Check(Size == 0 && Stride == 0 && Usage == BUF_NullResource);
			return true;
		}


		return false;
	}
};


class FRHIBuffer : public FRHIViewableResource
{
public:
	/** Initialization constructor. */
	FRHIBuffer(FRHIBufferDesc const& InDesc)
		: FRHIViewableResource(RRT_Buffer, ERHIAccess::Unknown /* TODO (RemoveUnknowns): Use InitialAccess from descriptor after refactor. */)
		, Desc(InDesc)
	{}

	FRHIBufferDesc const& GetDesc() const { return Desc; }

	/** @return The number of bytes in the buffer. */
	uint32 GetSize() const { return Desc.Size; }

	/** @return The stride in bytes of the buffer. */
	uint32 GetStride() const { return Desc.Stride; }

	/** @return The usage flags used to create the buffer. */
	EBufferUsageFlags GetUsage() const { return Desc.Usage; }

	void SetName(const string& InName) { Name = InName; }

	virtual uint32 GetParentGPUIndex() const { return 0; }


protected:
	// Used by RHI implementations that may adjust internal usage flags during object construction.
	void SetUsage(EBufferUsageFlags InUsage)
	{
		Desc.Usage = InUsage;
	}

	void TakeOwnership(FRHIBuffer& Other)
	{
		FRHIViewableResource::TakeOwnership(Other);
		Desc = Other.Desc;
	}

	void ReleaseOwnership()
	{
		FRHIViewableResource::ReleaseOwnership();
		Desc = FRHIBufferDesc::Null();
	}

private:
	FRHIBufferDesc Desc;
};

//
// Textures
//

class FLastRenderTimeContainer
{
public:
	FLastRenderTimeContainer() : LastRenderTime(-FLT_MAX) {}

	double GetLastRenderTime() const { return LastRenderTime; }

	void SetLastRenderTime(double InLastRenderTime) 
	{ 
		// avoid dirty caches from redundant writes
		if (LastRenderTime != InLastRenderTime)
		{
			LastRenderTime = InLastRenderTime;
		}
	}

private:
	/** The last time the resource was rendered. */
	double LastRenderTime;
};

struct FRHITextureDesc
{
	FRHITextureDesc() = default;

	FRHITextureDesc(const FRHITextureDesc& Other)
	{
		*this = Other;
	}

	FRHITextureDesc(ETextureDimension InDimension)
		: Dimension(InDimension)
	{}
	
	FRHITextureDesc(
		  ETextureDimension   InDimension
		, ETextureCreateFlags InFlags
		, EPixelFormat        InFormat
		, FClearValueBinding  InClearValue
		, Vector2           InExtent
		, uint16              InDepth
		, uint16              InArraySize
		, uint8               InNumMips
		, uint8               InNumSamples
		, uint32              InExtData
		)
		: Flags     (InFlags     )
		, ClearValue(InClearValue)
		, ExtData   (InExtData   )
		, Extent    (InExtent    )
		, Depth     (InDepth     )
		, ArraySize (InArraySize )
		, NumMips   (InNumMips   )
		, NumSamples(InNumSamples)
		, Dimension (InDimension )
		, Format    (InFormat    )
	{}

	
	friend uint32 GetTypeHash(const FRHITextureDesc& Desc)
	{
		uint32 Hash = GetTypeHash(Desc.Dimension);
		Hash = HashCombine(Hash, GetTypeHash(Desc.Flags		));
		Hash = HashCombine(Hash, GetTypeHash(Desc.Format	));
		Hash = HashCombine(Hash, GetTypeHash(Desc.UAVFormat	));
		//Hash = HashCombine(Hash, GetTypeHash(Desc.Extent	));
		Hash = HashCombine(Hash, GetTypeHash(Desc.Depth		));
		Hash = HashCombine(Hash, GetTypeHash(Desc.ArraySize	));
		Hash = HashCombine(Hash, GetTypeHash(Desc.NumMips	));
		Hash = HashCombine(Hash, GetTypeHash(Desc.NumSamples));
		Hash = HashCombine(Hash, GetTypeHash(Desc.FastVRAMPercentage));
		Hash = HashCombine(Hash, GetTypeHash(Desc.ClearValue));
		Hash = HashCombine(Hash, GetTypeHash(Desc.ExtData   ));
	//	Hash = HashCombine(Hash, GetTypeHash(Desc.GPUMask.GetNative()));
		return Hash;
	}

	bool operator == (const FRHITextureDesc& Other) const
	{
		return Dimension  == Other.Dimension
			&& Flags      == Other.Flags
			&& Format     == Other.Format
			&& UAVFormat  == Other.UAVFormat
			&& Extent     == Other.Extent
			&& Depth      == Other.Depth
			&& ArraySize  == Other.ArraySize
			&& NumMips    == Other.NumMips
			&& NumSamples == Other.NumSamples
			&& FastVRAMPercentage == Other.FastVRAMPercentage
			&& ClearValue == Other.ClearValue
			&& ExtData    == Other.ExtData;
		//	&& GPUMask    == Other.GPUMask;
	}

	bool operator != (const FRHITextureDesc& Other) const
	{
		return !(*this == Other);
	}

	FRHITextureDesc& operator=(const FRHITextureDesc& Other)
	{
		Dimension			= Other.Dimension;
		Flags				= Other.Flags;
		Format				= Other.Format;
		UAVFormat			= Other.UAVFormat;
		Extent				= Other.Extent;
		Depth				= Other.Depth;
		ArraySize			= Other.ArraySize;
		NumMips				= Other.NumMips;
		NumSamples			= Other.NumSamples;
		ClearValue			= Other.ClearValue;
		ExtData				= Other.ExtData;
		FastVRAMPercentage	= Other.FastVRAMPercentage;
	//	GPUMask				= Other.GPUMask;

		return *this;
	}

	bool IsTexture2D() const
	{
		return Dimension == ETextureDimension::Texture2D || Dimension == ETextureDimension::Texture2DArray;
	}

	bool IsTexture3D() const
	{
		return Dimension == ETextureDimension::Texture3D;
	}

	bool IsTextureCube() const
	{
		return Dimension == ETextureDimension::TextureCube || Dimension == ETextureDimension::TextureCubeArray;
	}

	bool IsTextureArray() const
	{
		return Dimension == ETextureDimension::Texture2DArray || Dimension == ETextureDimension::TextureCubeArray;
	}

	bool IsMipChain() const
	{
		return NumMips > 1;
	}

	bool IsMultisample() const
	{
		return NumSamples > 1;
	}

	Vector2 GetSize() const
	{
		//return Vector2(Extent.x, Extent.y, Depth);
	}

	void Reset()
	{
		// Usually we don't want to propagate MSAA samples.
		NumSamples = 1;

		// Remove UAV flag for textures that don't need it (some formats are incompatible).
		Flags |= TexCreate_RenderTargetable;
		Flags &= ~(TexCreate_UAV | TexCreate_ResolveTargetable | TexCreate_DepthStencilResolveTarget | TexCreate_Memoryless);
	}

	/** Returns whether this descriptor conforms to requirements. */
	bool IsValid() const
	{
		return FRHITextureDesc::Validate(*this, /* Name = */ TEXT(""), /* bFatal = */ false);
	}

	/** Texture flags passed on to RHI texture. */
	ETextureCreateFlags Flags = TexCreate_None;

	/** Clear value to use when fast-clearing the texture. */
	FClearValueBinding ClearValue;

	/* A mask representing which GPUs to create the resource on, in a multi-GPU system. */
	//FRHIGPUMask GPUMask = FRHIGPUMask::All();

	/** Platform-specific additional data. Used for offline processed textures on some platforms. */
	uint32 ExtData = 0;

	/** Extent of the texture in x and y. */
	Vector2 Extent = Vector2(1, 1);

	/** Depth of the texture if the dimension is 3D. */
	uint16 Depth = 1;

	/** The number of array elements in the texture. (Keep at 1 if dimension is 3D). */
	uint16 ArraySize = 1;

	/** Number of mips in the texture mip-map chain. */
	uint8 NumMips = 1;

	/** Number of samples in the texture. >1 for MSAA. */
	uint8 NumSamples = 1;

	/** Texture dimension to use when creating the RHI texture. */
	ETextureDimension Dimension = ETextureDimension::Texture2D;

	/** Pixel format used to create RHI texture. */
	EPixelFormat Format = PF_Unknown;

	/** Texture format used when creating the UAV. PF_Unknown means to use the default one (same as Format). */
	EPixelFormat UAVFormat = PF_Unknown;

	/** Resource memory percentage which should be allocated onto fast VRAM (hint-only). (encoding into 8bits, 0..255 -> 0%..100%) */
	uint8 FastVRAMPercentage = 0xFF;

	/** Check the validity. */
	static bool CheckValidity(const FRHITextureDesc& Desc, const TCHAR* Name)
	{
		return FRHITextureDesc::Validate(Desc, Name, /* bFatal = */ true);
	}

	/**
	 * Returns an estimated total memory size the described texture will occupy in GPU memory.
	 * This is an estimate because it only considers the dimensions / format etc of the texture, 
	 * not any specifics about platform texture layout.
	 * 
	 * To get a true measure of a texture resource for the current running platform RHI, use RHICalcTexturePlatformSize().
	 * 
	 * @param FirstMipIndex - the index of the most detailed mip to consider in the memory size calculation. Must be < NumMips and <= LastMipIndex.
	 * @param LastMipIndex  - the index of the least detailed mip to consider in the memory size calculation. Must be < NumMips and >= FirstMipIndex.
	 */
	uint64 CalcMemorySizeEstimate(uint32 FirstMipIndex, uint32 LastMipIndex) const;

	uint64 CalcMemorySizeEstimate(uint32 FirstMipIndex = 0) const
	{
		return CalcMemorySizeEstimate(FirstMipIndex, NumMips - 1);
	}

private:
	static bool Validate(const FRHITextureDesc& Desc, const TCHAR* Name, bool bFatal);
};


using FRHITextureCreateInfo = FRHITextureDesc;

extern ERHIAccess RHIGetDefaultResourceState(ETextureCreateFlags InUsage, bool bInHasInitialData);

struct FRHITextureCreateDesc : public FRHITextureDesc
{
	static FRHITextureCreateDesc Create(const TCHAR* InDebugName, ETextureDimension InDimension)
	{
		return FRHITextureCreateDesc(InDebugName, InDimension);
	}

	static FRHITextureCreateDesc Create2D(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::Texture2D);
	}

	static FRHITextureCreateDesc Create2DArray(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::Texture2DArray);
	}

	static FRHITextureCreateDesc Create3D(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::Texture3D);
	}

	static FRHITextureCreateDesc CreateCube(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::TextureCube);
	}

	static FRHITextureCreateDesc CreateCubeArray(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::TextureCubeArray);
	}

	static FRHITextureCreateDesc Create2D(const TCHAR* DebugName, Vector2 Size, EPixelFormat Format)
	{
		return Create2D(DebugName)
			.SetExtent(Size)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc Create2D(const TCHAR* DebugName, int32 SizeX, int32 SizeY, EPixelFormat Format)
	{
		return Create2D(DebugName)
			.SetExtent(SizeX, SizeY)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc Create2DArray(const TCHAR* DebugName, Vector2 Size, uint16 ArraySize, EPixelFormat Format)
	{
		return Create2DArray(DebugName)
			.SetExtent(Size)
			.SetFormat(Format)
			.SetArraySize((uint16)ArraySize);
	}

	static FRHITextureCreateDesc Create2DArray(const TCHAR* DebugName, int32 SizeX, int32 SizeY, int32 ArraySize, EPixelFormat Format)
	{
		return Create2DArray(DebugName)
			.SetExtent(SizeX, SizeY)
			.SetFormat(Format)
			.SetArraySize((uint16)ArraySize);
	}

	static FRHITextureCreateDesc Create3D(const TCHAR* DebugName, Vector3 Size, EPixelFormat Format)
	{
		return Create3D(DebugName)
			.SetExtent(Size.x, Size.y)
			.SetDepth((uint16)Size.z)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc Create3D(const TCHAR* DebugName, int32 SizeX, int32 SizeY, int32 SizeZ, EPixelFormat Format)
	{
		return Create3D(DebugName)
			.SetExtent(SizeX, SizeY)
			.SetDepth((uint16)SizeZ)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc CreateCube(const TCHAR* DebugName, uint32 Size, EPixelFormat Format)
	{
		return CreateCube(DebugName)
			.SetExtent(Size)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc CreateCubeArray(const TCHAR* DebugName, uint32 Size, uint16 ArraySize, EPixelFormat Format)
	{
		return CreateCubeArray(DebugName)
			.SetExtent(Size)
			.SetFormat(Format)
			.SetArraySize((uint16)ArraySize);
	}

	FRHITextureCreateDesc() = default;

	// Constructor with minimal argument set. Name and dimension are always required.
	FRHITextureCreateDesc(const TCHAR* InDebugName, ETextureDimension InDimension)
		: FRHITextureDesc(InDimension)
		, DebugName(InDebugName)
	{
	}

	// Constructor for when you already have an FRHITextureDesc
	FRHITextureCreateDesc(
		  FRHITextureDesc const&      InDesc
		, ERHIAccess                  InInitialState
		, TCHAR const*                InDebugName
		//, FResourceBulkDataInterface* InBulkData     = nullptr
		)
		: FRHITextureDesc(InDesc)
		, InitialState   (InInitialState)
		, DebugName      (InDebugName)
		//, BulkData       (InBulkData)
	{}

	

	FRHITextureCreateDesc& SetFlags(ETextureCreateFlags InFlags)               { Flags = InFlags;                          return *this; }
	FRHITextureCreateDesc& AddFlags(ETextureCreateFlags InFlags)               { Flags |= InFlags;                         return *this; }
	FRHITextureCreateDesc& SetClearValue(FClearValueBinding InClearValue)      { ClearValue = InClearValue;                return *this; }
	FRHITextureCreateDesc& SetExtData(uint32 InExtData)                        { ExtData = InExtData;                      return *this; }
	FRHITextureCreateDesc& SetExtent(const Vector2& InExtent)                { Extent = InExtent;                        return *this; }
	FRHITextureCreateDesc& SetExtent(int32 InExtentX, int32 InExtentY)         { Extent = Vector2(InExtentX, InExtentY); return *this; }
	FRHITextureCreateDesc& SetExtent(uint32 InExtent)                          { Extent = Vector2(InExtent);             return *this; }
	FRHITextureCreateDesc& SetDepth(uint16 InDepth)                            { Depth = InDepth;                          return *this; }
	FRHITextureCreateDesc& SetArraySize(uint16 InArraySize)                    { ArraySize = InArraySize;                  return *this; }
	FRHITextureCreateDesc& SetNumMips(uint8 InNumMips)                         { NumMips = InNumMips;                      return *this; }
	FRHITextureCreateDesc& SetNumSamples(uint8 InNumSamples)                   { NumSamples = InNumSamples;                return *this; }
	FRHITextureCreateDesc& SetDimension(ETextureDimension InDimension)         { Dimension = InDimension;                  return *this; }
	FRHITextureCreateDesc& SetFormat(EPixelFormat InFormat)                    { Format = InFormat;                        return *this; }
	FRHITextureCreateDesc& SetUAVFormat(EPixelFormat InUAVFormat)              { UAVFormat = InUAVFormat;                  return *this; }
	FRHITextureCreateDesc& SetInitialState(ERHIAccess InInitialState)          { InitialState = InInitialState;            return *this; }
	FRHITextureCreateDesc& SetDebugName(const TCHAR* InDebugName)              { DebugName = InDebugName;                  return *this; }
	FRHITextureCreateDesc& SetClassName(const string& InClassName)			   { ClassName = InClassName;				   return *this; }
	FRHITextureCreateDesc& SetOwnerName(const string& InOwnerName)			   { OwnerName = InOwnerName;                  return *this; }


	/* The RHI access state that the resource will be created in. */
	ERHIAccess InitialState = ERHIAccess::Unknown;

	/* A friendly name for the resource. */
	const TCHAR* DebugName = nullptr;

	/* Optional initial data to fill the resource with. */
	//FResourceBulkDataInterface* BulkData = nullptr;

	string ClassName = "";	// The owner class of FRHITexture used for Insight asset metadata tracing
	string OwnerName = "";	// The owner name used for Insight asset metadata tracing
};



class FRHITexture : public FRHIViewableResource
#if ENABLE_RHI_VALIDATION
	, public RHIValidation::FTextureResource
#endif
{
protected:
	/** Initialization constructor. Should only be called by platform RHI implementations. */
	FRHITexture(const FRHITextureCreateDesc& InDesc);

public:
	/**
	 * Get the texture description used to create the texture
	 * Still virtual because FRHITextureReference can override this function - remove virtual when FRHITextureReference is deprecated
	 *
	 * @return TextureDesc used to create the texture
	 */
	virtual const FRHITextureDesc& GetDesc() const { return TextureDesc; }
	
	///
	/// Virtual functions implemented per RHI
	/// 
	
	//virtual class FRHITextureReference* GetTextureReference() { return NULL; }
	virtual FRHIDescriptorHandle GetDefaultBindlessHandle() const { return FRHIDescriptorHandle(); }

	/**
	 * Returns access to the platform-specific native resource pointer.  This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all.
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason
	 */
	virtual void* GetNativeResource() const
	{
		// Override this in derived classes to expose access to the native texture resource
		return nullptr;
	}

	/**
	 * Returns access to the platform-specific native shader resource view pointer.  This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all.
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason
	 */
	virtual void* GetNativeShaderResourceView() const
	{
		// Override this in derived classes to expose access to the native texture resource
		return nullptr;
	}

	/**
	 * Returns access to the platform-specific RHI texture baseclass.  This is designed to provide the RHI with fast access to its base classes in the face of multiple inheritance.
	 * @return	The pointer to the platform-specific RHI texture baseclass or NULL if it not initialized or not supported for this RHI
	 */
	virtual void* GetTextureBaseRHI()
	{
		// Override this in derived classes to expose access to the native texture resource
		return nullptr;
	}

	virtual void GetWriteMaskProperties(void*& OutData, uint32& OutSize)
	{
		OutData = nullptr;
		OutSize = 0;
	}

	///
	/// Helper getter functions - non virtual
	/// 

	/**
	 * Returns the x, y & z dimensions if the texture
	 * The Z component will always be 1 for 2D/cube resources and will contain depth for volume textures & array size for array textures
	 */
	Vector3 GetSizeXYZ() const
	{
		const FRHITextureDesc& Desc = GetDesc();
		switch (Desc.Dimension)
		{
		case ETextureDimension::Texture2D:		  return Vector3(Desc.Extent.x, Desc.Extent.y, 1);
		case ETextureDimension::Texture2DArray:	  return Vector3(Desc.Extent.x, Desc.Extent.y, Desc.ArraySize);
		case ETextureDimension::Texture3D:		  return Vector3(Desc.Extent.x, Desc.Extent.y, Desc.Depth);
		case ETextureDimension::TextureCube:	  return Vector3(Desc.Extent.x, Desc.Extent.y, 1);
		case ETextureDimension::TextureCubeArray: return Vector3(Desc.Extent.x, Desc.Extent.y, Desc.ArraySize);
		}
		return Vector3(0, 0, 0);
	}

	/**
	 * Returns the dimensions (i.e. the actual number of texels in each dimension) of the specified mip. ArraySize is ignored.
	 * The Z component will always be 1 for 2D/cube resources and will contain depth for volume textures.
	 * This differs from GetSizeXYZ() which returns ArraySize in Z for 2D arrays.
	 */
	Vector3 GetMipDimensions(uint8 MipIndex) const
	{
		const FRHITextureDesc& Desc = GetDesc();
		return Vector3(
		static_cast<float>(std::max<int32>((int32)Desc.Extent.x >> MipIndex, 1)),
		static_cast<float>(std::max<int32>((int32)Desc.Extent.y >> MipIndex, 1)),
		static_cast<float>(std::max<int32>(Desc.Depth    >> MipIndex, 1))
		);
	}

	/** @return Whether the texture is multi sampled. */
	bool IsMultisampled() const { return GetDesc().NumSamples > 1; }

	/** @return Whether the texture has a clear color defined */
	bool HasClearValue() const
	{
		return GetDesc().ClearValue.ColorBinding != EClearBinding::ENoneBound;
	}

	/** @return the clear color value if set */
	Color GetClearColor() const
	{
		return GetDesc().ClearValue.GetClearColor();
	}

	/** @return the depth & stencil clear value if set */
	void GetDepthStencilClearValue(float& OutDepth, uint32& OutStencil) const
	{
		return GetDesc().ClearValue.GetDepthStencil(OutDepth, OutStencil);
	}

	/** @return the depth clear value if set */
	float GetDepthClearValue() const
	{
		float Depth;
		uint32 Stencil;
		GetDesc().ClearValue.GetDepthStencil(Depth, Stencil);
		return Depth;
	}

	/** @return the stencil clear value if set */
	uint32 GetStencilClearValue() const
	{
		float Depth;
		uint32 Stencil;
		GetDesc().ClearValue.GetDepthStencil(Depth, Stencil);
		return Stencil;
	}

	///
	/// RenderTime & Name functions - non virtual
	/// 

	/** sets the last time this texture was cached in a resource table. */
	void SetLastRenderTime(float InLastRenderTime)
	{
		LastRenderTime.SetLastRenderTime(InLastRenderTime);
	}

	double GetLastRenderTime() const
	{
		return LastRenderTime.GetLastRenderTime();
	}

	void SetName(const string& InName);

	///
	/// Deprecated functions
	/// 

	using FRHITexture2D                    = FRHITexture;
	using FRHITexture2DArray               = FRHITexture;
	using FRHITexture3D                    = FRHITexture;
	using FRHITextureCube                  = FRHITexture;
	
	//UE_DEPRECATED(5.1, "FRHITexture2D is deprecated, please use FRHITexture directly")
	inline FRHITexture2D* GetTexture2D() { return TextureDesc.Dimension == ETextureDimension::Texture2D ? this : nullptr; }
	//UE_DEPRECATED(5.1, "FRHITexture2DArray is deprecated, please use FRHITexture directly")
	inline FRHITexture2DArray* GetTexture2DArray() { return TextureDesc.Dimension == ETextureDimension::Texture2DArray ? this : nullptr; }
	//UE_DEPRECATED(5.1, "FRHITexture3D is deprecated, please use FRHITexture directly")
	inline FRHITexture3D* GetTexture3D() { return TextureDesc.Dimension == ETextureDimension::Texture3D ? this : nullptr; }
	//UE_DEPRECATED(5.1, "FRHITextureCube is deprecated, please use FRHITexture directly")
	inline FRHITextureCube* GetTextureCube() { return TextureDesc.IsTextureCube() ? this : nullptr; }

	//UE_DEPRECATED(5.1, "GetSizeX() is deprecated, please use GetDesc().Extent.X instead")
	uint32 GetSizeX() const { return GetDesc().Extent.x; }

	//UE_DEPRECATED(5.1, "GetSizeY() is deprecated, please use GetDesc().Extent.Y instead")
	uint32 GetSizeY() const { return GetDesc().Extent.y; }

	//UE_DEPRECATED(5.1, "GetSizeXY() is deprecated, please use GetDesc().Extent.X or GetDesc().Extent.Y instead")
	Vector2 GetSizeXY() const { return Vector2(GetDesc().Extent.x, GetDesc().Extent.y); }

	//UE_DEPRECATED(5.1, "GetSizeZ() is deprecated, please use GetDesc().ArraySize instead for TextureArrays and GetDesc().Depth for 3D textures")
	uint32 GetSizeZ() const { return GetSizeXYZ().z; }

	//UE_DEPRECATED(5.1, "GetNumMips() is deprecated, please use GetDesc().NumMips instead")
	uint32 GetNumMips() const { return GetDesc().NumMips; }

	//UE_DEPRECATED(5.1, "GetFormat() is deprecated, please use GetDesc().Format instead")
	EPixelFormat GetFormat() const { return GetDesc().Format; }

	//UE_DEPRECATED(5.1, "GetFlags() is deprecated, please use GetDesc().Flags instead")
	ETextureCreateFlags GetFlags() const { return GetDesc().Flags; }

	//UE_DEPRECATED(5.1, "GetNumSamples() is deprecated, please use GetDesc().NumSamples instead")
	uint32 GetNumSamples() const { return GetDesc().NumSamples; }

	//UE_DEPRECATED(5.1, "GetClearBinding() is deprecated, please use GetDesc().ClearValue instead")
	const FClearValueBinding GetClearBinding() const { return GetDesc().ClearValue; }

	//UE_DEPRECATED(5.1, "GetSize() is deprecated, please use GetDesc().Extent.X instead")
	uint32 GetSize() const { Check(GetDesc().IsTextureCube()); return GetDesc().Extent.x; }


private:

	//friend class FRHITextureReference;
	/** Constructor for texture references */
	FRHITexture(ERHIResourceType InResourceType)
		: FRHIViewableResource(InResourceType, ERHIAccess::Unknown)
	{
		Check(InResourceType == RRT_TextureReference);
	}

	FRHITextureDesc TextureDesc;

	FLastRenderTimeContainer LastRenderTime;
};

enum class ERHITexturePlane : uint8
{
	// The primary plane is used with default compression behavior.
	Primary = 0,

	// The primary plane is used without decompressing it.
	PrimaryCompressed = 1,

	// The depth plane is used with default compression behavior.
	Depth = 2,

	// The stencil plane is used with default compression behavior.
	Stencil = 3,

	// The HTile plane is used.
	HTile = 4,

	// the FMask plane is used.
	FMask = 5,

	// the CMask plane is used.
	CMask = 6,

	// This enum is packed into various structures. Avoid adding new 
	// members without verifying structure sizes aren't increased.
	Num,
	NumBits = 3,

	// @todo deprecate
	None = Primary,
	CompressedSurface = PrimaryCompressed,
};

template <typename TType>
struct TRHIRange
{
	TType First = 0;
	TType Num = 0;

	TRHIRange() = default;
	TRHIRange(uint32 InFirst, uint32 InNum)
		: First(InFirst)
		, Num  (InNum)
	{
		
	}

	TType ExclusiveLast() const { return First + Num; }
	TType InclusiveLast() const { return First + Num - 1; }

	bool IsInRange(uint32 Value) const
	{
		return Value >= First && Value < ExclusiveLast();
	}
};

using FRHIRange8  = TRHIRange<uint8>;
using FRHIRange16 = TRHIRange<uint16>;


class FRHIViewport : public FRHIResource 
{
public:
	FRHIViewport() : FRHIResource(RRT_Viewport) {}

	/**
	 * Returns access to the platform-specific native resource pointer.  This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all.
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason
	 */
	virtual void* GetNativeSwapChain() const { return nullptr; }
	/**
	 * Returns access to the platform-specific native resource pointer to a backbuffer texture.  This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all.
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason
	 */
	virtual void* GetNativeBackBufferTexture() const { return nullptr; }
	/**
	 * Returns access to the platform-specific native resource pointer to a backbuffer rendertarget. This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all.
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason
	 */
	virtual void* GetNativeBackBufferRT() const { return nullptr; }

	/**
	 * Returns access to the platform-specific native window. This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all. 
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason.
	 * AddParam could represent any additional platform-specific data (could be null).
	 */
	virtual void* GetNativeWindow(void** AddParam = nullptr) const { return nullptr; }
	
	/**
	 * Ticks the viewport on the Game thread
	 */
	virtual void Tick(float DeltaTime) {}

	virtual void WaitForFrameEventCompletion() { }

	virtual void IssueFrameEvent() { }
};

struct FRHIViewDesc
{
	enum class EViewType : uint8
	{
		BufferSRV,
		BufferUAV,
		TextureSRV,
		TextureUAV
	};

	enum class EBufferType : uint8
	{
		Unknown               = 0,

		Typed                 = 1,
		Structured            = 2,
		AccelerationStructure = 3,
		Raw                   = 4
	};

	enum class EDimension : uint8
	{
		Unknown          = 0,

		Texture2D        = 1,
		Texture2DArray   = 2,
		TextureCube      = 3,
		TextureCubeArray = 4,
		Texture3D        = 5,

		NumBits          = 3
	};

	// Properties that apply to all views.
	struct FCommon
	{
		EViewType    ViewType;
		EPixelFormat Format;
	};

	// Properties shared by buffer views. Some fields are SRV or UAV specific.
	struct FBuffer : public FCommon
	{
		EBufferType BufferType;
		uint8       bAtomicCounter : 1; // UAV only
		uint8       bAppendBuffer  : 1; // UAV only
		uint8       /* padding */  : 6;
		uint32      OffsetInBytes;
		uint32      NumElements;
		uint32      Stride;

		struct FViewInfo;
	protected:
		FViewInfo GetViewInfo(FRHIBuffer* TargetBuffer) const;
	};

	// Properties shared by texture views. Some fields are SRV or UAV specific.
	struct FTexture : public FCommon
	{
		ERHITexturePlane Plane        : uint32(ERHITexturePlane::NumBits);
		uint8            bDisableSRGB : 1; // SRV only
		EDimension       Dimension    : uint32(EDimension::NumBits);
		FRHIRange8       MipRange;    // UAVs only support 1 mip
		FRHIRange16      ArrayRange;

		struct FViewInfo;
	protected:
		FViewInfo GetViewInfo(FRHITexture* TargetTexture) const;
	};

	struct FBufferSRV : public FBuffer
	{
		struct FInitializer;
		struct FViewInfo;
		FViewInfo GetViewInfo(FRHIBuffer* TargetBuffer) const;
	};

	struct FBufferUAV : public FBuffer
	{
		struct FInitializer;
		struct FViewInfo;
		FViewInfo GetViewInfo(FRHIBuffer* TargetBuffer) const;
	};

	struct FTextureSRV : public FTexture
	{
		struct FInitializer;
		struct FViewInfo;
		FViewInfo GetViewInfo(FRHITexture* TargetTexture) const;
	};

	struct FTextureUAV : public FTexture
	{
		struct FInitializer;
		struct FViewInfo;
		FViewInfo GetViewInfo(FRHITexture* TargetTexture) const;
	};

	union
	{
		FCommon Common;
		union
		{
			FBufferSRV SRV;
			FBufferUAV UAV;
		} Buffer;
		union
		{
			FTextureSRV SRV;
			FTextureUAV UAV;
		} Texture;
	};

	static inline FBufferSRV::FInitializer CreateBufferSRV();
	static inline FBufferUAV::FInitializer CreateBufferUAV();

	static inline FTextureSRV::FInitializer CreateTextureSRV();
	static inline FTextureUAV::FInitializer CreateTextureUAV();

	bool IsSRV() const { return Common.ViewType == EViewType::BufferSRV || Common.ViewType == EViewType::TextureSRV; }
	bool IsUAV() const { return !IsSRV(); }

	bool IsBuffer () const { return Common.ViewType == EViewType::BufferSRV || Common.ViewType == EViewType::BufferUAV; }
	bool IsTexture() const { return !IsBuffer(); }

	bool operator == (FRHIViewDesc const& RHS) const
	{
		if (Common.ViewType != RHS.Common.ViewType)
			return false;

		if (IsBuffer())
		{
			const FBuffer& A = IsSRV() ? static_cast<const FBuffer&>(Buffer.SRV) : static_cast<const FBuffer&>(Buffer.UAV);
			const FBuffer& B = RHS.IsSRV() ? static_cast<const FBuffer&>(RHS.Buffer.SRV) : static_cast<const FBuffer&>(RHS.Buffer.UAV);

			return A.ViewType     == B.ViewType &&
				   A.Format       == B.Format &&
				   A.BufferType   == B.BufferType &&
				   A.bAtomicCounter == B.bAtomicCounter &&
				   A.bAppendBuffer  == B.bAppendBuffer &&
				   A.OffsetInBytes == B.OffsetInBytes &&
				   A.NumElements   == B.NumElements &&
				   A.Stride        == B.Stride;
		}
		else
		{
			const FTexture& A = IsSRV() ? static_cast<const FTexture&>(Texture.SRV) : static_cast<const FTexture&>(Texture.UAV);
			const FTexture& B = RHS.IsSRV() ? static_cast<const FTexture&>(RHS.Texture.SRV) : static_cast<const FTexture&>(RHS.Texture.UAV);

			return A.ViewType     == B.ViewType &&
				   A.Format       == B.Format &&
				   A.Plane        == B.Plane &&
				   A.bDisableSRGB == B.bDisableSRGB &&
				   A.Dimension    == B.Dimension &&
				   A.MipRange.First == B.MipRange.First &&
				   A.MipRange.Num   == B.MipRange.Num &&
				   A.ArrayRange.First == B.ArrayRange.First &&
				   A.ArrayRange.Num   == B.ArrayRange.Num;
		}
	}

	bool operator != (FRHIViewDesc const& RHS) const
	{
		return !(*this == RHS);
	}

	FRHIViewDesc()
		: FRHIViewDesc(EViewType::BufferSRV)
	{
	}

protected:
	FRHIViewDesc(EViewType ViewType)
	{
		Common.ViewType = ViewType;
	}
};

struct FRHIViewDesc::FBufferSRV::FInitializer : private FRHIViewDesc
{
	friend FRHIViewDesc;
	//friend FRHICommandListBase;
	friend struct FShaderResourceViewInitializer;
	friend struct FRawBufferShaderResourceViewInitializer;

protected:
	FInitializer()
		: FRHIViewDesc(EViewType::BufferSRV)
	{}

public:
	FInitializer& SetType(EBufferType Type)
	{
		Check(Type != EBufferType::Unknown);
		Buffer.SRV.BufferType = Type;
		return *this;
	}

	//
	// Provided for back-compat with existing code. Consider using SetType() instead for more direct control over the view.
	// For example, it is possible to create a typed view of a BUF_ByteAddress buffer, but not using this function which always choses raw access.
	//
	FInitializer& SetTypeFromBuffer(FRHIBuffer* TargetBuffer)
	{
		Check(TargetBuffer);
		//Checkf(!TargetBuffer->GetDesc().IsNull(), TEXT("Null buffer resources are placeholders for the streaming system. They do not contain a valid descriptor for this function to use. Call SetType() instead."));

		Buffer.SRV.BufferType =
			EnumHasAnyFlags(TargetBuffer->GetUsage(), BUF_ByteAddressBuffer    ) ? EBufferType::Raw                   :
			EnumHasAnyFlags(TargetBuffer->GetUsage(), BUF_StructuredBuffer     ) ? EBufferType::Structured            :
			EnumHasAnyFlags(TargetBuffer->GetUsage(), BUF_AccelerationStructure) ? EBufferType::AccelerationStructure :
			EBufferType::Typed;
		return *this;
	}

	FInitializer& SetFormat(EPixelFormat InFormat)
	{
		Buffer.SRV.Format = InFormat;
		return *this;
	}

	FInitializer& SetOffsetInBytes(uint32 InOffsetBytes)
	{
		Buffer.SRV.OffsetInBytes = InOffsetBytes;
		return *this;
	}

	FInitializer& SetStride(uint32 InStride)
	{
		Buffer.SRV.Stride = InStride;
		return *this;
	}

	FInitializer& SetNumElements(uint32 InNumElements)
	{
		Buffer.SRV.NumElements = InNumElements;
		return *this;
	}
};

struct FRHIViewDesc::FBufferUAV::FInitializer : private FRHIViewDesc
{
	friend FRHIViewDesc;
	//friend FRHICommandListBase;

protected:
	FInitializer()
		: FRHIViewDesc(EViewType::BufferUAV)
	{}

public:
	FInitializer& SetType(EBufferType Type)
	{
		Check(Type != EBufferType::Unknown);
		Buffer.UAV.BufferType = Type;
		return *this;
	}

	//
	// Provided for back-compat with existing code. Consider using SetType() instead for more direct control over the view.
	// For example, it is possible to create a typed view of a BUF_ByteAddress buffer, but not using this function which always choses raw access.
	//
	FInitializer& SetTypeFromBuffer(FRHIBuffer* TargetBuffer)
	{
		Check(TargetBuffer);
		//Checkf(!TargetBuffer->GetDesc().IsNull(), TEXT("Null buffer resources are placeholders for the streaming system. They do not contain a valid descriptor for this function to use. Call SetType() instead."));

		Buffer.UAV.BufferType =
			EnumHasAnyFlags(TargetBuffer->GetUsage(), BUF_ByteAddressBuffer    ) ? EBufferType::Raw                   :
			EnumHasAnyFlags(TargetBuffer->GetUsage(), BUF_StructuredBuffer     ) ? EBufferType::Structured            :
			EnumHasAnyFlags(TargetBuffer->GetUsage(), BUF_AccelerationStructure) ? EBufferType::AccelerationStructure :
			EBufferType::Typed;
		return *this;
	}

	FInitializer& SetFormat(EPixelFormat InFormat)
	{
		Buffer.UAV.Format = InFormat;
		return *this;
	}

	FInitializer& SetOffsetInBytes(uint32 InOffsetBytes)
	{
		Buffer.UAV.OffsetInBytes = InOffsetBytes;
		return *this;
	}

	FInitializer& SetStride(uint32 InStride)
	{
		Buffer.UAV.Stride = InStride;
		return *this;
	}

	FInitializer& SetNumElements(uint32 InNumElements)
	{
		Buffer.UAV.NumElements = InNumElements;
		return *this;
	}

	FInitializer& SetAtomicCounter(bool InAtomicCounter)
	{
		Buffer.UAV.bAtomicCounter = InAtomicCounter;
		return *this;
	}

	FInitializer& SetAppendBuffer(bool InAppendBuffer)
	{
		Buffer.UAV.bAppendBuffer = InAppendBuffer;
		return *this;
	}
};

struct FRHIViewDesc::FTextureSRV::FInitializer : private FRHIViewDesc
{
	friend FRHIViewDesc;
	//friend FRHICommandListBase;

protected:
	FInitializer()
		: FRHIViewDesc(EViewType::TextureSRV)
	{}

public:
	//
	// Specifies the type of view to create. Must match the shader parameter this view will be bound to.
	// 
	// The dimension is allowed to differ from the underlying resource's dimensions, e.g. to create a view
	// compatible with a Texture2D<> shader parameter, but where the underlying resource is a texture 2D array.
	//
	// Some combinations are not valid, e.g. 3D textures can only have 3D views.
	//
	FInitializer& SetDimension(ETextureDimension InDimension)
	{
		switch (InDimension)
		{
		//default: CheckNoEntry(); break;
		case ETextureDimension::Texture2D       : Texture.SRV.Dimension = EDimension::Texture2D       ; break;
		case ETextureDimension::Texture2DArray  : Texture.SRV.Dimension = EDimension::Texture2DArray  ; break;
		case ETextureDimension::Texture3D       : Texture.SRV.Dimension = EDimension::Texture3D       ; break;
		case ETextureDimension::TextureCube     : Texture.SRV.Dimension = EDimension::TextureCube     ; break;
		case ETextureDimension::TextureCubeArray: Texture.SRV.Dimension = EDimension::TextureCubeArray; break;
		}
		return *this;
	}

	//
	// Provided for back-compat with existing code. Consider using SetDimension() instead for more direct control over the view.
	// For example, it is possible to create a 2D view of a 2DArray texture, but not using this function which always choses 2DArray dimension.
	//
	FInitializer& SetDimensionFromTexture(FRHITexture* TargetTexture)
	{
		Check(TargetTexture);
		SetDimension(TargetTexture->GetDesc().Dimension);
		return *this;
	}

	FInitializer& SetFormat(EPixelFormat InFormat)
	{
		Texture.SRV.Format = InFormat;
		return *this;
	}

	FInitializer& SetPlane(ERHITexturePlane InPlane)
	{
		Texture.SRV.Plane = InPlane;
		return *this;
	}

	FInitializer& SetMipRange(uint8 InFirstMip, uint8 InNumMips)
	{
		Texture.SRV.MipRange.First = InFirstMip;
		Texture.SRV.MipRange.Num = InNumMips;
		return *this;
	}

	//
	// The meaning of array "elements" is given by the dimension of the underlying resource.
	// I.e. a view of a TextureCubeArray resource indexes the array in whole cubes.
	// 
	//     [0] = the first cube (2D slices 0 to 5)
	//     [1] = the second cube (2D slices 6 to 11)
	// 
	// If the view dimension is smaller than the resource dimension, the array range will be further limited.
	// E.g. creating a Texture2D dimension view of a TextureCubeArray resource
	//
	FInitializer& SetArrayRange(uint16 InFirstElement, uint16 InNumElements)
	{
		Texture.SRV.ArrayRange.First = InFirstElement;
		Texture.SRV.ArrayRange.Num = InNumElements;
		return *this;
	}

	FInitializer& SetDisableSRGB(bool InDisableSRGB)
	{
		Texture.SRV.bDisableSRGB = InDisableSRGB;
		return *this;
	}
};

struct FRHIViewDesc::FTextureUAV::FInitializer : private FRHIViewDesc
{
	friend FRHIViewDesc;
	//friend FRHICommandListBase;

protected:
	FInitializer()
		: FRHIViewDesc(EViewType::TextureUAV)
	{
		// Texture UAVs only support 1 mip
		Texture.UAV.MipRange.Num = 1;
	}

public:
	//
	// Specifies the type of view to create. Must match the shader parameter this view will be bound to.
	// 
	// The dimension is allowed to differ from the underlying resource's dimensions, e.g. to create a view
	// compatible with an RWTexture2D<> shader parameter, but where the underlying resource is a texture 2D array.
	//
	// Some combinations are not valid, e.g. 3D textures can only have 3D views.
	//
	FInitializer& SetDimension(ETextureDimension InDimension)
	{
		switch (InDimension)
		{
		case ETextureDimension::Texture2D       : Texture.UAV.Dimension = EDimension::Texture2D       ; break;
		case ETextureDimension::Texture2DArray  : Texture.UAV.Dimension = EDimension::Texture2DArray  ; break;
		case ETextureDimension::Texture3D       : Texture.UAV.Dimension = EDimension::Texture3D       ; break;
		case ETextureDimension::TextureCube     : Texture.UAV.Dimension = EDimension::TextureCube     ; break;
		case ETextureDimension::TextureCubeArray: Texture.UAV.Dimension = EDimension::TextureCubeArray; break;
		}
		return *this;
	}

	//
	// Provided for back-compat with existing code. Consider using SetDimension() instead for more direct control over the view.
	// For example, it is possible to create a 2D view of a 2DArray texture, but not using this function which always choses 2DArray dimension.
	//
	FInitializer& SetDimensionFromTexture(FRHITexture* TargetTexture)
	{
		Check(TargetTexture);
		SetDimension(TargetTexture->GetDesc().Dimension);
		return *this;
	}

	FInitializer& SetFormat(EPixelFormat InFormat)
	{
		Texture.UAV.Format = InFormat;
		return *this;
	}

	FInitializer& SetPlane(ERHITexturePlane InPlane)
	{
		Texture.UAV.Plane = InPlane;
		return *this;
	}

	FInitializer& SetMipLevel(uint8 InMipLevel)
	{
		Texture.UAV.MipRange.First = InMipLevel;
		return *this;
	}

	//
	// The meaning of array "elements" is given by the dimension of the underlying resource.
	// I.e. a view of a TextureCubeArray resource indexes the array in whole cubes.
	// 
	//     [0] = the first cube (2D slices 0 to 5)
	//     [1] = the second cube (2D slices 6 to 11)
	// 
	// If the view dimension is smaller than the resource dimension, the array range will be further limited.
	// E.g. creating a Texture2D dimension view of a TextureCubeArray resource
	//
	FInitializer& SetArrayRange(uint16 InFirstElement, uint16 InNumElements)
	{
		Texture.UAV.ArrayRange.First = InFirstElement;
		Texture.UAV.ArrayRange.Num = InNumElements;
		return *this;
	}
};

inline FRHIViewDesc::FBufferSRV::FInitializer FRHIViewDesc::CreateBufferSRV()
{
	return FRHIViewDesc::FBufferSRV::FInitializer();
}

inline FRHIViewDesc::FBufferUAV::FInitializer FRHIViewDesc::CreateBufferUAV()
{
	return FRHIViewDesc::FBufferUAV::FInitializer();
}

inline FRHIViewDesc::FTextureSRV::FInitializer FRHIViewDesc::CreateTextureSRV()
{
	return FRHIViewDesc::FTextureSRV::FInitializer();
}

inline FRHIViewDesc::FTextureUAV::FInitializer FRHIViewDesc::CreateTextureUAV()
{
	return FRHIViewDesc::FTextureUAV::FInitializer();
}

//
// Used by platform RHIs to create views of buffers. The data in this structure is computed in GetViewInfo(),
// and is specific to a particular buffer resource. It is not intended to be stored in a view instance.
//
struct FRHIViewDesc::FBuffer::FViewInfo
{
	// The offset in bytes from the beginning of the viewed buffer resource.
	uint32 OffsetInBytes;

	// The size in bytes of a single element in the view.
	uint32 StrideInBytes;

	// The number of elements visible in the view.
	uint32 NumElements;

	// The total number of bytes the data visible in the view covers (i.e. stride * numelements).
	uint32 SizeInBytes;

	// Whether this is a typed / structured / raw view etc.
	EBufferType BufferType;

	// The format of the data exposed by this view. PF_Unknown for all buffer types except typed buffer views.
	EPixelFormat Format;

	// When true, the view is refering to a BUF_NullResource, so a null descriptor should be created.
	bool bNullView;
};

// Buffer SRV specific info
struct FRHIViewDesc::FBufferSRV::FViewInfo : public FRHIViewDesc::FBuffer::FViewInfo
{};

// Buffer UAV specific info
struct FRHIViewDesc::FBufferUAV::FViewInfo : public FRHIViewDesc::FBuffer::FViewInfo
{
	bool bAtomicCounter = false;
	bool bAppendBuffer = false;
};

//
// Used by platform RHIs to create views of textures. The data in this structure is computed in GetViewInfo(),
// and is specific to a particular texture resource. It is not intended to be stored in a view instance.
//
struct FRHIViewDesc::FTexture::FViewInfo
{
	//
	// The range of array "elements" the view covers.
	// 
	// The meaning of "elements" is given by the view dimension.
	// I.e. a view with "Dimension == CubeArray" indexes the array in whole cubes.
	// 
	//		- [0]: the first cube (2D slices 0 to 5)
	//		- [1]: the second cube (2D slices 6 to 11)
	// 
	// 3D textures always have ArrayRange.Num == 1 because there are no "3D texture arrays".
	//
	FRHIRange16 ArrayRange;

	// Which plane of a texture to access (i.e. color, depth, stencil etc). 
	ERHITexturePlane Plane;

	// The typed format to use when reading / writing data in the viewed texture.
	EPixelFormat Format;

	// Specifies how to treat the texture resource when creating the view.
	// E.g. it is possible to create a 2DArray view of a 2D or Cube texture.
	EDimension Dimension : uint32(EDimension::NumBits);

	// True when the view covers every mip of the resource.
	uint8 bAllMips : 1;

	// True when the view covers every array slice of the resource.
	// This includes depth slices for 3D textures, and faces of texture cubes.
	uint8 bAllSlices : 1;
};

struct FRHIViewDesc::FTextureSRV::FViewInfo : public FRHIViewDesc::FTexture::FViewInfo
{
	// The range of texture mips the view covers.
	FRHIRange8 MipRange;

	// Indicates if this view should use an sRGB variant of the typed format.
	uint8 bSRGB : 1;
};


// Texture UAV specific info
struct FRHIViewDesc::FTextureUAV::FViewInfo : public FRHIViewDesc::FTexture::FViewInfo
{
	// The single mip level covered by this view.
	uint8 MipLevel;
};

class FRHIView : public FRHIResource
{
public:
	FRHIView(ERHIResourceType InResourceType, FRHIViewableResource* InResource, FRHIViewDesc const& InViewDesc)
		: FRHIResource(InResourceType)
		, Resource(InResource)
		, ViewDesc(InViewDesc)
	{
		//Assert(InResource, TEXT("Cannot create a view of a nullptr resource."));
	}

	virtual FRHIDescriptorHandle GetBindlessHandle() const
	{
		return FRHIDescriptorHandle();
	}

	FRHIViewableResource* GetResource() const
	{
		return Resource.get();
	}

	FRHIBuffer* GetBuffer() const
	{
		Check(ViewDesc.IsBuffer());
		return static_cast<FRHIBuffer*>(Resource.get());
	}

	FRHITexture* GetTexture() const
	{
		Check(ViewDesc.IsTexture());
		return static_cast<FRHITexture*>(Resource.get());
	}

	bool IsBuffer () const { return ViewDesc.IsBuffer (); }
	bool IsTexture() const { return ViewDesc.IsTexture(); }

#if ENABLE_RHI_VALIDATION
	RHIValidation::FViewIdentity GetViewIdentity()
	{
		return RHIValidation::FViewIdentity(Resource, ViewDesc);
	}
#endif

	FRHIViewDesc const& GetDesc() const
	{
		return ViewDesc;
	}

private:
	shared_ptr<FRHIViewableResource> Resource;

protected:
	FRHIViewDesc const ViewDesc;
};

class FRHIUnorderedAccessView : public FRHIView
{
public:
	explicit FRHIUnorderedAccessView(FRHIViewableResource* InResource, FRHIViewDesc const& InViewDesc)
		: FRHIView(RRT_UnorderedAccessView, InResource, InViewDesc)
	{
		Check(ViewDesc.IsUAV());
	}
};

class FRHIShaderResourceView : public FRHIView
{
public:
	explicit FRHIShaderResourceView(FRHIViewableResource* InResource, FRHIViewDesc const& InViewDesc)
		: FRHIView(RRT_ShaderResourceView, InResource, InViewDesc)
	{
		Check(ViewDesc.IsSRV());
	}
};

class FRHIStagingBuffer : public FRHIResource
{
public:
	FRHIStagingBuffer()
		: FRHIResource(RRT_StagingBuffer)
		, bIsLocked(false)
	{}

	virtual ~FRHIStagingBuffer() {}

	virtual void *Lock(uint32 Offset, uint32 NumBytes) = 0;
	virtual void Unlock() = 0;

	// For debugging, may not be implemented on all RHIs
	virtual uint64 GetGPUSizeBytes() const { return 0; }

protected:
	bool bIsLocked;
};



class FGenericRHIStagingBuffer : public FRHIStagingBuffer
{
public:
	FGenericRHIStagingBuffer()
		: FRHIStagingBuffer()
	{}

	~FGenericRHIStagingBuffer() {}

	virtual void* Lock(uint32 Offset, uint32 NumBytes) final override;
	virtual void Unlock() final override;
	virtual uint64 GetGPUSizeBytes() const final override { return ShadowBuffer!=nullptr ? ShadowBuffer->GetSize() : 0; }

	shared_ptr<FRHIBuffer> ShadowBuffer;
	uint32 Offset;
};

class FRHIRenderTargetView
{
public:
	FRHITexture* Texture = nullptr;
	uint32 MipIndex = 0;

	/** Array slice or texture cube face.  Only valid if texture resource was created with TexCreate_TargetArraySlicesIndependently! */
	uint32 ArraySliceIndex = -1;

	ERenderTargetLoadAction LoadAction = ERenderTargetLoadAction::ENoAction;
	ERenderTargetStoreAction StoreAction = ERenderTargetStoreAction::ENoAction;

	FRHIRenderTargetView() = default;
	FRHIRenderTargetView(FRHIRenderTargetView&&) = default;
	FRHIRenderTargetView(const FRHIRenderTargetView&) = default;
	FRHIRenderTargetView& operator=(FRHIRenderTargetView&&) = default;
	FRHIRenderTargetView& operator=(const FRHIRenderTargetView&) = default;

	//common case
	explicit FRHIRenderTargetView(FRHITexture* InTexture, ERenderTargetLoadAction InLoadAction) :
		Texture(InTexture),
		MipIndex(0),
		ArraySliceIndex(-1),
		LoadAction(InLoadAction),
		StoreAction(ERenderTargetStoreAction::EStore)
	{}

	//common case
	explicit FRHIRenderTargetView(FRHITexture* InTexture, ERenderTargetLoadAction InLoadAction, uint32 InMipIndex, uint32 InArraySliceIndex) :
		Texture(InTexture),
		MipIndex(InMipIndex),
		ArraySliceIndex(InArraySliceIndex),
		LoadAction(InLoadAction),
		StoreAction(ERenderTargetStoreAction::EStore)
	{}
	
	explicit FRHIRenderTargetView(FRHITexture* InTexture, uint32 InMipIndex, uint32 InArraySliceIndex, ERenderTargetLoadAction InLoadAction, ERenderTargetStoreAction InStoreAction) :
		Texture(InTexture),
		MipIndex(InMipIndex),
		ArraySliceIndex(InArraySliceIndex),
		LoadAction(InLoadAction),
		StoreAction(InStoreAction)
	{}

	bool operator==(const FRHIRenderTargetView& Other) const
	{
		return 
			Texture == Other.Texture &&
			MipIndex == Other.MipIndex &&
			ArraySliceIndex == Other.ArraySliceIndex &&
			LoadAction == Other.LoadAction &&
			StoreAction == Other.StoreAction;
	}
};

class FRHIDepthRenderTargetView
{
public:
	FRHITexture* Texture;

	ERenderTargetLoadAction		DepthLoadAction;
	ERenderTargetStoreAction	DepthStoreAction;
	ERenderTargetLoadAction		StencilLoadAction;

private:
	ERenderTargetStoreAction	StencilStoreAction;
	FExclusiveDepthStencil		DepthStencilAccess;
public:

	// accessor to prevent write access to StencilStoreAction
	ERenderTargetStoreAction GetStencilStoreAction() const { return StencilStoreAction; }
	// accessor to prevent write access to DepthStencilAccess
	FExclusiveDepthStencil GetDepthStencilAccess() const { return DepthStencilAccess; }

	explicit FRHIDepthRenderTargetView() :
		Texture(nullptr),
		DepthLoadAction(ERenderTargetLoadAction::ENoAction),
		DepthStoreAction(ERenderTargetStoreAction::ENoAction),
		StencilLoadAction(ERenderTargetLoadAction::ENoAction),
		StencilStoreAction(ERenderTargetStoreAction::ENoAction),
		DepthStencilAccess(FExclusiveDepthStencil::DepthNop_StencilNop)
	{
		Validate();
	}

	//common case
	explicit FRHIDepthRenderTargetView(FRHITexture* InTexture, ERenderTargetLoadAction InLoadAction, ERenderTargetStoreAction InStoreAction) :
		Texture(InTexture),
		DepthLoadAction(InLoadAction),
		DepthStoreAction(InStoreAction),
		StencilLoadAction(InLoadAction),
		StencilStoreAction(InStoreAction),
		DepthStencilAccess(FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		Validate();
	}

	explicit FRHIDepthRenderTargetView(FRHITexture* InTexture, ERenderTargetLoadAction InLoadAction, ERenderTargetStoreAction InStoreAction, FExclusiveDepthStencil InDepthStencilAccess) :
		Texture(InTexture),
		DepthLoadAction(InLoadAction),
		DepthStoreAction(InStoreAction),
		StencilLoadAction(InLoadAction),
		StencilStoreAction(InStoreAction),
		DepthStencilAccess(InDepthStencilAccess)
	{
		Validate();
	}

	explicit FRHIDepthRenderTargetView(FRHITexture* InTexture, ERenderTargetLoadAction InDepthLoadAction, ERenderTargetStoreAction InDepthStoreAction, ERenderTargetLoadAction InStencilLoadAction, ERenderTargetStoreAction InStencilStoreAction) :
		Texture(InTexture),
		DepthLoadAction(InDepthLoadAction),
		DepthStoreAction(InDepthStoreAction),
		StencilLoadAction(InStencilLoadAction),
		StencilStoreAction(InStencilStoreAction),
		DepthStencilAccess(FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		Validate();
	}

	explicit FRHIDepthRenderTargetView(FRHITexture* InTexture, ERenderTargetLoadAction InDepthLoadAction, ERenderTargetStoreAction InDepthStoreAction, ERenderTargetLoadAction InStencilLoadAction, ERenderTargetStoreAction InStencilStoreAction, FExclusiveDepthStencil InDepthStencilAccess) :
		Texture(InTexture),
		DepthLoadAction(InDepthLoadAction),
		DepthStoreAction(InDepthStoreAction),
		StencilLoadAction(InStencilLoadAction),
		StencilStoreAction(InStencilStoreAction),
		DepthStencilAccess(InDepthStencilAccess)
	{
		Validate();
	}

	void Validate() const
	{
		// VK and Metal MAY leave the attachment in an undefined state if the StoreAction is DontCare. So we can't assume read-only implies it should be DontCare unless we know for sure it will never be used again.
		// ensureMsgf(DepthStencilAccess.IsDepthWrite() || DepthStoreAction == ERenderTargetStoreAction::ENoAction, TEXT("Depth is read-only, but we are performing a store.  This is a waste on mobile.  If depth can't change, we don't need to store it out again"));
		/*ensureMsgf(DepthStencilAccess.IsStencilWrite() || StencilStoreAction == ERenderTargetStoreAction::ENoAction, TEXT("Stencil is read-only, but we are performing a store.  This is a waste on mobile.  If stencil can't change, we don't need to store it out again"));*/
	}

	bool operator==(const FRHIDepthRenderTargetView& Other) const
	{
		return
			Texture == Other.Texture &&
			DepthLoadAction == Other.DepthLoadAction &&
			DepthStoreAction == Other.DepthStoreAction &&
			StencilLoadAction == Other.StencilLoadAction &&
			StencilStoreAction == Other.StencilStoreAction &&
			DepthStencilAccess == Other.DepthStencilAccess;
	}
};

class FRHISetRenderTargetsInfo
{
public:
	// Color Render Targets Info
	FRHIRenderTargetView ColorRenderTarget[MaxSimultaneousRenderTargets];	
	int32 NumColorRenderTargets;
	bool bClearColor;

	// Color Render Targets Info
	FRHIRenderTargetView ColorResolveRenderTarget[MaxSimultaneousRenderTargets];	
	bool bHasResolveAttachments;

	// Depth/Stencil Render Target Info
	FRHIDepthRenderTargetView DepthStencilRenderTarget;	
	bool bClearDepth;
	bool bClearStencil;

	FRHITexture* ShadingRateTexture;
	EVRSRateCombiner ShadingRateTextureCombiner;

	uint8 MultiViewCount;

	FRHISetRenderTargetsInfo() :
		NumColorRenderTargets(0),
		bClearColor(false),
		bHasResolveAttachments(false),
		bClearDepth(false),
		ShadingRateTexture(nullptr),
		MultiViewCount(0)
	{}

	FRHISetRenderTargetsInfo(int32 InNumColorRenderTargets, const FRHIRenderTargetView* InColorRenderTargets, const FRHIDepthRenderTargetView& InDepthStencilRenderTarget) :
		NumColorRenderTargets(InNumColorRenderTargets),
		bClearColor(InNumColorRenderTargets > 0 && InColorRenderTargets[0].LoadAction == ERenderTargetLoadAction::EClear),
		bHasResolveAttachments(false),
		DepthStencilRenderTarget(InDepthStencilRenderTarget),		
		bClearDepth(InDepthStencilRenderTarget.Texture && InDepthStencilRenderTarget.DepthLoadAction == ERenderTargetLoadAction::EClear),
		ShadingRateTexture(nullptr),
		ShadingRateTextureCombiner(VRSRB_Passthrough)
	{
		Check(InNumColorRenderTargets <= 0 || InColorRenderTargets);
		for (int32 Index = 0; Index < InNumColorRenderTargets; ++Index)
		{
			ColorRenderTarget[Index] = InColorRenderTargets[Index];			
		}
	}
	// @todo metal mrt: This can go away after all the cleanup is done
	void SetClearDepthStencil(bool bInClearDepth, bool bInClearStencil = false)
	{
		if (bInClearDepth)
		{
			DepthStencilRenderTarget.DepthLoadAction = ERenderTargetLoadAction::EClear;
		}
		if (bInClearStencil)
		{
			DepthStencilRenderTarget.StencilLoadAction = ERenderTargetLoadAction::EClear;
		}
		bClearDepth = bInClearDepth;		
		bClearStencil = bInClearStencil;		
	}

	uint32 CalculateHash() const
	{
		// Need a separate struct so we can memzero/remove dependencies on reference counts
		struct FHashableStruct
		{
			// *2 for color and resolves, depth goes in the second-to-last slot, shading rate goes in the last slot
			FRHITexture* Texture[MaxSimultaneousRenderTargets*2 + 2];
			uint32 MipIndex[MaxSimultaneousRenderTargets];
			uint32 ArraySliceIndex[MaxSimultaneousRenderTargets];
			ERenderTargetLoadAction LoadAction[MaxSimultaneousRenderTargets];
			ERenderTargetStoreAction StoreAction[MaxSimultaneousRenderTargets];

			ERenderTargetLoadAction		DepthLoadAction;
			ERenderTargetStoreAction	DepthStoreAction;
			ERenderTargetLoadAction		StencilLoadAction;
			ERenderTargetStoreAction	StencilStoreAction;
			FExclusiveDepthStencil		DepthStencilAccess;

			bool bClearDepth;
			bool bClearStencil;
			bool bClearColor;
			bool bHasResolveAttachments;
			FRHIUnorderedAccessView* UnorderedAccessView[MaxSimultaneousUAVs];
			uint8 MultiViewCount;

			void Set(const FRHISetRenderTargetsInfo& RTInfo)
			{
				memset(this, 0, sizeof(*this));
				for (int32 Index = 0; Index < RTInfo.NumColorRenderTargets; ++Index)
				{
					Texture[Index] = RTInfo.ColorRenderTarget[Index].Texture;
					Texture[MaxSimultaneousRenderTargets+Index] = RTInfo.ColorResolveRenderTarget[Index].Texture;
					MipIndex[Index] = RTInfo.ColorRenderTarget[Index].MipIndex;
					ArraySliceIndex[Index] = RTInfo.ColorRenderTarget[Index].ArraySliceIndex;
					LoadAction[Index] = RTInfo.ColorRenderTarget[Index].LoadAction;
					StoreAction[Index] = RTInfo.ColorRenderTarget[Index].StoreAction;
				}

				Texture[MaxSimultaneousRenderTargets] = RTInfo.DepthStencilRenderTarget.Texture;
				Texture[MaxSimultaneousRenderTargets + 1] = RTInfo.ShadingRateTexture;
				DepthLoadAction = RTInfo.DepthStencilRenderTarget.DepthLoadAction;
				DepthStoreAction = RTInfo.DepthStencilRenderTarget.DepthStoreAction;
				StencilLoadAction = RTInfo.DepthStencilRenderTarget.StencilLoadAction;
				StencilStoreAction = RTInfo.DepthStencilRenderTarget.GetStencilStoreAction();
				DepthStencilAccess = RTInfo.DepthStencilRenderTarget.GetDepthStencilAccess();

				bClearDepth = RTInfo.bClearDepth;
				bClearStencil = RTInfo.bClearStencil;
				bClearColor = RTInfo.bClearColor;
				bHasResolveAttachments = RTInfo.bHasResolveAttachments;
				MultiViewCount = RTInfo.MultiViewCount;
			}
		};

		FHashableStruct RTHash{}; 
		RTHash.Set(*this);
		return FCrc::MemCrc32(&RTHash, sizeof(RTHash));
	}
};

class FRHICustomPresent : public FRHIResource
{
public:
	FRHICustomPresent() : FRHIResource(RRT_CustomPresent) {}
	
	virtual ~FRHICustomPresent() {} // should release any references to D3D resources.
	
	// Called when viewport is resized.
	virtual void OnBackBufferResize() = 0;

	// Called from render thread to see if a native present will be requested for this frame.
	// @return	true if native Present will be requested for this frame; false otherwise.  Must
	// match value subsequently returned by Present for this frame.
	virtual bool NeedsNativePresent() = 0;
	// In come cases we want to use custom present but still let the native environment handle 
	// advancement of the backbuffer indices.
	// @return true if backbuffer index should advance independently from CustomPresent.
	virtual bool NeedsAdvanceBackbuffer() { return false; };

	// Called from RHI thread when the engine begins drawing to the viewport.
	virtual void BeginDrawing() {};

	// Called from RHI thread to perform custom present.
	// @param InOutSyncInterval - in out param, indicates if vsync is on (>0) or off (==0).
	// @return	true if native Present should be also be performed; false otherwise. If it returns
	// true, then InOutSyncInterval could be modified to switch between VSync/NoVSync for the normal 
	// Present.  Must match value previously returned by NeedsNativePresent for this frame.
	virtual bool Present(int32& InOutSyncInterval) = 0;

	// Called from RHI thread after native Present has been called
	virtual void PostPresent() {};

	// Called when rendering thread is acquired
	virtual void OnAcquireThreadOwnership() {}
	// Called when rendering thread is released
	virtual void OnReleaseThreadOwnership() {}
};

template<typename TRHIShader> struct TRHIShaderToEnum {};
template<> struct TRHIShaderToEnum<FRHIVertexShader>           { enum { ShaderFrequency = SF_Vertex        }; };
template<> struct TRHIShaderToEnum<FRHIMeshShader>             { enum { ShaderFrequency = SF_Mesh          }; };
template<> struct TRHIShaderToEnum<FRHIAmplificationShader>    { enum { ShaderFrequency = SF_Amplification }; };
template<> struct TRHIShaderToEnum<FRHIPixelShader>            { enum { ShaderFrequency = SF_Pixel         }; };
template<> struct TRHIShaderToEnum<FRHIGeometryShader>         { enum { ShaderFrequency = SF_Geometry      }; };
//template<> struct TRHIShaderToEnum<FRHIComputeShader>          { enum { ShaderFrequency = SF_Compute       }; };
template<> struct TRHIShaderToEnum<FRHIVertexShader*>          { enum { ShaderFrequency = SF_Vertex        }; };
template<> struct TRHIShaderToEnum<FRHIMeshShader*>            { enum { ShaderFrequency = SF_Mesh          }; };
template<> struct TRHIShaderToEnum<FRHIAmplificationShader*>   { enum { ShaderFrequency = SF_Amplification }; };
template<> struct TRHIShaderToEnum<FRHIPixelShader*>           { enum { ShaderFrequency = SF_Pixel         }; };
template<> struct TRHIShaderToEnum<FRHIGeometryShader*>        { enum { ShaderFrequency = SF_Geometry      }; };
//template<> struct TRHIShaderToEnum<FRHIComputeShader*>         { enum { ShaderFrequency = SF_Compute       }; };
template<> struct TRHIShaderToEnum<shared_ptr<FRHIVertexShader>>        { enum { ShaderFrequency = SF_Vertex        }; };
//template<> struct TRHIShaderToEnum<FMeshShaderRHIRef>          { enum { ShaderFrequency = SF_Mesh          }; };
//template<> struct TRHIShaderToEnum<FAmplificationShaderRHIRef> { enum { ShaderFrequency = SF_Amplification }; };
template<> struct TRHIShaderToEnum<shared_ptr<FRHIPixelShader>>         { enum { ShaderFrequency = SF_Pixel         }; };
template<> struct TRHIShaderToEnum<shared_ptr<FRHIGeometryShader>> { enum { ShaderFrequency = SF_Geometry };	};
template<> struct TRHIShaderToEnum<shared_ptr<FRHIDomainShader>>         { enum { ShaderFrequency = SF_Domain	 }; };
template<> struct TRHIShaderToEnum<shared_ptr<FRHIHullShader>> { enum { ShaderFrequency = SF_Hull };};

//template<> struct TRHIShaderToEnum<FComputeShaderRHIRef>       { enum { ShaderFrequency = SF_Compute       }; };

template<typename TRHIShaderType>
inline const TCHAR* GetShaderFrequencyString(bool bIncludePrefix = true)
{
	return GetShaderFrequencyString(static_cast<EShaderFrequency>(TRHIShaderToEnum<TRHIShaderType>::ShaderFrequency), bIncludePrefix);
}

struct FBoundShaderStateInput
{
	inline FBoundShaderStateInput() {}

	inline FBoundShaderStateInput
	(
		FRHIVertexDeclaration* InVertexDeclarationRHI
		, FRHIVertexShader* InVertexShaderRHI
		, FRHIPixelShader* InPixelShaderRHI
		, FRHIGeometryShader* InGeometryShaderRHI,
		FRHIHullShader* InHullShader,
		FRHIDomainShader* InDomainShader
	)
		: VertexDeclarationRHI(InVertexDeclarationRHI)
		, VertexShaderRHI(InVertexShaderRHI)
		, PixelShaderRHI(InPixelShaderRHI)
		, GeometryShaderRHI(InGeometryShaderRHI)
		, HullShaderRHI(InHullShader)
		, DomainShaderRHI(InDomainShader)
	{
	}
	
	FRHIVertexShader* GetVertexShader() const { return VertexShaderRHI; }
	FRHIPixelShader* GetPixelShader() const { return PixelShaderRHI; }
	FRHIGeometryShader* GetGeometryShader() const { return GeometryShaderRHI; }
	FRHIHullShader* GetHullShader() const { return HullShaderRHI; }
	FRHIDomainShader* GetDomainShader() const { return DomainShaderRHI; }
	
	void SetGeometryShader(FRHIGeometryShader* InGeometryShader) { GeometryShaderRHI = InGeometryShader; }


	FRHIVertexDeclaration* VertexDeclarationRHI = nullptr;
	FRHIVertexShader* VertexShaderRHI = nullptr;
	FRHIPixelShader* PixelShaderRHI = nullptr;
	FRHIGeometryShader* GeometryShaderRHI = nullptr;
	FRHIHullShader* HullShaderRHI = nullptr;
	FRHIDomainShader* DomainShaderRHI = nullptr;

};

// Hints for some RHIs that support subpasses
enum class ESubpassHint : uint8
{
	// Regular rendering
	None,

	// Render pass has depth reading subpass
	DepthReadSubpass,

	// Mobile defferred shading subpass
	DeferredShadingSubpass,
};

enum class EConservativeRasterization : uint8
{
	Disabled,
	Overestimated,
};

enum ENoInit {NoInit};
enum EInPlace {InPlace};

struct FGraphicsPipelineRenderTargetsInfo
{
	FGraphicsPipelineRenderTargetsInfo()
		: DepthStencilAccess(FExclusiveDepthStencil::DepthNop_StencilNop)
	{
		RenderTargetFormats.fill(UE_PIXELFORMAT_TO_UINT8(PF_Unknown));
		RenderTargetFlags.fill(TexCreate_None);
	}

	uint32															RenderTargetsEnabled = 0;
	array<uint8, MaxSimultaneousRenderTargets>				RenderTargetFormats;
	array<ETextureCreateFlags, MaxSimultaneousRenderTargets>	RenderTargetFlags;
	EPixelFormat													DepthStencilTargetFormat = PF_Unknown;
	ETextureCreateFlags												DepthStencilTargetFlag = ETextureCreateFlags::None;
	ERenderTargetLoadAction											DepthTargetLoadAction = ERenderTargetLoadAction::ENoAction;
	ERenderTargetStoreAction										DepthTargetStoreAction = ERenderTargetStoreAction::ENoAction;
	ERenderTargetLoadAction											StencilTargetLoadAction = ERenderTargetLoadAction::ENoAction;
	ERenderTargetStoreAction										StencilTargetStoreAction = ERenderTargetStoreAction::ENoAction;
	FExclusiveDepthStencil											DepthStencilAccess;
	uint16															NumSamples = 0;
	uint8															MultiViewCount = 0;
	bool															bHasFragmentDensityAttachment = false;
};

class FGraphicsPipelineStateInitializer
{
public:
	// Can't use TEnumByte<EPixelFormat> as it changes the struct to be non trivially constructible, breaking memset
	using TRenderTargetFormats		= array<uint8/*EPixelFormat*/, MaxSimultaneousRenderTargets>;
	using TRenderTargetFlags		= array<ETextureCreateFlags, MaxSimultaneousRenderTargets>;

	FGraphicsPipelineStateInitializer()
		: BlendState(nullptr)
		, RasterizerState(nullptr)
		, DepthStencilState(nullptr)
		, RenderTargetsEnabled(0)
		, DepthStencilTargetFormat(PF_Unknown)
		, DepthStencilTargetFlag(TexCreate_None)
		, DepthTargetLoadAction(ERenderTargetLoadAction::ENoAction)
		, DepthTargetStoreAction(ERenderTargetStoreAction::ENoAction)
		, StencilTargetLoadAction(ERenderTargetLoadAction::ENoAction)
		, StencilTargetStoreAction(ERenderTargetStoreAction::ENoAction)
		, NumSamples(0)
		, SubpassHint(ESubpassHint::None)
		, SubpassIndex(0)
		, ConservativeRasterization(EConservativeRasterization::Disabled)
		, bDepthBounds(false)
		, MultiViewCount(0)
		, bHasFragmentDensityAttachment(false)
		, ShadingRate(EVRSShadingRate::VRSSR_1x1)
		, Flags(0)
		, StatePrecachePSOHash(0)
	{
		RenderTargetFormats.fill(UE_PIXELFORMAT_TO_UINT8(PF_Unknown));
		RenderTargetFlags.fill(TexCreate_None);
	}

	FGraphicsPipelineStateInitializer(
		FBoundShaderStateInput		InBoundShaderState,
		FRHIBlendState*				InBlendState,
		FRHIRasterizerState*		InRasterizerState,
		FRHIDepthStencilState*		InDepthStencilState,
		//FImmutableSamplerState		InImmutableSamplerState,
		EPrimitiveType				InPrimitiveType,
		uint32						InRenderTargetsEnabled,
		const TRenderTargetFormats&	InRenderTargetFormats,
		const TRenderTargetFlags&	InRenderTargetFlags,
		EPixelFormat				InDepthStencilTargetFormat,
		ETextureCreateFlags			InDepthStencilTargetFlag,
		ERenderTargetLoadAction		InDepthTargetLoadAction,
		ERenderTargetStoreAction	InDepthTargetStoreAction,
		ERenderTargetLoadAction		InStencilTargetLoadAction,
		ERenderTargetStoreAction	InStencilTargetStoreAction,
		FExclusiveDepthStencil		InDepthStencilAccess,
		uint16						InNumSamples,
		ESubpassHint				InSubpassHint,
		uint8						InSubpassIndex,
		EConservativeRasterization	InConservativeRasterization,
		uint16						InFlags,
		bool						bInDepthBounds,
		uint8						InMultiViewCount,
		bool						bInHasFragmentDensityAttachment,
		EVRSShadingRate				InShadingRate)
		: BoundShaderState(InBoundShaderState)
		, BlendState(InBlendState)
		, RasterizerState(InRasterizerState)
		, DepthStencilState(InDepthStencilState)
		//, ImmutableSamplerState(InImmutableSamplerState)
		, PrimitiveType(InPrimitiveType)
		, RenderTargetsEnabled(InRenderTargetsEnabled)
		, RenderTargetFormats(InRenderTargetFormats)
		, RenderTargetFlags(InRenderTargetFlags)
		, DepthStencilTargetFormat(InDepthStencilTargetFormat)
		, DepthStencilTargetFlag(InDepthStencilTargetFlag)
		, DepthTargetLoadAction(InDepthTargetLoadAction)
		, DepthTargetStoreAction(InDepthTargetStoreAction)
		, StencilTargetLoadAction(InStencilTargetLoadAction)
		, StencilTargetStoreAction(InStencilTargetStoreAction)
		, DepthStencilAccess(InDepthStencilAccess)
		, NumSamples(InNumSamples)
		, SubpassHint(InSubpassHint)
		, SubpassIndex(InSubpassIndex)
		, ConservativeRasterization(EConservativeRasterization::Disabled)
		, bDepthBounds(bInDepthBounds)
		, MultiViewCount(InMultiViewCount)
		, bHasFragmentDensityAttachment(bInHasFragmentDensityAttachment)
		, ShadingRate(InShadingRate)
		, Flags(InFlags)
		, StatePrecachePSOHash(0)
	{
	}

	bool operator==(const FGraphicsPipelineStateInitializer& rhs) const
	{
		if (BoundShaderState.VertexDeclarationRHI != rhs.BoundShaderState.VertexDeclarationRHI ||
			BoundShaderState.VertexShaderRHI != rhs.BoundShaderState.VertexShaderRHI ||
			BoundShaderState.PixelShaderRHI != rhs.BoundShaderState.PixelShaderRHI ||
			//BoundShaderState.GetMeshShader() != rhs.BoundShaderState.GetMeshShader() ||
			//BoundShaderState.GetAmplificationShader() != rhs.BoundShaderState.GetAmplificationShader() ||
			BoundShaderState.GetGeometryShader() != rhs.BoundShaderState.GetGeometryShader() ||
			BlendState != rhs.BlendState ||
			RasterizerState != rhs.RasterizerState ||
			DepthStencilState != rhs.DepthStencilState ||
			//ImmutableSamplerState != rhs.ImmutableSamplerState ||
			PrimitiveType != rhs.PrimitiveType ||
			bDepthBounds != rhs.bDepthBounds ||
			MultiViewCount != rhs.MultiViewCount ||
			ShadingRate != rhs.ShadingRate ||
			bHasFragmentDensityAttachment != rhs.bHasFragmentDensityAttachment ||
			RenderTargetsEnabled != rhs.RenderTargetsEnabled ||
			RenderTargetFormats != rhs.RenderTargetFormats || 
			!RelevantRenderTargetFlagsEqual(RenderTargetFlags, rhs.RenderTargetFlags) || 
			DepthStencilTargetFormat != rhs.DepthStencilTargetFormat || 
			!RelevantDepthStencilFlagsEqual(DepthStencilTargetFlag, rhs.DepthStencilTargetFlag) ||
			DepthTargetLoadAction != rhs.DepthTargetLoadAction ||
			DepthTargetStoreAction != rhs.DepthTargetStoreAction ||
			StencilTargetLoadAction != rhs.StencilTargetLoadAction ||
			StencilTargetStoreAction != rhs.StencilTargetStoreAction || 
			DepthStencilAccess != rhs.DepthStencilAccess ||
			NumSamples != rhs.NumSamples ||
			SubpassHint != rhs.SubpassHint ||
			SubpassIndex != rhs.SubpassIndex ||
			ConservativeRasterization != rhs.ConservativeRasterization)
		{
			return false;
		}

		return true;
	}

	// We care about flags that influence RT formats (which is the only thing the underlying API cares about).
	// In most RHIs, the format is only influenced by TexCreate_SRGB. D3D12 additionally uses TexCreate_Shared in its format selection logic.
	static constexpr ETextureCreateFlags RelevantRenderTargetFlagMask = ETextureCreateFlags::SRGB | ETextureCreateFlags::Shared;

	// We care about flags that influence DS formats (which is the only thing the underlying API cares about).
	// D3D12 shares the format choice function with the RT, so preserving all the flags used there out of abundance of caution.
	static constexpr ETextureCreateFlags RelevantDepthStencilFlagMask = ETextureCreateFlags::SRGB | ETextureCreateFlags::Shared | ETextureCreateFlags::DepthStencilTargetable;

	static bool RelevantRenderTargetFlagsEqual(const TRenderTargetFlags& A, const TRenderTargetFlags& B)
	{
		for (int32 Index = 0; Index < A.size(); ++Index)
		{
			ETextureCreateFlags FlagsA = A[Index] & RelevantRenderTargetFlagMask;
			ETextureCreateFlags FlagsB = B[Index] & RelevantRenderTargetFlagMask;
			if (FlagsA != FlagsB)
			{
				return false;
			}
		}
		return true;
	}

	static bool RelevantDepthStencilFlagsEqual(const ETextureCreateFlags A, const ETextureCreateFlags B)
	{
		ETextureCreateFlags FlagsA = (A & RelevantDepthStencilFlagMask);
		ETextureCreateFlags FlagsB = (B & RelevantDepthStencilFlagMask);
		return (FlagsA == FlagsB);
	}

	uint32 ComputeNumValidRenderTargets() const
	{
		// Get the count of valid render targets (ignore those at the end of the array with PF_Unknown)
		if (RenderTargetsEnabled > 0)
		{
			int32 LastValidTarget = -1;
			for (int32 i = (int32)RenderTargetsEnabled - 1; i >= 0; i--)
			{
				if (RenderTargetFormats[i] != PF_Unknown)
				{
					LastValidTarget = i;
					break;
				}
			}
			return uint32(LastValidTarget + 1);
		}
		return RenderTargetsEnabled;
	}

	FBoundShaderStateInput			BoundShaderState;
	FRHIBlendState*					BlendState;
	FRHIRasterizerState*			RasterizerState;
	FRHIDepthStencilState*			DepthStencilState;
	//FImmutableSamplerState			ImmutableSamplerState;

	EPrimitiveType					PrimitiveType;
	uint32							RenderTargetsEnabled;
	TRenderTargetFormats			RenderTargetFormats;
	TRenderTargetFlags				RenderTargetFlags;
	EPixelFormat					DepthStencilTargetFormat;
	ETextureCreateFlags				DepthStencilTargetFlag;
	ERenderTargetLoadAction			DepthTargetLoadAction;
	ERenderTargetStoreAction		DepthTargetStoreAction;
	ERenderTargetLoadAction			StencilTargetLoadAction;
	ERenderTargetStoreAction		StencilTargetStoreAction;
	FExclusiveDepthStencil			DepthStencilAccess;
	uint16							NumSamples;
	ESubpassHint					SubpassHint;
	uint8							SubpassIndex;
	EConservativeRasterization		ConservativeRasterization;
	bool							bDepthBounds;
	uint8							MultiViewCount;
	bool							bHasFragmentDensityAttachment;
	EVRSShadingRate					ShadingRate;
	
	
	union
	{
		struct
		{
			uint16					Reserved			: 14;
			uint16					bPSOPrecache			: 1;
			uint16					bFromPSOFileCache	: 1;
		};
		uint16						Flags;
	};

	
	uint64							StatePrecachePSOHash;
};

// This PSO is used as a fallback for RHIs that dont support PSOs. It is used to set the graphics state using the legacy state setting APIs
class FRHIGraphicsPipelineStateFallBack : public FRHIGraphicsPipelineState
{
public:
	FRHIGraphicsPipelineStateFallBack() {}

	FRHIGraphicsPipelineStateFallBack(const FGraphicsPipelineStateInitializer& Init)
		: Initializer(Init)
	{
	}

	FGraphicsPipelineStateInitializer Initializer;
};

enum class ERenderTargetActions : uint8
{
	LoadOpMask = 2,

#define RTACTION_MAKE_MASK(Load, Store) (((uint8)ERenderTargetLoadAction::Load << (uint8)LoadOpMask) | (uint8)ERenderTargetStoreAction::Store)

	DontLoad_DontStore =	RTACTION_MAKE_MASK(ENoAction, ENoAction),

	DontLoad_Store =		RTACTION_MAKE_MASK(ENoAction, EStore),
	Clear_Store =			RTACTION_MAKE_MASK(EClear, EStore),
	Load_Store =			RTACTION_MAKE_MASK(ELoad, EStore),

	Clear_DontStore =		RTACTION_MAKE_MASK(EClear, ENoAction),
	Load_DontStore =		RTACTION_MAKE_MASK(ELoad, ENoAction),
	Clear_Resolve =			RTACTION_MAKE_MASK(EClear, EMultisampleResolve),
	Load_Resolve =			RTACTION_MAKE_MASK(ELoad, EMultisampleResolve),

#undef RTACTION_MAKE_MASK
};


inline ERenderTargetActions MakeRenderTargetActions(ERenderTargetLoadAction Load, ERenderTargetStoreAction Store)
{
	return (ERenderTargetActions)(((uint8)Load << (uint8)ERenderTargetActions::LoadOpMask) | (uint8)Store);
}

inline ERenderTargetLoadAction GetLoadAction(ERenderTargetActions Action)
{
	return (ERenderTargetLoadAction)((uint8)Action >> (uint8)ERenderTargetActions::LoadOpMask);
}

inline ERenderTargetStoreAction GetStoreAction(ERenderTargetActions Action)
{
	return (ERenderTargetStoreAction)((uint8)Action & ((1 << (uint8)ERenderTargetActions::LoadOpMask) - 1));
}

enum class EDepthStencilTargetActions : uint8
{
	DepthMask = 4,

#define RTACTION_MAKE_MASK(Depth, Stencil) (((uint8)ERenderTargetActions::Depth << (uint8)DepthMask) | (uint8)ERenderTargetActions::Stencil)

	DontLoad_DontStore =						RTACTION_MAKE_MASK(DontLoad_DontStore, DontLoad_DontStore),
	DontLoad_StoreDepthStencil =				RTACTION_MAKE_MASK(DontLoad_Store, DontLoad_Store),
	DontLoad_StoreStencilNotDepth =				RTACTION_MAKE_MASK(DontLoad_DontStore, DontLoad_Store),
	ClearDepthStencil_StoreDepthStencil =		RTACTION_MAKE_MASK(Clear_Store, Clear_Store),
	LoadDepthStencil_StoreDepthStencil =		RTACTION_MAKE_MASK(Load_Store, Load_Store),
	LoadDepthNotStencil_StoreDepthNotStencil =	RTACTION_MAKE_MASK(Load_Store, DontLoad_DontStore),
	LoadDepthNotStencil_DontStore =				RTACTION_MAKE_MASK(Load_DontStore, DontLoad_DontStore),
	LoadDepthStencil_StoreStencilNotDepth =		RTACTION_MAKE_MASK(Load_DontStore, Load_Store),

	ClearDepthStencil_DontStoreDepthStencil =	RTACTION_MAKE_MASK(Clear_DontStore, Clear_DontStore),
	LoadDepthStencil_DontStoreDepthStencil =	RTACTION_MAKE_MASK(Load_DontStore, Load_DontStore),
	ClearDepthStencil_StoreDepthNotStencil =	RTACTION_MAKE_MASK(Clear_Store, Clear_DontStore),
	ClearDepthStencil_StoreStencilNotDepth =	RTACTION_MAKE_MASK(Clear_DontStore, Clear_Store),
	ClearDepthStencil_ResolveDepthNotStencil =	RTACTION_MAKE_MASK(Clear_Resolve, Clear_DontStore),
	ClearDepthStencil_ResolveStencilNotDepth =	RTACTION_MAKE_MASK(Clear_DontStore, Clear_Resolve),
	LoadDepthClearStencil_StoreDepthStencil  =  RTACTION_MAKE_MASK(Load_Store, Clear_Store),

	ClearStencilDontLoadDepth_StoreStencilNotDepth = RTACTION_MAKE_MASK(DontLoad_DontStore, Clear_Store),

#undef RTACTION_MAKE_MASK
};


inline constexpr EDepthStencilTargetActions MakeDepthStencilTargetActions(const ERenderTargetActions Depth, const ERenderTargetActions Stencil)
{
	return (EDepthStencilTargetActions)(((uint8)Depth << (uint8)EDepthStencilTargetActions::DepthMask) | (uint8)Stencil);
}

inline ERenderTargetActions GetDepthActions(EDepthStencilTargetActions Action)
{
	return (ERenderTargetActions)((uint8)Action >> (uint8)EDepthStencilTargetActions::DepthMask);
}

inline ERenderTargetActions GetStencilActions(EDepthStencilTargetActions Action)
{
	return (ERenderTargetActions)((uint8)Action & ((1 << (uint8)EDepthStencilTargetActions::DepthMask) - 1));
}

struct FIntRect
{
	Vector2 Min;
	Vector2 Max;
};

struct FResolveRect
{
	int32 X1;
	int32 Y1;
	int32 X2;
	int32 Y2;

	// e.g. for a a full 256 x 256 area starting at (0, 0) it would be 
	// the values would be 0, 0, 256, 256
	FResolveRect(int32 InX1 = -1, int32 InY1 = -1, int32 InX2 = -1, int32 InY2 = -1)
		: X1(InX1)
		, Y1(InY1)
		, X2(InX2)
		, Y2(InY2)
	{}

	explicit FResolveRect(FIntRect Other)
		: X1(static_cast<int32>(Other.Min.x))
		, Y1(static_cast<int32>(Other.Min.y))
		, X2(static_cast<int32>(Other.Max.x))
		, Y2(static_cast<int32>(Other.Max.y))
	{}

	bool operator==(FResolveRect Other) const
	{
		return X1 == Other.X1 && Y1 == Other.Y1 && X2 == Other.X2 && Y2 == Other.Y2;
	}

	bool operator!=(FResolveRect Other) const
	{
		return !(*this == Other);
	}

	bool IsValid() const
	{
		return X1 >= 0 && Y1 >= 0 && X2 - X1 > 0 && Y2 - Y1 > 0;
	}
};

struct FRHIRenderPassInfo
{
	struct FColorEntry
	{
		FRHITexture*         RenderTarget      = nullptr;
		FRHITexture*         ResolveTarget     = nullptr;
		int32                ArraySlice        = -1;
		uint8                MipIndex          = 0;
		ERenderTargetActions Action            = ERenderTargetActions::DontLoad_DontStore;
	};
	array<FColorEntry, MaxSimultaneousRenderTargets> ColorRenderTargets;

	struct FDepthStencilEntry
	{
		FRHITexture*         DepthStencilTarget = nullptr;
		FRHITexture*         ResolveTarget      = nullptr;
		EDepthStencilTargetActions Action       = EDepthStencilTargetActions::DontLoad_DontStore;
		FExclusiveDepthStencil ExclusiveDepthStencil;
	};
	FDepthStencilEntry DepthStencilRenderTarget;

	// Controls the area for a multisample resolve or raster UAV (i.e. no fixed-function targets) operation.
	FResolveRect ResolveRect;

	// Some RHIs can use a texture to control the sampling and/or shading resolution of different areas 
	shared_ptr<FRHITexture> ShadingRateTexture = nullptr;
	EVRSRateCombiner ShadingRateTextureCombiner = VRSRB_Passthrough;

	// Some RHIs require a hint that occlusion queries will be used in this render pass
	uint32 NumOcclusionQueries = 0;
	bool bOcclusionQueries = false;

	// if this renderpass should be multiview, and if so how many views are required
	uint8 MultiViewCount = 0;

	// Hint for some RHI's that renderpass will have specific sub-passes 
	ESubpassHint SubpassHint = ESubpassHint::None;

	FRHIRenderPassInfo() = default;
	FRHIRenderPassInfo(const FRHIRenderPassInfo&) = default;
	FRHIRenderPassInfo& operator=(const FRHIRenderPassInfo&) = default;

	// Color, no depth, optional resolve, optional mip, optional array slice
	explicit FRHIRenderPassInfo(FRHITexture* ColorRT, ERenderTargetActions ColorAction, FRHITexture* ResolveRT = nullptr, uint8 InMipIndex = 0, int32 InArraySlice = -1)
	{
		Check(!ResolveRT || ResolveRT->IsMultisampled());
		Check(ColorRT);
		ColorRenderTargets[0].RenderTarget = ColorRT;
		ColorRenderTargets[0].ResolveTarget = ResolveRT;
		ColorRenderTargets[0].ArraySlice = InArraySlice;
		ColorRenderTargets[0].MipIndex = InMipIndex;
		ColorRenderTargets[0].Action = ColorAction;
	}

	// Color MRTs, no depth
	explicit FRHIRenderPassInfo(int32 NumColorRTs, FRHITexture* ColorRTs[], ERenderTargetActions ColorAction)
	{
		Check(NumColorRTs > 0);
		for (int32 Index = 0; Index < NumColorRTs; ++Index)
		{
			Check(ColorRTs[Index]);
			ColorRenderTargets[Index].RenderTarget = ColorRTs[Index];
			ColorRenderTargets[Index].ArraySlice = -1;
			ColorRenderTargets[Index].Action = ColorAction;
		}
		DepthStencilRenderTarget.DepthStencilTarget = nullptr;
		DepthStencilRenderTarget.Action = EDepthStencilTargetActions::DontLoad_DontStore;
		DepthStencilRenderTarget.ExclusiveDepthStencil = FExclusiveDepthStencil::DepthNop_StencilNop;
		DepthStencilRenderTarget.ResolveTarget = nullptr;
	}

	// Color MRTs, no depth
	explicit FRHIRenderPassInfo(int32 NumColorRTs, FRHITexture* ColorRTs[], ERenderTargetActions ColorAction, FRHITexture* ResolveTargets[])
	{
		Check(NumColorRTs > 0);
		for (int32 Index = 0; Index < NumColorRTs; ++Index)
		{
			Check(ColorRTs[Index]);
			ColorRenderTargets[Index].RenderTarget = ColorRTs[Index];
			ColorRenderTargets[Index].ResolveTarget = ResolveTargets[Index];
			ColorRenderTargets[Index].ArraySlice = -1;
			ColorRenderTargets[Index].MipIndex = 0;
			ColorRenderTargets[Index].Action = ColorAction;
		}
		DepthStencilRenderTarget.DepthStencilTarget = nullptr;
		DepthStencilRenderTarget.Action = EDepthStencilTargetActions::DontLoad_DontStore;
		DepthStencilRenderTarget.ExclusiveDepthStencil = FExclusiveDepthStencil::DepthNop_StencilNop;
		DepthStencilRenderTarget.ResolveTarget = nullptr;
	}

	// Color MRTs and depth
	explicit FRHIRenderPassInfo(int32 NumColorRTs, FRHITexture* ColorRTs[], ERenderTargetActions ColorAction, FRHITexture* DepthRT, EDepthStencilTargetActions DepthActions, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		Check(NumColorRTs > 0);
		for (int32 Index = 0; Index < NumColorRTs; ++Index)
		{
			Check(ColorRTs[Index]);
			ColorRenderTargets[Index].RenderTarget = ColorRTs[Index];
			ColorRenderTargets[Index].ResolveTarget = nullptr;
			ColorRenderTargets[Index].ArraySlice = -1;
			ColorRenderTargets[Index].MipIndex = 0;
			ColorRenderTargets[Index].Action = ColorAction;
		}
		Check(DepthRT);
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = nullptr;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
	}

	// Color MRTs and depth
	explicit FRHIRenderPassInfo(int32 NumColorRTs, FRHITexture* ColorRTs[], ERenderTargetActions ColorAction, FRHITexture* ResolveRTs[], FRHITexture* DepthRT, EDepthStencilTargetActions DepthActions, FRHITexture* ResolveDepthRT, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		Check(NumColorRTs > 0);
		for (int32 Index = 0; Index < NumColorRTs; ++Index)
		{
			Check(!ResolveRTs[Index] || ResolveRTs[Index]->IsMultisampled());
			Check(ColorRTs[Index]);
			ColorRenderTargets[Index].RenderTarget = ColorRTs[Index];
			ColorRenderTargets[Index].ResolveTarget = ResolveRTs[Index];
			ColorRenderTargets[Index].ArraySlice = -1;
			ColorRenderTargets[Index].MipIndex = 0;
			ColorRenderTargets[Index].Action = ColorAction;
		}
		Check(!ResolveDepthRT || ResolveDepthRT->IsMultisampled());
		Check(DepthRT);
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = ResolveDepthRT;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
	}

	// Depth, no color
	explicit FRHIRenderPassInfo(FRHITexture* DepthRT, EDepthStencilTargetActions DepthActions, FRHITexture* ResolveDepthRT = nullptr, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		Check(!ResolveDepthRT || ResolveDepthRT->IsMultisampled());
		Check(DepthRT);
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = ResolveDepthRT;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
	}

	// Depth, no color, occlusion queries
	explicit FRHIRenderPassInfo(FRHITexture* DepthRT, uint32 InNumOcclusionQueries, EDepthStencilTargetActions DepthActions, FRHITexture* ResolveDepthRT = nullptr, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite)
		: NumOcclusionQueries(InNumOcclusionQueries)
	{
		Check(!ResolveDepthRT || ResolveDepthRT->IsMultisampled());
		Check(DepthRT);
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = ResolveDepthRT;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
	}

	// Color and depth
	explicit FRHIRenderPassInfo(FRHITexture* ColorRT, ERenderTargetActions ColorAction, FRHITexture* DepthRT, EDepthStencilTargetActions DepthActions, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		Check(ColorRT);
		ColorRenderTargets[0].RenderTarget = ColorRT;
		ColorRenderTargets[0].ResolveTarget = nullptr;
		ColorRenderTargets[0].ArraySlice = -1;
		ColorRenderTargets[0].MipIndex = 0;
		ColorRenderTargets[0].Action = ColorAction;
		Check(DepthRT);
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = nullptr;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
		std::memset(&ColorRenderTargets[1], 0, sizeof(FColorEntry) * (MaxSimultaneousRenderTargets - 1));
	}

	// Color and depth with resolve
	explicit FRHIRenderPassInfo(FRHITexture* ColorRT, ERenderTargetActions ColorAction, FRHITexture* ResolveColorRT,
		FRHITexture* DepthRT, EDepthStencilTargetActions DepthActions, FRHITexture* ResolveDepthRT, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		Check(!ResolveColorRT || ResolveColorRT->IsMultisampled());
		Check(!ResolveDepthRT || ResolveDepthRT->IsMultisampled());
		Check(ColorRT);
		ColorRenderTargets[0].RenderTarget = ColorRT;
		ColorRenderTargets[0].ResolveTarget = ResolveColorRT;
		ColorRenderTargets[0].ArraySlice = -1;
		ColorRenderTargets[0].MipIndex = 0;
		ColorRenderTargets[0].Action = ColorAction;
		Check(DepthRT);
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = ResolveDepthRT;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
		std::memset(&ColorRenderTargets[1], 0, sizeof(FColorEntry) * (MaxSimultaneousRenderTargets - 1));
	}

	// Color and depth with resolve and optional sample density
	explicit FRHIRenderPassInfo(FRHITexture* ColorRT, ERenderTargetActions ColorAction, FRHITexture* ResolveColorRT,
		FRHITexture* DepthRT, EDepthStencilTargetActions DepthActions, FRHITexture* ResolveDepthRT, 
		FRHITexture* InShadingRateTexture, EVRSRateCombiner InShadingRateTextureCombiner,
		FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		Check(!ResolveColorRT || ResolveColorRT->IsMultisampled());
		Check(!ResolveDepthRT || ResolveDepthRT->IsMultisampled());
		Check(ColorRT);
		ColorRenderTargets[0].RenderTarget = ColorRT;
		ColorRenderTargets[0].ResolveTarget = ResolveColorRT;
		ColorRenderTargets[0].ArraySlice = -1;
		ColorRenderTargets[0].MipIndex = 0;
		ColorRenderTargets[0].Action = ColorAction;
		Check(DepthRT);
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = ResolveDepthRT;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
		ShadingRateTexture = shared_ptr<FRHITexture>(InShadingRateTexture);
		ShadingRateTextureCombiner = InShadingRateTextureCombiner;
		std::memset(&ColorRenderTargets[1], 0, sizeof(FColorEntry) * (MaxSimultaneousRenderTargets - 1));
	}

	inline int32 GetNumColorRenderTargets() const
	{
		int32 ColorIndex = 0;
		for (; ColorIndex < MaxSimultaneousRenderTargets; ++ColorIndex)
		{
			const FColorEntry& Entry = ColorRenderTargets[ColorIndex];
			if (!Entry.RenderTarget)
			{
				break;
			}
		}

		return ColorIndex;
	}

	FGraphicsPipelineRenderTargetsInfo ExtractRenderTargetsInfo() const
	{
		FGraphicsPipelineRenderTargetsInfo RenderTargetsInfo;

		RenderTargetsInfo.NumSamples = 1;
		int32 RenderTargetIndex = 0;

		for (; RenderTargetIndex < MaxSimultaneousRenderTargets; ++RenderTargetIndex)
		{
			FRHITexture* RenderTarget = ColorRenderTargets[RenderTargetIndex].RenderTarget;
			if (!RenderTarget)
			{
				break;
			}

			RenderTargetsInfo.RenderTargetFormats[RenderTargetIndex] = (uint8)RenderTarget->GetFormat();
			RenderTargetsInfo.RenderTargetFlags[RenderTargetIndex] = RenderTarget->GetFlags();
			RenderTargetsInfo.NumSamples |= RenderTarget->GetNumSamples();
		}

		RenderTargetsInfo.RenderTargetsEnabled = RenderTargetIndex;
		for (; RenderTargetIndex < MaxSimultaneousRenderTargets; ++RenderTargetIndex)
		{
			RenderTargetsInfo.RenderTargetFormats[RenderTargetIndex] = PF_Unknown;
		}

		if (DepthStencilRenderTarget.DepthStencilTarget)
		{
			RenderTargetsInfo.DepthStencilTargetFormat = DepthStencilRenderTarget.DepthStencilTarget->GetFormat();
			RenderTargetsInfo.DepthStencilTargetFlag = DepthStencilRenderTarget.DepthStencilTarget->GetFlags();
			RenderTargetsInfo.NumSamples |= DepthStencilRenderTarget.DepthStencilTarget->GetNumSamples();
		}
		else
		{
			RenderTargetsInfo.DepthStencilTargetFormat = PF_Unknown;
		}

		const ERenderTargetActions DepthActions = GetDepthActions(DepthStencilRenderTarget.Action);
		const ERenderTargetActions StencilActions = GetStencilActions(DepthStencilRenderTarget.Action);
		RenderTargetsInfo.DepthTargetLoadAction = GetLoadAction(DepthActions);
		RenderTargetsInfo.DepthTargetStoreAction = GetStoreAction(DepthActions);
		RenderTargetsInfo.StencilTargetLoadAction = GetLoadAction(StencilActions);
		RenderTargetsInfo.StencilTargetStoreAction = GetStoreAction(StencilActions);
		RenderTargetsInfo.DepthStencilAccess = DepthStencilRenderTarget.ExclusiveDepthStencil;

		RenderTargetsInfo.MultiViewCount = MultiViewCount;
		RenderTargetsInfo.bHasFragmentDensityAttachment = ShadingRateTexture != nullptr;

		return RenderTargetsInfo;
	}

	
	void Validate() const {}
	void ConvertToRenderTargetsInfo(FRHISetRenderTargetsInfo& OutRTInfo) const;
};

using ERHITextureMetaDataAccess = ERHITexturePlane;

enum ERHITextureSRVOverrideSRGBType : uint8
{
	SRGBO_Default,
	SRGBO_ForceDisable,
};

struct FRHITextureSRVCreateInfo
{
	explicit FRHITextureSRVCreateInfo(uint8 InMipLevel = 0u, uint8 InNumMipLevels = 1u, EPixelFormat InFormat = PF_Unknown)
		: Format(InFormat)
		, MipLevel(InMipLevel)
		, NumMipLevels(InNumMipLevels)
		, SRGBOverride(SRGBO_Default)
		, FirstArraySlice(0)
		, NumArraySlices(0)
	{}

	explicit FRHITextureSRVCreateInfo(uint8 InMipLevel, uint8 InNumMipLevels, uint16 InFirstArraySlice, uint16 InNumArraySlices, EPixelFormat InFormat = PF_Unknown)
		: Format(InFormat)
		, MipLevel(InMipLevel)
		, NumMipLevels(InNumMipLevels)
		, SRGBOverride(SRGBO_Default)
		, FirstArraySlice(InFirstArraySlice)
		, NumArraySlices(InNumArraySlices)
	{}

	/** View the texture with a different format. Leave as PF_Unknown to use original format. Useful when sampling stencil */
	EPixelFormat Format;

	/** Specify the mip level to use. Useful when rendering to one mip while sampling from another */
	uint8 MipLevel;

	/** Create a view to a single, or multiple mip levels */
	uint8 NumMipLevels;

	/** Potentially override the texture's sRGB flag */
	ERHITextureSRVOverrideSRGBType SRGBOverride;

	/** Specify first array slice index. By default 0. */
	uint16 FirstArraySlice;

	/** Specify number of array slices. If FirstArraySlice and NumArraySlices are both zero, the SRV is created for all array slices. By default 0. */
	uint16 NumArraySlices;

	/** Specify the metadata plane to use when creating a view. */
	ERHITextureMetaDataAccess MetaData = ERHITextureMetaDataAccess::None;
    
    /** Specify a dimension to use which overrides the default  */
    std::optional<ETextureDimension> DimensionOverride;

	FORCEINLINE bool operator==(const FRHITextureSRVCreateInfo& Other)const
	{
		return (
			Format == Other.Format &&
			MipLevel == Other.MipLevel &&
			NumMipLevels == Other.NumMipLevels &&
			SRGBOverride == Other.SRGBOverride &&
			FirstArraySlice == Other.FirstArraySlice &&
			NumArraySlices == Other.NumArraySlices &&
			MetaData == Other.MetaData &&
            DimensionOverride == Other.DimensionOverride);
	}

	FORCEINLINE bool operator!=(const FRHITextureSRVCreateInfo& Other)const
	{
		return !(*this == Other);
	}

	friend uint32 GetTypeHash(const FRHITextureSRVCreateInfo& Info)
	{
		uint32 Hash = uint32(Info.Format) | uint32(Info.MipLevel) << 8 | uint32(Info.NumMipLevels) << 16 | uint32(Info.SRGBOverride) << 24;
		Hash = HashCombine(Hash, uint32(Info.FirstArraySlice) | uint32(Info.NumArraySlices) << 16);
		Hash = HashCombine(Hash, Info.DimensionOverride.has_value() ? uint32(*Info.DimensionOverride) : MAX_uint32);
		Hash = HashCombine(Hash, uint32(Info.MetaData));
		return Hash;
	}

	/** Check the validity. */
	static bool CheckValidity(const FRHITextureDesc& TextureDesc, const FRHITextureSRVCreateInfo& TextureSRVDesc, const TCHAR* TextureName)
	{
		return FRHITextureSRVCreateInfo::Validate(TextureDesc, TextureSRVDesc, TextureName, /* bFatal = */ true);
	}

protected:
	static bool Validate(const FRHITextureDesc& TextureDesc, const FRHITextureSRVCreateInfo& TextureSRVDesc, const TCHAR* TextureName, bool bFatal);
};


struct FRHITextureUAVCreateInfo
{
public:
	FRHITextureUAVCreateInfo() = default;

	explicit FRHITextureUAVCreateInfo(uint8 InMipLevel, EPixelFormat InFormat = PF_Unknown, uint16 InFirstArraySlice = 0, uint16 InNumArraySlices = 0)
		: Format(InFormat)
		, MipLevel(InMipLevel)
		, FirstArraySlice(InFirstArraySlice)
		, NumArraySlices(InNumArraySlices)
	{}

	explicit FRHITextureUAVCreateInfo(ERHITextureMetaDataAccess InMetaData)
		: MetaData(InMetaData)
	{}

	FORCEINLINE bool operator==(const FRHITextureUAVCreateInfo& Other)const
	{
		return Format == Other.Format && MipLevel == Other.MipLevel && MetaData == Other.MetaData &&
                FirstArraySlice == Other.FirstArraySlice && NumArraySlices == Other.NumArraySlices &&
                DimensionOverride == Other.DimensionOverride;
	}

	FORCEINLINE bool operator!=(const FRHITextureUAVCreateInfo& Other)const
	{
		return !(*this == Other);
	}

	friend uint32 GetTypeHash(const FRHITextureUAVCreateInfo& Info)
	{
		uint32 Hash = uint32(Info.Format) | uint32(Info.MipLevel) << 8 | uint32(Info.FirstArraySlice) << 16;
        Hash = HashCombine(Hash, Info.DimensionOverride.has_value() ? uint32(*Info.DimensionOverride) : MAX_uint32);
		Hash = HashCombine(Hash, uint32(Info.NumArraySlices) | uint32(Info.MetaData) << 16);
		return Hash;
	}

	EPixelFormat Format = PF_Unknown;
	uint8 MipLevel = 0;
	uint16 FirstArraySlice = 0;
	uint16 NumArraySlices = 0;	// When 0, the default behavior will be used, e.g. all slices mapped.
	ERHITextureMetaDataAccess MetaData = ERHITextureMetaDataAccess::None;
    
    /** Specify a dimension to use which overrides the default  */
    std::optional<ETextureDimension> DimensionOverride;
};

/** Descriptor used to create a buffer resource */
struct FRHIBufferCreateInfo
{
	bool operator == (const FRHIBufferCreateInfo& Other) const
	{
		return (
			Size == Other.Size &&
			Stride == Other.Stride &&
			Usage == Other.Usage);
	}

	bool operator != (const FRHIBufferCreateInfo& Other) const
	{
		return !(*this == Other);
	}

	/** Total size of the buffer. */
	uint32 Size = 1;

	/** Stride in bytes */
	uint32 Stride = 1;

	/** Bitfields describing the uses of that buffer. */
	EBufferUsageFlags Usage = BUF_None;
};

struct FRHIBufferSRVCreateInfo
{
	explicit FRHIBufferSRVCreateInfo() = default;

	explicit FRHIBufferSRVCreateInfo(EPixelFormat InFormat)
		: Format(InFormat)
	{}

	FRHIBufferSRVCreateInfo(uint32 InStartOffsetBytes, uint32 InNumElements)
		: StartOffsetBytes(InStartOffsetBytes)
		, NumElements(InNumElements)
	{}

	FORCEINLINE bool operator==(const FRHIBufferSRVCreateInfo& Other)const
	{
		return Format == Other.Format
			&& StartOffsetBytes == Other.StartOffsetBytes
			&& NumElements == Other.NumElements;
	}

	FORCEINLINE bool operator!=(const FRHIBufferSRVCreateInfo& Other)const
	{
		return !(*this == Other);
	}

	friend uint32 GetTypeHash(const FRHIBufferSRVCreateInfo& Desc)
	{
		return HashCombine(
			HashCombine(GetTypeHash(Desc.Format), GetTypeHash(Desc.StartOffsetBytes)),
			GetTypeHash(Desc.NumElements)
		);
	}

	/** Encoding format for the element. */
	EPixelFormat Format = PF_Unknown;

	/** Offset in bytes from the beginning of buffer */
	uint32 StartOffsetBytes = 0;

	/** Number of elements (whole buffer by default) */
	uint32 NumElements = UINT32_MAX;
};

struct FRHIBufferUAVCreateInfo
{
	FRHIBufferUAVCreateInfo() = default;

	explicit FRHIBufferUAVCreateInfo(EPixelFormat InFormat)
		: Format(InFormat)
	{}

	FORCEINLINE bool operator==(const FRHIBufferUAVCreateInfo& Other)const
	{
		return Format == Other.Format && bSupportsAtomicCounter == Other.bSupportsAtomicCounter && bSupportsAppendBuffer == Other.bSupportsAppendBuffer;
	}

	FORCEINLINE bool operator!=(const FRHIBufferUAVCreateInfo& Other)const
	{
		return !(*this == Other);
	}

	friend uint32 GetTypeHash(const FRHIBufferUAVCreateInfo& Info)
	{
		return uint32(Info.Format) | uint32(Info.bSupportsAtomicCounter) << 8 | uint32(Info.bSupportsAppendBuffer) << 16;
	}

	/** Number of bytes per element (used for typed buffers). */
	EPixelFormat Format = PF_Unknown;

	/** Whether the uav supports atomic counter or append buffer operations (used for structured buffers) */
	bool bSupportsAtomicCounter = false;
	bool bSupportsAppendBuffer = false;
};


/*
class FRHIBufferViewCache
{
public:
	// Finds a UAV matching the descriptor in the cache or creates a new one and updates the cache.
	RHI_API FRHIUnorderedAccessView* GetOrCreateUAV(FRHICommandListBase& RHICmdList, FRHIBuffer* Buffer, const FRHIBufferUAVCreateInfo& CreateInfo);

	// Finds a SRV matching the descriptor in the cache or creates a new one and updates the cache.
	RHI_API FRHIShaderResourceView* GetOrCreateSRV(FRHICommandListBase& RHICmdList, FRHIBuffer* Buffer, const FRHIBufferSRVCreateInfo& CreateInfo);

	UE_DEPRECATED(5.3, "GetOrCreateUAV now requires a command list.")
	RHI_API FRHIUnorderedAccessView* GetOrCreateUAV(FRHIBuffer* Buffer, const FRHIBufferUAVCreateInfo& CreateInfo);
	UE_DEPRECATED(5.3, "GetOrCreateSRV now requires a command list.")
	RHI_API FRHIShaderResourceView* GetOrCreateSRV(FRHIBuffer* Buffer, const FRHIBufferSRVCreateInfo& CreateInfo);

	// Sets the debug name of the RHI view resources.
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	RHI_API void SetDebugName(const TCHAR* DebugName);
#else
	void SetDebugName(const TCHAR* DebugName) {}
#endif

private:
	TArray<TPair<FRHIBufferUAVCreateInfo, FUnorderedAccessViewRHIRef>, TInlineAllocator<1>> UAVs;
	TArray<TPair<FRHIBufferSRVCreateInfo, FShaderResourceViewRHIRef>, TInlineAllocator<1>> SRVs;
};
*/