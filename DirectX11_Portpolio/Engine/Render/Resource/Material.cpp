#include "HeaderCollection.h"
#include "Material.h"


Material::Material()
{
	for (int i = 0; i < static_cast<int>(EMaterialMapType::MAX_TEXTURE_COUNT); i++)
	{
		Textures[i] = nullptr;		// 텍스처 포인터 초기화
		SRVs[i] = nullptr;		// Shader Resource View 초기화
	}
	
	ColorConstantBuffer = make_shared<ConstantBuffer>(&MaterialDesc, sizeof(MaterialDescription));
}


void Material::BindMaterial()
{
	GetConstantBuffer()->UpdateConstBuffer();
	GetConstantBuffer()->PSSetConstantBuffer(EConstBufferSlot::CB_MaterialDesc, 1);

	if (!bCubeMap)
	{
		D3D::Get()->GetDeviceContext()->PSSetShaderResources(static_cast<UINT>(EShaderResourceSlot::MaterialTexture),
			static_cast<int>(EMaterialMapType::MAX_TEXTURE_COUNT), SRVs->GetAddressOf());
	}
	else
	{
		D3D::Get()->GetDeviceContext()->PSSetShaderResources(static_cast<UINT>(EShaderResourceSlot::CubeMapTexture),
			static_cast<int>(ECubeMapType::MAX_CUBEMAP_TEXTURE_COUNT), SRVs->GetAddressOf());
	}
}


void Material::SetTextureMap(wstring InFilePath, EMaterialMapType InMaterialMapType)
{
	int matType = static_cast<int>(InMaterialMapType);
	TexMetadata mataData;
	Textures[matType] = make_shared<Texture>(InFilePath, mataData, 
		CheckingMaterialMap(InMaterialMapType), bCubeMap, false);

	SRVs[matType] = Textures[matType]->GetSRV();
}
bool Material::CheckingMaterialMap(EMaterialMapType InMaterialMapType)
{
	bool bUseSRGB = false;
	switch (InMaterialMapType)
	{
	case EMaterialMapType::ALBEDO:
		MaterialDesc.bUseAlbedoMap = true;
		bUseSRGB = true;
		break;
	case EMaterialMapType::NORMAL:
		MaterialDesc.bUseNormalMap = true;
		break;
	case EMaterialMapType::AMBIENTOCCLUSION:
		MaterialDesc.bUseAOMap = true;
		break;
	case EMaterialMapType::METALLIC:
		MaterialDesc.bUseMetallicMap = true;
		break;
	case EMaterialMapType::ROUGHNESS:
		MaterialDesc.bUseRoughnessMap = true;
		break;
	case EMaterialMapType::EMISSIVE:
		MaterialDesc.bUseEmissiveMap = true;
		bUseSRGB = true;
		break;
	}
	
	return bUseSRGB;
}

ID3D11ShaderResourceView* Material::GetSRV(EMaterialMapType InMaterialMapType)
{
	int matType = static_cast<int>(InMaterialMapType);
	if(SRVs[matType] != nullptr)
		return SRVs[matType].Get();
	return nullptr;
}

ID3D11ShaderResourceView* Material::GetSkyMapSRV(ECubeMapType InCubeMapType)
{
	int matType = static_cast<int>(InCubeMapType);
	if (SRVs[matType] != nullptr)
		return SRVs[matType].Get();
	return nullptr;
}

void Material::SetUVTiling(Vector2 InUV_Tiling)
{
	MaterialDesc.UV_Tiling = InUV_Tiling;
}


