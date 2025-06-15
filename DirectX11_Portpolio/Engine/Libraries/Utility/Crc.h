#pragma once
#include "HeaderCollection.h"
#include <cctype>

template <typename... Types>
struct TTuple;

typedef char				ANSICHAR;
typedef wchar_t				WIDECHAR;
enum UTF8CHAR : unsigned char {};

/** 
 * CRC hash generation for different types of input data
 **/
struct FCrc
{
	/** lookup table with precalculated CRC values - slicing by 8 implementation */
	static uint32 CRCTablesSB8[8][256];

	/** initializes the CRC lookup table. Must be called before any of the
		CRC functions are used. */
	static void Init();

	/** generates CRC hash of the memory area */
	typedef uint32 (*MemCrc32Functor)( const void* Data, int32 Length, uint32 CRC );
	static MemCrc32Functor MemCrc32Func;
	static FORCEINLINE uint32 MemCrc32(const void* Data, int32 Length, uint32 CRC = 0)
	{
		return MemCrc32Func(Data, Length, CRC);
	}

	/** generates CRC hash of the element */
	template <typename T>
	static uint32 TypeCrc32( const T& Data, uint32 CRC=0 )
	{
		return MemCrc32(&Data, sizeof(T), CRC);
	}

	/** String CRC. */
	template <typename CharType>
	static typename std::enable_if<sizeof(CharType) != 1, uint32>::Type StrCrc32(const CharType* Data, uint32 CRC = 0)
	{
		// We ensure that we never try to do a StrCrc32 with a CharType of more than 4 bytes.  This is because
		// we always want to treat every CRC as if it was based on 4 byte chars, even if it's less, because we
		// want consistency between equivalent strings with different character types.
		static_assert(sizeof(CharType) <= 4, "StrCrc32 only works with CharType up to 32 bits.");

		CRC = ~CRC;
		while (CharType Ch = *Data++)
		{
			CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC ^ Ch) & 0xFF];
			Ch >>= 8;
			CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC ^ Ch) & 0xFF];
			Ch >>= 8;
			CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC ^ Ch) & 0xFF];
			Ch >>= 8;
			CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC ^ Ch) & 0xFF];
		}
		return ~CRC;
	}

	template <typename CharType>
	static typename std::enable_if<sizeof(CharType) == 1, uint32>::Type StrCrc32(const CharType* Data, uint32 CRC = 0)
	{
		/* Overload for when CharType is a byte, which causes warnings when right-shifting by 8 */
		CRC = ~CRC;
		while (CharType Ch = *Data++)
		{
			CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC ^ Ch) & 0xFF];
			CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC     ) & 0xFF];
			CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC     ) & 0xFF];
			CRC = (CRC >> 8) ^ CRCTablesSB8[0][(CRC     ) & 0xFF];
		}
		return ~CRC;
	}

	/**
	 * DEPRECATED
	 * These tables and functions are deprecated because they're using tables and implementations
	 * which give values different from what a user of a typical CRC32 algorithm might expect.
	 */

	/** lookup table with precalculated CRC values */
	static uint32 CRCTable_DEPRECATED[256];
	/** lookup table with precalculated CRC values - slicing by 8 implementation */
	static uint32 CRCTablesSB8_DEPRECATED[8][256];

	/** String CRC. */
	template <typename CharType>
	static inline uint32 StrCrc_DEPRECATED(const CharType* Data)
	{
		// make sure table is initialized
		Check(CRCTable_DEPRECATED[1] != 0);

		uint32 CRC = 0xFFFFFFFF;
		while (*Data)
		{
			CharType C = *Data++;
			int32 CL = (C&255);
			CRC = (CRC << 8) ^ CRCTable_DEPRECATED[(CRC >> 24) ^ CL];
			int32 CH = (C>>8)&255;
			CRC = (CRC << 8) ^ CRCTable_DEPRECATED[(CRC >> 24) ^ CH];
		}
		return ~CRC;
	}

	/** String CRC. */
	template <typename CharType>
	static inline uint32 StrCrc_DEPRECATED(const int32 DataLen, const CharType* Data)
	{
		// make sure table is initialized
		Check(CRCTable_DEPRECATED[1] != 0);

		uint32 CRC = 0xFFFFFFFF;
		for (int32 i = 0; i < DataLen; i++)
		{
			CharType C = *Data++;
			int32 CL = (C&255);
			CRC = (CRC << 8) ^ CRCTable_DEPRECATED[(CRC >> 24) ^ CL];
			int32 CH = (C>>8)&255;
			CRC = (CRC << 8) ^ CRCTable_DEPRECATED[(CRC >> 24) ^ CH];
		}
		return ~CRC;
	}

	/** Case insensitive string hash function. */
	template <typename CharType> static inline uint32 Strihash_DEPRECATED( const CharType* Data );
	template <typename CharType> static inline uint32 Strihash_DEPRECATED( const int32 DataLen, const CharType* Data );

	/** generates CRC hash of the memory area */
	static uint32 MemCrc_DEPRECATED( const void* Data, int32 Length, uint32 CRC=0 );
};

template <>
inline uint32 FCrc::Strihash_DEPRECATED(const ANSICHAR* Data)
{
	// make sure table is initialized
	Check(CRCTable_DEPRECATED[1] != 0);

	uint32 Hash=0;
	while( *Data )
	{
		ANSICHAR Ch = static_cast<ANSICHAR>(std::toupper(static_cast<unsigned char>(*Data++)));
		uint8 B  = Ch;
		Hash = ((Hash >> 8) & 0x00FFFFFF) ^ CRCTable_DEPRECATED[(Hash ^ B) & 0x000000FF];
	}
	return Hash;
}

template <>
inline uint32 FCrc::Strihash_DEPRECATED(const int32 DataLen, const ANSICHAR* Data)
{
	// make sure table is initialized
	Check(CRCTable_DEPRECATED[1] != 0);

	uint32 Hash=0;
	for (int32 Idx = 0; Idx < DataLen; ++Idx)
	{
		ANSICHAR Ch = static_cast<ANSICHAR>(std::toupper(static_cast<unsigned char>(*Data++)));
		uint8 B  = Ch;
		Hash = ((Hash >> 8) & 0x00FFFFFF) ^ CRCTable_DEPRECATED[(Hash ^ B) & 0x000000FF];
	}
	return Hash;
}

template <>
inline uint32 FCrc::Strihash_DEPRECATED(const WIDECHAR* Data)
{
	// make sure table is initialized
	Check(CRCTable_DEPRECATED[1] != 0);

	uint32 Hash=0;
	while( *Data )
	{
		WIDECHAR Ch = static_cast<WIDECHAR>(std::toupper(static_cast<unsigned char>(*Data++)));
		uint16  B  = Ch;
		Hash     = ((Hash >> 8) & 0x00FFFFFF) ^ CRCTable_DEPRECATED[(Hash ^ B) & 0x000000FF];
		B        = Ch>>8;
		Hash     = ((Hash >> 8) & 0x00FFFFFF) ^ CRCTable_DEPRECATED[(Hash ^ B) & 0x000000FF];
	}
	return Hash;
}

template <>
inline uint32 FCrc::Strihash_DEPRECATED(const int32 DataLen, const WIDECHAR* Data)
{
	// make sure table is initialized
	Check(CRCTable_DEPRECATED[1] != 0);

	uint32 Hash=0;
	for (int32 Idx = 0; Idx < DataLen; ++Idx)
	{
		WIDECHAR Ch = static_cast<WIDECHAR>(std::toupper(static_cast<unsigned char>(*Data++)));
		uint16  B  = Ch;
		Hash     = ((Hash >> 8) & 0x00FFFFFF) ^ CRCTable_DEPRECATED[(Hash ^ B) & 0x000000FF];
		B        = Ch>>8;
		Hash     = ((Hash >> 8) & 0x00FFFFFF) ^ CRCTable_DEPRECATED[(Hash ^ B) & 0x000000FF];
	}
	return Hash;
}

template <>
inline uint32 FCrc::Strihash_DEPRECATED(const UTF8CHAR* Data)
{
	// We can't utilize StringConv.h here due to circular includes, so do the conversion manually

	int32 Len = static_cast<int32>(strlen(reinterpret_cast<const char*>(Data))); // UTF-8 길이

	int32 ConvertedLen = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(Data), Len, nullptr, 0);  
	WIDECHAR* Temp = new WIDECHAR[ConvertedLen + 1]; // 널 문자 고려

	int32 ResultLen = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(Data), Len, Temp, ConvertedLen);
	Temp[ResultLen] = L'\0'; // 명시적 null-terminate
	
	//Checkf(TempEnd, TEXT("String conversion unsuccessful"));

	// This doesn't work for strings containing characters outside the BMP, but
	// then neither does the WIDECHAR overload.
	uint32 Result = FCrc::Strihash_DEPRECATED(ConvertedLen, Temp);

	delete [] Temp;

	return Result;
}

template <>
inline uint32 FCrc::Strihash_DEPRECATED(const int32 DataLen, const UTF8CHAR* Data)
{
	// We can't utilize StringConv.h here due to circular includes, so do the conversion manually

	int32 ConvertedLen = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(Data), DataLen, nullptr, 0);

	WIDECHAR* Temp = new WIDECHAR[ConvertedLen + 1]; // null-terminate 고려

	int32 WrittenLen = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(Data), DataLen, Temp, ConvertedLen);
	Temp[WrittenLen] = L'\0'; // 명시적 null-termination

	WIDECHAR* TempEnd = Temp + WrittenLen;
	

	// This doesn't work for strings containing characters outside the BMP, but
	// then neither does the WIDECHAR overload.
	uint32 Result = FCrc::Strihash_DEPRECATED(ConvertedLen, Temp);

	delete [] Temp;

	return Result;
}

/**
 * Gets a non-owning TCHAR pointer from a string type.
 *
 * Can be used generically to get a const TCHAR*, when it is not known if the argument is a TCHAR* or an FString:
 *
 * template <typename T>
 * void LogValue(const T& Val)
 * {
 *     Logf(TEXT("Value: %s"), ToCStr(LexToString(Val)));
 * }
 */
FORCEINLINE const TCHAR* ToCStr(const TCHAR* Ptr)
{
	return Ptr;
}

/**
 * An implementation of KeyFuncs for sets which hashes string pointers by FCrc::Strihash_DEPRECATED.
 */
template <typename InKeyType, bool bInAllowDuplicateKeys = false>
struct TStringPointerSetKeyFuncs_DEPRECATED
{

	using KeyType         = InKeyType;
	using KeyInitType     = const InKeyType*;
	using ElementInitType = const InKeyType&;

	static constexpr bool bAllowDuplicateKeys = bInAllowDuplicateKeys;

	static FORCEINLINE KeyInitType GetSetKey(ElementInitType Element)
	{
		return Element;
	}

	template <typename ComparableKey>
	static FORCEINLINE bool Matches(KeyInitType A, const ComparableKey& B)
	{
		return A == B;
	}

	template <typename ComparableKey = KeyInitType>
	static FORCEINLINE uint32 GetKeyHash(ComparableKey Key)
	{
		return FCrc::Strihash_DEPRECATED(ToCStr(Key));
	}
};

/**
 * An implementation of KeyFuncs for maps which hashes string pointers by FCrc::Strihash_DEPRECATED.
 */
template <typename InKeyType, typename InValueType, bool bInAllowDuplicateKeys = false>
struct TStringPointerMapKeyFuncs_DEPRECATED
{

	using KeyType         = InKeyType;
	using KeyInitType     = const InKeyType*;
	using ElementInitType = const std::pair<const InKeyType&, const InValueType&>&;

	static constexpr bool bAllowDuplicateKeys = bInAllowDuplicateKeys;

	static FORCEINLINE KeyInitType GetSetKey(ElementInitType Element)
	{
		return Element.Key;
	}

	template <typename ComparableKey>
	static FORCEINLINE bool Matches(KeyInitType A, const ComparableKey& B)
	{
		return A == B;
	}

	template <typename ComparableKey = KeyInitType>
	static FORCEINLINE uint32 GetKeyHash(ComparableKey Key)
	{
		return FCrc::Strihash_DEPRECATED(ToCStr(Key));
	}
};