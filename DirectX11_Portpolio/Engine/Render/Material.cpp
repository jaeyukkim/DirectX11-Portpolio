#include "HeaderCollection.h"
#include "Material.h"


Material::Material()
{
	for (int i = 0; i < static_cast<int>(MaterialMapType::MAX_TEXTURE_COUNT); i++)
	{
		Textures[i] = nullptr;		//// 텍스처 포인터 초기화
		SRVs[i] = nullptr;		// Shader Resource View 초기화
	}
	
	ColorConstantBuffer = make_shared<ConstantBuffer>(&MaterialDesc, sizeof(MaterialDescription));
}


void Material::BindMaterial()
{
	GetConstantBuffer()->UpdateConstBuffer();
	GetConstantBuffer()->PSSetConstantBuffer(EConstBufferSlot::MaterialDesc, 1);

	if (!bCubeMap)
	{
		D3D::Get()->GetDeviceContext()->PSSetShaderResources(static_cast<UINT>(EShaderResourceSlot::MaterialTexture),
			static_cast<int>(MaterialMapType::MAX_TEXTURE_COUNT), SRVs->GetAddressOf());
	}
	else
	{
		D3D::Get()->GetDeviceContext()->PSSetShaderResources(static_cast<UINT>(EShaderResourceSlot::CubeMapTexture),
			static_cast<int>(CubeMapType::MAX_CUBEMAP_TEXTURE_COUNT), SRVs->GetAddressOf());
	}
}

void Material::SetSamplerDesc(ESamplerSlot InsampDesc)
{
	SampDesc = InsampDesc;
}

void Material::SetTextureMap(wstring InFilePath, MaterialMapType InMaterialMapType)
{
	int matType = static_cast<int>(InMaterialMapType);
	TexMetadata mataData;
	Textures[matType] = make_shared<Texture>(InFilePath, mataData, 
		CheckingMaterialMap(InMaterialMapType), bCubeMap, false);

	SRVs[matType] = Textures[matType]->GetSRV();
}
bool Material::CheckingMaterialMap(MaterialMapType InMaterialMapType)
{
	bool bUseSRGB = false;
	switch (InMaterialMapType)
	{
	case MaterialMapType::ALBEDO:
		MaterialDesc.bUseAlbedoMap = true;
		bUseSRGB = true;
		break;
	case MaterialMapType::NORMAL:
		MaterialDesc.bUseNormalMap = true;
		break;
	case MaterialMapType::AMBIENTOCCLUSION:
		MaterialDesc.bUseAOMap = true;
		break;
	case MaterialMapType::METALLIC:
		MaterialDesc.bUseMetallicMap = true;
		break;
	case MaterialMapType::ROUGHNESS:
		MaterialDesc.bUseRoughnessMap = true;
		break;
	case MaterialMapType::EMISSIVE:
		MaterialDesc.bUseEmissiveMap = true;
		bUseSRGB = true;
		break;
	}
	
	return bUseSRGB;
}

ID3D11ShaderResourceView* Material::GetSRV(MaterialMapType InMaterialMapType)
{
	int matType = static_cast<int>(InMaterialMapType);
	if(SRVs[matType] != nullptr)
		return SRVs[matType].Get();
	return nullptr;
}

ID3D11ShaderResourceView* Material::GetSkyMapSRV(CubeMapType InCubeMapType)
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


