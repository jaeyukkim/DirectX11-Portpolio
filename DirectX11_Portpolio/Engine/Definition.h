#pragma once

#define Check(x) { assert((x) >= (0)); }
#define Assert(x, message) { assert((x) && (message)); }
#define AssertHR(x, message) { assert((SUCCEEDED(x)) && (message)); }

#define Delete(x)\
{\
	if (x != nullptr)\
	{\
		delete (x);\
		(x) = nullptr;\
	}\
}

#define CheckTrue(x) { if(x == true) return; }
#define CheckTrueResult(x, y) { if(x == true) return y; }

#define CheckFalse(x) { if(x == false) return;}
#define CheckFalseResult(x, y) { if(x == false) return y;}

#define CheckNull(x) { if(x == nullptr) return; }
#define CheckNotNull(x) { if(x != nullptr) return; }

#define Super  __super

/* Numeric constants
 *****************************************************************************/

#define uint8 UINT8
#define uint16 UINT16
#define uint32 UINT32
#define uint64 UINT64

#define int8 INT8
#define int16 INT16
#define int32 INT32
#define int64 INT64

#define MIN_uint8		((UINT8)	0x00)
#define	MIN_uint16		((UINT16)	0x0000)
#define	MIN_uint32		((UINT32)	0x00000000)
#define MIN_uint64		((UINT64)	0x0000000000000000)
#define MIN_int8		((INT8)		-128)
#define MIN_int16		((INT16)	-32768)
#define MIN_int32		((INT32)	0x80000000)
#define MIN_int64		((INT64)	0x8000000000000000)

#define MAX_uint8		((UINT8)	0xff)
#define MAX_uint16		((UINT16)	0xffff)
#define MAX_uint32		((UINT32)	0xffffffff)
#define MAX_uint64		((UINT64)	0xffffffffffffffff)
#define MAX_int8		((INT8)		0x7f)
#define MAX_int16		((INT16)	0x7fff)
#define MAX_int32		((INT32)	0x7fffffff)
#define MAX_int64		((INT64)	0x7fffffffffffffff)

#define MIN_flt			(1.175494351e-38F)			/* min positive value */
#define MAX_flt			(3.402823466e+38F)
#define MIN_dbl			(2.2250738585072014e-308)	/* min positive value */
#define MAX_dbl			(1.7976931348623158e+308)



//********************************* Enum ????*****************************************

// Defines all bitwise operators for enum classes so it can be (mostly) used as a regular flags enum
#define ENUM_CLASS_FLAGS(Enum) \
inline           Enum& operator|=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
inline           Enum& operator&=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
inline           Enum& operator^=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
inline constexpr Enum  operator| (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
inline constexpr Enum  operator& (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
inline constexpr Enum  operator^ (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
inline constexpr bool  operator! (Enum  E)             { return !(__underlying_type(Enum))E; } \
inline constexpr Enum  operator~ (Enum  E)             { return (Enum)~(__underlying_type(Enum))E; }

// Friends all bitwise operators for enum classes so the definition can be kept private / protected.
#define FRIEND_ENUM_CLASS_FLAGS(Enum) \
friend           Enum& operator|=(Enum& Lhs, Enum Rhs); \
friend           Enum& operator&=(Enum& Lhs, Enum Rhs); \
friend           Enum& operator^=(Enum& Lhs, Enum Rhs); \
friend constexpr Enum  operator| (Enum  Lhs, Enum Rhs); \
friend constexpr Enum  operator& (Enum  Lhs, Enum Rhs); \
friend constexpr Enum  operator^ (Enum  Lhs, Enum Rhs); \
friend constexpr bool  operator! (Enum  E); \
friend constexpr Enum  operator~ (Enum  E);

template<typename Enum>
constexpr bool EnumHasAllFlags(Enum Flags, Enum Contains)
{
	using UnderlyingType = __underlying_type(Enum);
	return ((UnderlyingType)Flags & (UnderlyingType)Contains) == (UnderlyingType)Contains;
}

template<typename Enum>
constexpr bool EnumHasAnyFlags(Enum Flags, Enum Contains)
{
	using UnderlyingType = __underlying_type(Enum);
	return ((UnderlyingType)Flags & (UnderlyingType)Contains) != 0;
}

template<typename Enum>
void EnumAddFlags(Enum& Flags, Enum FlagsToAdd)
{
	using UnderlyingType = __underlying_type(Enum);
	Flags = (Enum)((UnderlyingType)Flags | (UnderlyingType)FlagsToAdd);
}

template<typename Enum>
void EnumRemoveFlags(Enum& Flags, Enum FlagsToRemove)
{
	using UnderlyingType = __underlying_type(Enum);
	Flags = (Enum)((UnderlyingType)Flags & ~(UnderlyingType)FlagsToRemove);
}

//* ------------------------Hash-----------------------------

inline uint32 HashCombine(uint32 A, uint32 C)
{
	uint32 B = 0x9e3779b9;
	A += B;

	A -= B; A -= C; A ^= (C>>13);
	B -= C; B -= A; B ^= (A<<8);
	C -= A; C -= B; C ^= (B>>13);
	A -= B; A -= C; A ^= (C>>12);
	B -= C; B -= A; B ^= (A<<16);
	C -= A; C -= B; C ^= (B>>5);
	A -= B; A -= C; A ^= (C>>3);
	B -= C; B -= A; B ^= (A<<10);
	C -= A; C -= B; C ^= (B>>15);

	return C;
}

template <
	typename ScalarType,
	std::enable_if_t<std::is_scalar_v<ScalarType> && !std::is_same_v<ScalarType, TCHAR*> && !std::is_same_v<ScalarType, const TCHAR*>>* = nullptr
>
inline uint32 GetTypeHash(ScalarType Value)
{
	if constexpr (std::is_integral_v<ScalarType>)
	{
		if constexpr (sizeof(ScalarType) <= 4)
		{
			return Value;
		}
		else if constexpr (sizeof(ScalarType) == 8)
		{
			return (uint32)Value + ((uint32)(Value >> 32) * 23);
		}
		else if constexpr (sizeof(ScalarType) == 16)
		{
			const uint64 Low = (uint64)Value;
			const uint64 High = (uint64)(Value >> 64);
			return GetTypeHash(Low) ^ GetTypeHash(High);
		}
		else
		{
			static_assert(sizeof(ScalarType) == 0, "Unsupported integral type");
		}
	}
	else if constexpr (std::is_floating_point_v<ScalarType>)
	{
		if constexpr (std::is_same_v<ScalarType, float>)
		{
			return *(uint32*)&Value;
		}
		else if constexpr (std::is_same_v<ScalarType, double>)
		{
			return GetTypeHash(*(uint64*)&Value);
		}
		else
		{
			static_assert(sizeof(ScalarType) == 0, "Unsupported floating point type");
		}
	}
	else if constexpr (std::is_enum_v<ScalarType>)
	{
		return GetTypeHash((__underlying_type(ScalarType))Value);
	}
	else if constexpr (std::is_pointer_v<ScalarType>)
	{
		// Once the TCHAR* deprecations below are removed, we want to prevent accidental string hashing, so this static_assert should be commented back in
		//static_assert(!TIsCharType<std::remove_pointer_t<ScalarType>>::Value, "Pointers to string types should use a PointerHash() or FCrc::Stricmp_DEPRECATED() call depending on requirements");

		return PointerHash(Value);
	}
	else
	{
		static_assert(sizeof(ScalarType) == 0, "Unsupported scalar type");
	}
}


