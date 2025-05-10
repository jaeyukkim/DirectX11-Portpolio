#pragma once


class Texture
{
public:
    Texture(wstring InFileName, TexMetadata& InLoadInfo, bool bDefaultPath = true);
    ~Texture();

public:
    UINT GetWidth() { return Metadata.width; }
    UINT GetHeight() { return Metadata.height; }

    ID3D11ShaderResourceView* GetSRV() { return SRV.Get(); }

private:
    void LoadMetadata(TexMetadata& InLoadInfo);
    void LoadTexture();

public:
    //D3D11_TEXTURE2D_DESC ReadPixel(vector<Color>& OutPixel);
    //D3D11_TEXTURE2D_DESC ReadPixel(DXGI_FORMAT InFormat, vector<Color>& OutPixel);
    //D3D11_TEXTURE2D_DESC ReadPixel(ID3D11Texture2D* InSource, DXGI_FORMAT InFormat, vector<Color>& OutPixel);

private:
    ComPtr<ID3D11ShaderResourceView> SRV = nullptr;
    TexMetadata Metadata;

private:
    wstring FileName;
};