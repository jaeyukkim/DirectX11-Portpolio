#pragma once
#include "HeaderCollection.h"



class FString;

enum class ERHIAccess : uint32
{
	Unknown = 0,

	CPURead             	= 1 <<  0,
	Present             	= 1 <<  1,
	IndirectArgs        	= 1 <<  2,
	VertexOrIndexBuffer 	= 1 <<  3,
	SRVCompute          	= 1 <<  4,
	SRVGraphics         	= 1 <<  5,
	CopySrc             	= 1 <<  6,
	ResolveSrc          	= 1 <<  7,
	DSVRead					= 1 <<  8,
	
	UAVCompute          	= 1 <<  9,
	UAVGraphics         	= 1 << 10,
	RTV                 	= 1 << 11,
	CopyDest            	= 1 << 12,
	ResolveDst          	= 1 << 13,
	DSVWrite            	= 1 << 14,


	BVHRead                  = 1 << 15,
	BVHWrite                 = 1 << 16,
	
	Discard					= 1 << 17,
	
	ShadingRateSource	= 1 << 18,

	Last = ShadingRateSource,
	None = Unknown,
	Mask = (Last << 1) - 1,

	// A mask of the two possible SRV states
	SRVMask = SRVCompute | SRVGraphics,

	// A mask of the two possible UAV states
	UAVMask = UAVCompute | UAVGraphics,

	// A mask of all bits representing read-only states which cannot be combined with other write states.
	ReadOnlyExclusiveMask = CPURead | Present | IndirectArgs | VertexOrIndexBuffer | SRVGraphics | SRVCompute | CopySrc | ResolveSrc | BVHRead | ShadingRateSource,

	// A mask of all bits representing read-only states on the compute pipe which cannot be combined with other write states.
	ReadOnlyExclusiveComputeMask = CPURead | IndirectArgs | SRVCompute | CopySrc | BVHRead,

	// A mask of all bits representing read-only states which may be combined with other write states.
	ReadOnlyMask = ReadOnlyExclusiveMask | DSVRead | ShadingRateSource,

	// A mask of all bits representing readable states which may also include writable states.
	ReadableMask = ReadOnlyMask | UAVMask,

	// A mask of all bits representing write-only states which cannot be combined with other read states.
	WriteOnlyExclusiveMask = RTV | CopyDest | ResolveDst,

	// A mask of all bits representing write-only states which may be combined with other read states.
	WriteOnlyMask = WriteOnlyExclusiveMask | DSVWrite,

	// A mask of all bits representing writable states which may also include readable states.
	WritableMask = WriteOnlyMask | UAVMask | BVHWrite
};
ENUM_CLASS_FLAGS(ERHIAccess)

inline constexpr bool IsReadOnlyExclusiveAccess(ERHIAccess Access)
{
	return EnumHasAnyFlags(Access, ERHIAccess::ReadOnlyExclusiveMask) && !EnumHasAnyFlags(Access, ~ERHIAccess::ReadOnlyExclusiveMask);
}

inline constexpr bool IsReadOnlyAccess(ERHIAccess Access)
{
	return EnumHasAnyFlags(Access, ERHIAccess::ReadOnlyMask) && !EnumHasAnyFlags(Access, ~ERHIAccess::ReadOnlyMask);
}

inline constexpr bool IsWriteOnlyAccess(ERHIAccess Access)
{
	return EnumHasAnyFlags(Access, ERHIAccess::WriteOnlyMask) && !EnumHasAnyFlags(Access, ~ERHIAccess::WriteOnlyMask);
}

inline constexpr bool IsWritableAccess(ERHIAccess Access)
{
	return EnumHasAnyFlags(Access, ERHIAccess::WritableMask);
}

inline constexpr bool IsReadableAccess(ERHIAccess Access)
{
	return EnumHasAnyFlags(Access, ERHIAccess::ReadableMask);
}

inline constexpr bool IsInvalidAccess(ERHIAccess Access)
{
	return
		((EnumHasAnyFlags(Access, ERHIAccess::ReadOnlyExclusiveMask) && EnumHasAnyFlags(Access, ERHIAccess::WritableMask)) ||
			(EnumHasAnyFlags(Access, ERHIAccess::WriteOnlyExclusiveMask) && EnumHasAnyFlags(Access, ERHIAccess::ReadableMask)));
}

inline constexpr bool IsValidAccess(ERHIAccess Access)
{
	return !IsInvalidAccess(Access);
}

/** Mask of states which are allowed to be considered for state merging. */
extern ERHIAccess GRHIMergeableAccessMask;

/** Mask of states which are allowed to be considered for multi-pipeline state merging. This should be a subset of GRHIMergeableAccessMask. */
extern ERHIAccess GRHIMultiPipelineMergeableAccessMask;