#include "HeaderCollection.h"
#include "Material.h"


Material::Material()
{
	Initialize();

	ColorConstantBuffer = make_shared<ConstantBuffer>(&ColorData, sizeof(Colors));
	Renderer = make_shared<Shader>();
		
}

Material::~Material()
{

	
}



void Material::Render()
{
	
	GetConstantBuffer()->UpdateConstBuffer();
	GetConstantBuffer()->PSSetConstantBuffer(EConstBufferSlot::MaterialDesc, 1);

	if (!bCubeMap)
	{
		D3D::Get()->GetDeviceContext()->PSSetShaderResources(static_cast<UINT>(EShaderResourceSlot::MaterialTexture),
			MAX_MATERIAL_TEXTURE_COUNT, SRVs->GetAddressOf());
	}
	else
	{
		D3D::Get()->GetDeviceContext()->PSSetShaderResources(static_cast<UINT>(EShaderResourceSlot::CubeMapTexture),
			MAX_CUBEMAP_TEXTURE_COUNT, SRVs->GetAddressOf());
	}
	
	
}

void Material::Initialize()
{
	SampDesc = SamplerDescCollection::GetDefaultSamplerDesc();
	for (int i = 0; i < MAX_MATERIAL_TEXTURE_COUNT; i++)
	{
		Textures[i] = nullptr;		//// 텍스처 포인터 초기화
		SRVs[i] = nullptr;		// Shader Resource View 초기화
	}
}

void Material::SetSamplerDesc(const D3D11_SAMPLER_DESC& InsampDesc)
{
	SampDesc = InsampDesc;
}

void Material::SetAmbientMap(string InFilePath)
{
	// 문자열(string)을 wstring으로 변환 후 재호출
	SetAmbientMap(String::ToWString(InFilePath));
}

void Material::SetAmbientMap(wstring InFilePath)
{
	// 기존 텍스처가 있다면 삭제


	TexMetadata mataData;
	ZeroMemory(&mataData, sizeof(TexMetadata));

	// 새로운 텍스처를 생성
	Textures[MATERIAL_TEXTURE_AMBIENT] = make_shared<Texture>(InFilePath, mataData, false);

	// 텍스처의 Shader Resource View를 설정
	SRVs[MATERIAL_TEXTURE_AMBIENT] = Textures[MATERIAL_TEXTURE_AMBIENT]->GetSRV();
}


void Material::SetDiffuseMap(string InFilePath)
{
	// 문자열(string)을 wstring으로 변환 후 재호출
	SetDiffuseMap(String::ToWString(InFilePath));
}

void Material::SetDiffuseMap(wstring InFilePath)
{
	// 기존 텍스처가 있다면 삭제


	TexMetadata mataData;
	ZeroMemory(&mataData, sizeof(TexMetadata));

	// 새로운 텍스처를 생성
	Textures[MATERIAL_TEXTURE_DIFFUSE] = make_shared<Texture>(InFilePath, mataData, false);

	// 텍스처의 Shader Resource View를 설정
	SRVs[MATERIAL_TEXTURE_DIFFUSE] = Textures[MATERIAL_TEXTURE_DIFFUSE]->GetSRV();
}

void Material::SetSpecularMap(string InFilePath)
{
	SetSpecularMap(String::ToWString(InFilePath));
}

void Material::SetSpecularMap(wstring InFilePath)
{
	
	TexMetadata mataData;

	Textures[MATERIAL_TEXTURE_SPECULAR] = make_shared<Texture>(InFilePath, mataData, false);
	
	
	SRVs[MATERIAL_TEXTURE_SPECULAR] = Textures[MATERIAL_TEXTURE_SPECULAR]->GetSRV();
}

void Material::SetNormalMap(string InFilePath)
{
	SetNormalMap(String::ToWString(InFilePath));
}

void Material::SetNormalMap(wstring InFilePath)
{

	TexMetadata mataData;
	// 새로운 노멀맵 텍스처 생성
	Textures[MATERIAL_TEXTURE_NORMAL] = make_shared<Texture>(InFilePath, mataData, false);

	// Shader Resource View 설정
	SRVs[MATERIAL_TEXTURE_NORMAL] = Textures[MATERIAL_TEXTURE_NORMAL]->GetSRV();
}

ID3D11ShaderResourceView* Material::GetSRV(int materialType)
{
	if(SRVs[materialType] != nullptr)
		return SRVs[materialType].Get();
}

void Material::SetUVTiling(Vector2 InUV_Tiling)
{
	ColorData.UV_Tiling = InUV_Tiling;
}