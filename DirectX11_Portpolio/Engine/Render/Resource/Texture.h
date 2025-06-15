#pragma once


class Texture
{
public:
    Texture(wstring InFileName, TexMetadata& InLoadInfo,  
        bool bUseSRGB, bool bIsCubeMap, bool bDefaultPath = true);
    ~Texture();

public:
    UINT GetWidth() { return Metadata.width; }
    UINT GetHeight() { return Metadata.height; }

    ID3D11ShaderResourceView* GetSRV() { return SRV.Get(); }

private:
    void LoadMetadata(TexMetadata& InLoadInfo);
    void LoadTexture(bool InbIsCubeMap, bool bUseSRGB);
    void ReadImage(vector<uint8_t>& image, int& width, int& height);

private:
    void CreateDDSTexture(bool InbIsCubeMap);
    void CreateTexture(bool bUseSRGB);

    ComPtr<ID3D11Texture2D> CreateStagingTexture(
        const int width, const int height, vector<uint8_t>& image,
        const DXGI_FORMAT pixelFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
        const int mipLevels = 1, const int arraySize = 1);

public:
    //D3D11_TEXTURE2D_DESC ReadPixel(vector<Color>& OutPixel);
    //D3D11_TEXTURE2D_DESC ReadPixel(DXGI_FORMAT InFormat, vector<Color>& OutPixel);
    //D3D11_TEXTURE2D_DESC ReadPixel(ID3D11Texture2D* InSource, DXGI_FORMAT InFormat, vector<Color>& OutPixel);

private:
    ComPtr<ID3D11ShaderResourceView> SRV = nullptr;
    ComPtr<ID3D11Texture2D> texture = nullptr;
    

    TexMetadata Metadata;

private:
    wstring FileName;
};