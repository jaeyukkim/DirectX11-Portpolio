#include "HeaderCollection.h"
#include "Material.h"


Material::Material()
{
	Initialize();

	CBuffer = make_shared<ConstantBuffer>(&ColorData, sizeof(Colors));
	Renderer = make_shared<Shader>();
		
}

Material::~Material()
{

	
}


/*
void Material::SetShader(Shader* InDrawer)
{
	Drawer = InDrawer;
	
	sCBuffer = Drawer->AsConstantBuffer("CB_Material");
	sSRVs = Drawer->AsSRV("MaterialMaps");
}

void Material::SetShader(wstring InFileName)
{
	assert(InFileName.size() > 0);

	SetShader(new Shader(InFileName));
}
*/

void Material::Render()
{
	
	// 머티리얼의 상수 버퍼(Constant Buffer)를 셰이더에 설정
	CBuffer->UpdateConstBuffer();

	// 셰이더에 텍스처 리소스 뷰(SRV) 배열을 설정
	/*sSRVs->SetResourceArray(
		(ID3D11ShaderResourceView**)&SRVs, // 텍스처 리소스 배열
		0,                                 // 시작 인덱스
		MAX_MATERIAL_TEXTURE_COUNT         // 사용할 텍스처 개수
	);*/
	
}

void Material::Initialize()
{
	for (int i = 0; i < MAX_MATERIAL_TEXTURE_COUNT; i++)
	{
		Textures[i] = nullptr;		//// 텍스처 포인터 초기화
		SRVs[i] = nullptr;		// Shader Resource View 초기화
	}
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