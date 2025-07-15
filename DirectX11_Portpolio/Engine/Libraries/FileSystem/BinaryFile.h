#pragma once


class BinaryWriter
{
public:
    BinaryWriter();
    ~BinaryWriter();

    void Open(wstring InFilePath, UINT InOption = CREATE_ALWAYS);
    void Close();

public:
    void ToInt(int InData);
    void ToUInt(UINT InData);
    void ToFloat(float InData);
    
    void ToMatrix(Matrix& InData);

    void ToString(const string& InData);
    void ToByte(void* InData, UINT InDataSize);

private:
    HANDLE FileHandle = nullptr;
};

//-----------------------------------------------------------------------------

class BinaryReader
{
public:
    BinaryReader();
    ~BinaryReader();

    void Open(wstring InFilePath);
    void Close();

public:
    int FromInt();
    UINT FromUInt();

    Matrix FromMatrix();
    string FromString();
	
    void FromByte(void* OutData, UINT InDataSize) const;

private:
    HANDLE FileHandle = nullptr;
};

