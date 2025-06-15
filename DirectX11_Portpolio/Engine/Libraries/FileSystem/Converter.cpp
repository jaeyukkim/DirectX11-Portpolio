#include <HeaderCollection.h>
#include "Converter.h"

Converter::Converter()
{
	Loader = make_shared<Assimp::Importer>();
}

Converter::~Converter()
{

}

void Converter::ReadFile(const wstring objectName, const EMeshType& meshType)
{
	wstring fbxPath = L"../Contents/_Assets/" + objectName + L"/" + objectName + L".fbx";
	wstring gltfPath = L"../Contents/_Assets/" + objectName + L"/" + objectName + L".gltf";
	bool bIsGLTF = false;
	wstring finalPath;
	if (filesystem::exists(fbxPath))
	{
		finalPath = fbxPath;
	}
	else if (filesystem::exists(gltfPath))
	{
		finalPath = gltfPath;
		bIsGLTF = true;
	}
	else
	{
		wcout << L"모델 파일이 존재하지 않습니다: " << endl;
		return;
	}

	Scene = Loader->ReadFile
	(
		String::ToString(finalPath).c_str(),
		aiProcess_ConvertToLeftHanded
		| aiProcess_Triangulate
		| aiProcess_GenUVCoords
		| aiProcess_GenNormals
		| aiProcess_CalcTangentSpace
		| aiProcess_GenBoundingBoxes

	);
	Assert(Scene != nullptr, "모델 정상 로드 안됨");
	ExportMaterial(objectName, true, meshType);
	ExportMesh(objectName, meshType);
	
}


void Converter::ExportMaterial(wstring InSaveFileName, bool InOverwrite, EMeshType InMeshType)
{
	switch (InMeshType)
	{
	case EMeshType::StaticMeshType:
		InSaveFileName = L"../Contents/_Objects/" + InSaveFileName + L"/" + InSaveFileName + L".material";
		break;

	case EMeshType::SkeletalMeshType:
		InSaveFileName = L"../Contents/_Models/" + InSaveFileName + L"/" + InSaveFileName + L".material";
		break;
	}
	
	ReadMaterials(InMeshType);
	WriteMaterial(InSaveFileName, InOverwrite);
}

void Converter::ReadMaterials(EMeshType InMeshType)
{
	
	for (UINT i = 0; i < Scene->mNumMaterials; i++)
	{
		aiMaterial* material = Scene->mMaterials[i];
		MaterialData* data = new MaterialData();
		
		data->Name = material->GetName().C_Str();
		
		aiColor4D color;
		float value;

		material->Get(AI_MATKEY_BASE_COLOR, color);
		data->Albedo = Color(color.r, color.g, color.b, color.a);

		material->Get(AI_MATKEY_ROUGHNESS_FACTOR, value);
		data->Roughness = value;

		material->Get(AI_MATKEY_METALLIC_FACTOR, value);
		data->Metallic = value;

		material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		data->Emissive = Color(color.r, color.g, color.b, color.a);



		aiString textureFile;

		material->GetTexture(aiTextureType_BASE_COLOR, 0, &textureFile);
		data->AlbedoFile = textureFile.C_Str();
		
		material->GetTexture(aiTextureType_METALNESS, 0, &textureFile);
		data->MetallicFile = textureFile.C_Str();
		
		material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &textureFile);
		data->DiffuseRoughnessFile = textureFile.C_Str();

		material->GetTexture(aiTextureType_NORMAL_CAMERA, 0, &textureFile);
		data->NormalFile = textureFile.C_Str();

		material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &textureFile);
		data->AmbientOcclusionFile = textureFile.C_Str();

		material->GetTexture(aiTextureType_EMISSIVE, 0, &textureFile);
		data->EmissiveFile = textureFile.C_Str();

		material->GetTexture(aiTextureType_HEIGHT, 0, &textureFile);
		data->HeightFile = textureFile.C_Str();
		
		Materials.push_back(data);
	}
}

void Converter::WriteMaterial(wstring InSaveFileName, bool InOverwrite)
{
	if (InOverwrite == false)
	{
		if (Path::ExistFile(InSaveFileName) == true)
			return;
	}


	string folderName = String::ToString(Path::GetDirectoryName(InSaveFileName));
	string fileName = String::ToString(Path::GetFileName(InSaveFileName));

	Path::CreateFolders(folderName);


	Json::Value root;

	for (MaterialData* data : Materials)
	{
		Json::Value value;
		
		value["Albedo"] = ColorToJson(data->Albedo);
		value["Roughness"] = FloatToJson(data->Roughness);
		value["Metallic"] = FloatToJson(data->Metallic);
		value["Emissive"] = ColorToJson(data->Emissive);
		value["AlbedoMap"] = SaveTexture(folderName, data->AlbedoFile);
		value["MetallicMap"] = SaveTexture(folderName, data->MetallicFile);
		value["RoughnessMap"] = SaveTexture(folderName, data->DiffuseRoughnessFile);
		value["NormalMap"] = SaveTexture(folderName, data->NormalFile);
		value["AmbientOcclusionMap"] = SaveTexture(folderName, data->AmbientOcclusionFile);
		value["EmissiveMap"] = SaveTexture(folderName, data->EmissiveFile);
		value["HeightMap"] = SaveTexture(folderName, data->HeightFile);
		root[data->Name.c_str()] = value;

		Delete(data);
	}


	fileName = folderName + fileName;

	Json::StyledWriter writer;
	string str = writer.write(root);

	ofstream stream;
	stream.open(fileName);
	stream << str;
	stream.close();
}

string Converter::ColorToJson(const Color& InColor)
{
	return String::Format("%f,%f,%f,%f", InColor.x, InColor.y, InColor.z, InColor.w);
}

string Converter::FloatToJson(const float val)
{
	return String::Format("%f", val);
}

Color Converter::JsonStringToColor(string InString)
{
	vector<string> v;
	String::SplitString(&v, InString, ",");

	return Color(stof(v[0]), stof(v[1]), stof(v[2]), stof(v[3]));
}

string Converter::SaveTexture(string InSaveFolder, string InFileName)
{
	CheckTrueResult(InFileName.length() < 1, "");
	CheckTrueResult(InSaveFolder.length() < 1, "");


	string fileName = Path::GetFileName(InFileName);
	const aiTexture* texture = Scene->GetEmbeddedTexture(InFileName.c_str());

	if (texture != nullptr)
	{
		if (texture->mHeight < 1)
		{
			BinaryWriter writer;
			writer.Open(String::ToWString(InSaveFolder + fileName));
			writer.ToByte(texture->pcData, texture->mWidth);
			writer.Close();

			return InSaveFolder + fileName;
		}

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = texture->mWidth;
		desc.Height = texture->mHeight;
		desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA subResource;
		ZeroMemory(&subResource, sizeof(D3D11_SUBRESOURCE_DATA));
		subResource.pSysMem = texture->pcData;


		ID3D11Texture2D* saveTexture;
		Check(D3D::Get()->GetDevice()->CreateTexture2D(&desc, &subResource, &saveTexture));

		ScratchImage scratch;
		HRESULT hr = CaptureTexture(D3D::Get()->GetDevice(), D3D::Get()->GetDeviceContext(), saveTexture, scratch);

		Check(hr);
		
		std::wstring savePath = String::ToWString(InSaveFolder + fileName);
		
		hr = SaveToWICFile(
			*scratch.GetImage(0, 0, 0),          // Get first mip level image
			WIC_FLAGS_NONE,
			GUID_ContainerFormatPng,
			savePath.c_str()
		);
		return InSaveFolder + fileName;
	}


	string directory = Path::GetDirectoryName(String::ToString(ReadFilePath));
	string origin = directory + InFileName;
	String::Replace(&origin, "\\", "/");

	CheckFalseResult(Path::ExistFile(origin), "");


	string path = InSaveFolder + fileName;
	CopyFileA(origin.c_str(), path.c_str(), FALSE);

	return InSaveFolder + Path::GetFileName(path);
}

void Converter::ExportMesh(wstring InSaveFileName, EMeshType FileType)
{
	
	switch (FileType)
	{
	case EMeshType::StaticMeshType:
		InSaveFileName = L"../Contents/_Objects/" + InSaveFileName + L"/" + InSaveFileName + L".mesh";
		ReadStaticMeshData();
		WriteStaticMeshData(InSaveFileName);
		break;

	case EMeshType::SkeletalMeshType:
		InSaveFileName = L"../Contents/_Models/" + InSaveFileName + L"/" + InSaveFileName + L".mesh";
		ReadBoneData(Scene->mRootNode, 0, -1);
		ReadSkeletalMeshData();
		WriteSkeletalMeshData(InSaveFileName);
		break;
	}
	
}

void Converter::ReadBoneData(aiNode* InNode, int InIndex, int InParent)
{
	BoneData* bone = new BoneData();
	bone->Index = InIndex;
	bone->Parent = InParent;
	bone->Name = InNode->mName.C_Str();

	memcpy(&bone->Transform, InNode->mTransformation[0], sizeof(Matrix));
	bone->Transform = bone->Transform.Transpose();

	
	Matrix parent;

	if (InParent < 0)
		parent = Matrix::Identity;
	else
		parent = Bones[InParent]->Transform;

	bone->Transform = bone->Transform * parent;

	for (UINT i = 0; i < InNode->mNumMeshes; i++)
		bone->MeshNumbers.push_back(InNode->mMeshes[i]);

	Bones.push_back(bone);

	for (UINT i = 0; i < InNode->mNumChildren; i++)
		ReadBoneData(InNode->mChildren[i], (int)Bones.size(), InIndex);
}

void Converter::ReadSkeletalMeshData()
{
	for (UINT i = 0; i < Scene->mNumMeshes; i++)
	{
		SkeletalMeshData* data = new SkeletalMeshData();

		aiMesh* mesh = Scene->mMeshes[i];
		
		UINT materialIndex = mesh->mMaterialIndex;
		aiMaterial* material = Scene->mMaterials[materialIndex];
		
		
		data->Name = mesh->mName.C_Str();

		data->MaterialName = material->GetName().C_Str();


		for (UINT v = 0; v < mesh->mNumVertices; v++)
		{
			VertexModel vertex;

			
			memcpy_s(&vertex.Position, sizeof(Vector3), &mesh->mVertices[v], sizeof(Vector3));

			if(mesh->HasTextureCoords(0))
				memcpy_s(&vertex.Uv, sizeof(Vector2), &mesh->mTextureCoords[0][v], sizeof(Vector2));

			if(mesh->HasVertexColors(0))
				memcpy_s(&vertex.Color, sizeof(Color), &mesh->mColors[0][v], sizeof(Color));

			if (mesh->HasNormals())
				memcpy_s(&vertex.Normal, sizeof(Vector3), &mesh->mNormals[v], sizeof(Vector3));

			if (mesh->HasTangentsAndBitangents())
				memcpy_s(&vertex.Tangent, sizeof(Vector3), &mesh->mTangents[v], sizeof(Vector3));

			if (bIsGLTF)
				ConvertToDXCoord(&vertex.Normal, &vertex.Tangent);

			data->Vertices.push_back(vertex);
		}

		for (UINT f = 0; f < mesh->mNumFaces; f++)
		{
			aiFace& face = mesh->mFaces[f];

			for (UINT k = 0; k < face.mNumIndices; k++)
				data->Indices.push_back(face.mIndices[k]);
		}

		SkeletalMeshes.push_back(data);
	}
}

void Converter::ConvertToDXCoord(Vector3* normal, Vector3* tangent)
{
	// Normal 변환
	float temp= -normal->y;
	normal->y = normal->z;
	normal->z = temp;
	normal->Normalize();
	
	// Tangent 변환
	float tempT = -tangent->y;
	tangent->y = tangent->z;
	tangent->z = tempT;
	tangent->Normalize();
}


void Converter::WriteSkeletalMeshData(wstring InSaveFileName)
{
	Path::CreateFolders(Path::GetDirectoryName(InSaveFileName));

	shared_ptr<BinaryWriter> w = make_shared<BinaryWriter>();
	w->Open(InSaveFileName);

	w->ToUInt((UINT)Bones.size());
	for (BoneData* data : Bones)
	{
		w->ToUInt(data->Index);
		w->ToString(data->Name);

		w->ToInt(data->Parent);
		w->ToMatrix(data->Transform);
		w->ToUInt((UINT)data->MeshNumbers.size());

		if(data->MeshNumbers.size() > 0)
			w->ToByte(&data->MeshNumbers[0], (UINT)(sizeof(UINT) * data->MeshNumbers.size()));

		Delete(data);
	}

	w->ToUInt((UINT)SkeletalMeshes.size());
	for (SkeletalMeshData* data : SkeletalMeshes)
	{
		w->ToString(data->Name);
		w->ToString(data->MaterialName);
		w->ToUInt(data->BoneIndex);

		w->ToUInt((UINT)data->Vertices.size());
		w->ToByte(&data->Vertices[0], (UINT)(sizeof(VertexModel) * data->Vertices.size()));

		w->ToUInt((UINT)data->Indices.size());
		w->ToByte(&data->Indices[0], (UINT)(sizeof(UINT) * data->Indices.size()));

		Delete(data);
	}
	
	w->Close();
	
}

void Converter::ReadStaticMeshData()
{
	for (UINT i = 0; i < Scene->mNumMeshes; i++)
	{
		StaticMeshData* data = new StaticMeshData();

		aiMesh* mesh = Scene->mMeshes[i];
		
		UINT materialIndex = mesh->mMaterialIndex;
		aiMaterial* material = Scene->mMaterials[materialIndex];
		
		
		data->Name = mesh->mName.C_Str();

		data->MaterialName = material->GetName().C_Str();


		for (UINT v = 0; v < mesh->mNumVertices; v++)
		{
			VertexObject vertex;

			
			memcpy_s(&vertex.Position, sizeof(Vector3), &mesh->mVertices[v], sizeof(Vector3));

			if(mesh->HasTextureCoords(0))
				memcpy_s(&vertex.Uv, sizeof(Vector2), &mesh->mTextureCoords[0][v], sizeof(Vector2));

			if(mesh->HasVertexColors(0))
				memcpy_s(&vertex.Color, sizeof(Color), &mesh->mColors[0][v], sizeof(Color));

			if (mesh->HasNormals())
				memcpy_s(&vertex.Normal, sizeof(Vector3), &mesh->mNormals[v], sizeof(Vector3));

			if (mesh->HasTangentsAndBitangents())
				memcpy_s(&vertex.Tangent, sizeof(Vector3), &mesh->mTangents[v], sizeof(Vector3));

			data->Vertices.push_back(vertex);
		}

		for (UINT f = 0; f < mesh->mNumFaces; f++)
		{
			aiFace& face = mesh->mFaces[f];

			for (UINT k = 0; k < face.mNumIndices; k++)
				data->Indices.push_back(face.mIndices[k]);
		}

		StaticMeshes.push_back(data);
	}
}


void Converter::WriteStaticMeshData(wstring InSaveFileName)
{
	Path::CreateFolders(Path::GetDirectoryName(InSaveFileName));

	shared_ptr<BinaryWriter> w = make_shared<BinaryWriter>();
	w->Open(InSaveFileName);
	
	w->ToUInt((UINT)StaticMeshes.size());
	for (StaticMeshData* data : StaticMeshes)
	{
		w->ToString(data->Name);
		w->ToString(data->MaterialName);

		w->ToUInt((UINT)data->Vertices.size());
		w->ToByte(&data->Vertices[0], (UINT)(sizeof(VertexObject) * data->Vertices.size()));

		w->ToUInt((UINT)data->Indices.size());
		w->ToByte(&data->Indices[0], (UINT)(sizeof(UINT) * data->Indices.size()));

		Delete(data);
	}
	
	w->Close();
}


void Converter::ReadBoneData(BinaryReader* InReader, USkeletalMeshComponent* meshComp)
{
	UINT count = InReader->FromUInt();

	for (UINT i = 0; i < count; i++)
	{
		auto bone = make_shared<Skeletal>();

		bone->Index = InReader->FromUInt();
		bone->Name = InReader->FromString();

		bone->ParentIndex = InReader->FromInt();
		bone->Transform = InReader->FromMatrix();

		// Bone이 포함하는 Mesh 개수 읽기
		UINT meshCount = InReader->FromUInt();

		if (meshCount > 0)
		{
			//Mesh 개수만큼 공간을 할당
			bone->MeshNumbers.assign(meshCount, UINT());

			// Mesh 인덱스 정보를 바이너리 데이터에서 읽어옴
			void* ptr = (void*)&(bone->MeshNumbers[0]);
			InReader->FromByte(ptr, sizeof(UINT) * meshCount);
		}
        
		//읽은 Bone을 Bone 리스트에 추가
		meshComp->Bones.push_back(bone);
	}

	//부모-자식 관계 설정
	for (shared_ptr<Skeletal>& bone : meshComp->Bones)
	{
		if (bone->ParentIndex < 0)
			continue;

		bone->Parent = meshComp->Bones[bone->ParentIndex];
		bone->Parent->Children.push_back(bone);
	}
}
