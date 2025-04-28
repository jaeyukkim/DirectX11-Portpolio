#include "Pch.h"
#include "ACubeMap.h"
#include "Render/Material.h"
#include "Frameworks/UStaticMeshComponent.h"

ACubeMap::ACubeMap(wstring InTextureFileName, wstring InObjectFileName)
{
    Cube = CreateComponent<UStaticMeshComponent>(this, InObjectFileName);
    SetRootComponent(Cube.get());
    Cube->ReverseIndices();

    
    ScratchImage image;

    InTextureFileName = L"../Contents/_Texture/CubeMap/" + InTextureFileName;
    HRESULT hr = LoadFromDDSFile(InTextureFileName.c_str(), DDS_FLAGS_NONE, nullptr, image);
    AssertHR(hr, "텍스쳐 파일 찾을 수 없음");
    

    hr = CreateShaderResourceView(D3D::Get()->GetDevice(),
        image.GetImages(),
        image.GetImageCount(),
        image.GetMetadata(),
        SRV.GetAddressOf());
    
    AssertHR(hr, "ShaderResourceView 생성 실패");
}

void ACubeMap::Tick(float deltaTime)
{
    Actor::Tick(deltaTime);
}

void ACubeMap::Render()
{
    Actor::Render();

   
    Cube->RenderComponent(true);
    D3D::Get()->GetDeviceContext()->PSSetShaderResources(static_cast<UINT>(EShaderResourceSlot::CubeMapTexture), 1, SRV.GetAddressOf());
    Cube->DrawComponentIndex();
}

void ACubeMap::InitCubeMapTexture()
{
}
