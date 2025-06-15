#pragma once
#include "HeaderCollection.h"

#include "RHIAccess.h"
#include "RHIDefinition.h"

extern uint64 GRHITransitionPrivateData_SizeInBytes;
extern uint64 GRHITransitionPrivateData_AlignInBytes;

struct FRHISubresourceRange
{
	static const uint32 kDepthPlaneSlice = 0;
	static const uint32 kStencilPlaneSlice = 1;
	static const uint32 kAllSubresources = MAXINT32;

	uint32 MipIndex = kAllSubresources;
	uint32 ArraySlice = kAllSubresources;
	uint32 PlaneSlice = kAllSubresources;

	FRHISubresourceRange() = default;

	FRHISubresourceRange(
		uint32 InMipIndex,
		uint32 InArraySlice,
		uint32 InPlaneSlice)
		: MipIndex(InMipIndex)
		, ArraySlice(InArraySlice)
		, PlaneSlice(InPlaneSlice)
	{}

	inline bool IsAllMips() const
	{
		return MipIndex == kAllSubresources;
	}

	inline bool IsAllArraySlices() const
	{
		return ArraySlice == kAllSubresources;
	}

	inline bool IsAllPlaneSlices() const
	{
		return PlaneSlice == kAllSubresources;
	}

	inline bool IsWholeResource() const
	{
		return IsAllMips() && IsAllArraySlices() && IsAllPlaneSlices();
	}

	inline bool IgnoreDepthPlane() const
	{
		return PlaneSlice == kStencilPlaneSlice;
	}

	inline bool IgnoreStencilPlane() const
	{
		return PlaneSlice == kDepthPlaneSlice;
	}

	inline bool operator == (FRHISubresourceRange const& RHS) const
	{
		return MipIndex == RHS.MipIndex
			&& ArraySlice == RHS.ArraySlice
			&& PlaneSlice == RHS.PlaneSlice;
	}

	inline bool operator != (FRHISubresourceRange const& RHS) const
	{
		return !(*this == RHS);
	}
};

struct FRHITransitionInfo : public FRHISubresourceRange
{
	union
	{
		class FRHIResource* Resource = nullptr;
		class FRHIViewableResource* ViewableResource;
		class FRHITexture* Texture;
		class FRHIBuffer* Buffer;
		class FRHIUnorderedAccessView* UAV;
		//class FRHIRayTracingAccelerationStructure* BVH;
	};

	enum class EType : uint8
	{
		Unknown,
		Texture,
		Buffer,
		UAV,
		BVH,
	} Type = EType::Unknown;

	ERHIAccess AccessBefore = ERHIAccess::Unknown;
	ERHIAccess AccessAfter = ERHIAccess::Unknown;
	EResourceTransitionFlags Flags = EResourceTransitionFlags::None;

	FRHITransitionInfo() = default;

	FRHITransitionInfo(
		class FRHITexture* InTexture,
		ERHIAccess InPreviousState,
		ERHIAccess InNewState,
		EResourceTransitionFlags InFlags = EResourceTransitionFlags::None,
		uint32 InMipIndex = kAllSubresources,
		uint32 InArraySlice = kAllSubresources,
		uint32 InPlaneSlice = kAllSubresources)
		: FRHISubresourceRange(InMipIndex, InArraySlice, InPlaneSlice)
		, Texture(InTexture)
		, Type(EType::Texture)
		, AccessBefore(InPreviousState)
		, AccessAfter(InNewState)
		, Flags(InFlags)
	{}

	FRHITransitionInfo(class FRHIUnorderedAccessView* InUAV, ERHIAccess InPreviousState, ERHIAccess InNewState, EResourceTransitionFlags InFlags = EResourceTransitionFlags::None)
		: UAV(InUAV)
		, Type(EType::UAV)
		, AccessBefore(InPreviousState)
		, AccessAfter(InNewState)
		, Flags(InFlags)
	{}

	FRHITransitionInfo(class FRHIBuffer* InRHIBuffer, ERHIAccess InPreviousState, ERHIAccess InNewState, EResourceTransitionFlags InFlags = EResourceTransitionFlags::None)
		: Buffer(InRHIBuffer)
		, Type(EType::Buffer)
		, AccessBefore(InPreviousState)
		, AccessAfter(InNewState)
		, Flags(InFlags)
	{}
	/*
	FRHITransitionInfo(class FRHIRayTracingAccelerationStructure* InBVH, ERHIAccess InPreviousState, ERHIAccess InNewState, EResourceTransitionFlags InFlags = EResourceTransitionFlags::None)
		: BVH(InBVH)
		, Type(EType::BVH)
		, AccessBefore(InPreviousState)
		, AccessAfter(InNewState)
		, Flags(InFlags)
	{}*/

	FRHITransitionInfo(class FRHITexture* InTexture, ERHIAccess InNewState)
		: Texture(InTexture)
		, Type(EType::Texture)
		, AccessAfter(InNewState)
	{}

	FRHITransitionInfo(class FRHIUnorderedAccessView* InUAV, ERHIAccess InNewState)
		: UAV(InUAV)
		, Type(EType::UAV)
		, AccessAfter(InNewState)
	{}

	FRHITransitionInfo(class FRHIBuffer* InRHIBuffer, ERHIAccess InNewState)
		: Buffer(InRHIBuffer)
		, Type(EType::Buffer)
		, AccessAfter(InNewState)
	{}

	inline bool operator == (FRHITransitionInfo const& RHS) const
	{
		return Resource == RHS.Resource
			&& Type == RHS.Type
			&& AccessBefore == RHS.AccessBefore
			&& AccessAfter == RHS.AccessAfter
			&& Flags == RHS.Flags
			&& FRHISubresourceRange::operator==(RHS);
	}

	inline bool operator != (FRHITransitionInfo const& RHS) const
	{
		return !(*this == RHS);
	}
};
