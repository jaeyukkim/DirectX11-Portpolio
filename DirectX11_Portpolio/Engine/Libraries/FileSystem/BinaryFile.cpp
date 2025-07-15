#include "HeaderCollection.h"
#include "BinaryFile.h"

BinaryWriter::BinaryWriter()
{
	
}

BinaryWriter::~BinaryWriter()
{
	Close();
}

void BinaryWriter::Open(wstring InFilePath, UINT InOption)
{
	assert(InFilePath.length() > 0);

	FileHandle = CreateFile
	(
		InFilePath.c_str(),
		GENERIC_WRITE,
		0,
		nullptr,
		InOption,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	assert(FileHandle != INVALID_HANDLE_VALUE);
}

void BinaryWriter::Close()
{
	CheckNull(FileHandle);

	CloseHandle(FileHandle);
	FileHandle = nullptr;
}

void BinaryWriter::ToInt(int InData)
{
	DWORD size = 0;
	WriteFile(FileHandle, &InData, sizeof(int), &size, nullptr);
}

void BinaryWriter::ToUInt(UINT InData)
{
	DWORD size = 0;
	WriteFile(FileHandle, &InData, sizeof(UINT), &size, nullptr);
}

void BinaryWriter::ToFloat(float InData)
{
	DWORD size = 0;
	WriteFile(FileHandle, &InData, sizeof(float), &size, nullptr);
}

void BinaryWriter::ToMatrix(Matrix& InData)
{
	DWORD size = 0;

	const float* m = reinterpret_cast<const float*>(&InData);
	WriteFile(FileHandle, m, sizeof(Matrix), &size, nullptr);
	
	assert(sizeof(Matrix) == size);
}

void BinaryWriter::ToString(const string& InData)
{
	ToUInt(InData.size());


	DWORD size = 0;

	const char* str = InData.c_str();
	WriteFile(FileHandle, str, InData.size(), &size, nullptr);
	
	assert(InData.size() == size);
}

void BinaryWriter::ToByte(void* InData, UINT InDataSize)
{
	DWORD size = 0;

	WriteFile(FileHandle, InData, InDataSize, &size, nullptr);
	assert(InDataSize == size);
}

//-----------------------------------------------------------------------------

BinaryReader::BinaryReader()
{

}

BinaryReader::~BinaryReader()
{
	Close();
}

void BinaryReader::Open(wstring InFilePath)
{
	assert(InFilePath.length() > 0);

	FileHandle = CreateFile
	(
		InFilePath.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	assert(FileHandle != INVALID_HANDLE_VALUE);
}

void BinaryReader::Close()
{
	CheckNull(FileHandle);

	CloseHandle(FileHandle);
	FileHandle = nullptr;
}

int BinaryReader::FromInt()
{
	int temp = 0;
	
	DWORD size = 0;
	ReadFile(FileHandle, &temp, sizeof(int), &size, nullptr);

	return temp;
}

UINT BinaryReader::FromUInt()
{
	UINT temp = 0;

	DWORD size = 0;
	ReadFile(FileHandle, &temp, sizeof(UINT), &size, nullptr);

	return temp;
}

float BinaryReader::FromFloat()
{
	float temp = 0.0f;

	DWORD size = 0;
	ReadFile(FileHandle, &temp, sizeof(float), &size, nullptr);

	return temp;
}

Matrix BinaryReader::FromMatrix()
{
	float temp[16];

	DWORD size = 0;
	ReadFile(FileHandle, temp, sizeof(Matrix), &size, nullptr);
	assert(size == sizeof(Matrix));

	Matrix result;
	memcpy(&result, temp, sizeof(Matrix));	

	return result;
}

string BinaryReader::FromString()
{
	UINT length = FromUInt();
	unique_ptr<char[]> temp(new char[length + 1]);

	DWORD size = 0;
	ReadFile(FileHandle, temp.get(), sizeof(char) * length, &size, nullptr);
	assert(size == (sizeof(char) * length));

	temp[length] = '\0';

	string result(temp.get());
	return result;
}

void BinaryReader::FromByte(void* OutData, UINT InDataSize) const
{
	DWORD size = 0;
	ReadFile(FileHandle, OutData, InDataSize, &size, nullptr);
	assert(size == InDataSize);
}
