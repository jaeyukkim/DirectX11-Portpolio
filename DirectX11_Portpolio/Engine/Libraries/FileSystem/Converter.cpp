#include <HeaderCollection.h>
#include "Converter.h"

#include "Frameworks/Animation/UAnimInstance.h"
#include "Render/Resource/AnimationData.h"

Converter::Converter()
{
	Loader = make_shared<Assimp::Importer>();
}

Converter::~Converter()
{

}

void Converter::ImportFBXFile(const wstring objectName, const EMeshType& meshType)
{
	wstring fbxPath = L"../../Contents/_Assets/" + objectName + L"/" + objectName + L".fbx";
	wstring gltfPath = L"../../Contents/_Assets/" + objectName + L"/" + objectName + L".gltf";
	bIsGLTF = false;
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
	ImportFBX_Material(objectName, true, meshType);
	ImportFBX_Mesh(objectName, meshType);
	
}


void Converter::ImportFBX_Material(wstring InSaveFileName, bool InOverwrite, EMeshType InMeshType)
{
	switch (InMeshType)
	{
	case EMeshType::StaticMeshType:
		InSaveFileName = L"../../Contents/_Objects/" + InSaveFileName + L"/" + InSaveFileName + L".material";
		break;

	case EMeshType::SkeletalMeshType:
		InSaveFileName = L"../../Contents/_Models/" + InSaveFileName + L"/" + InSaveFileName + L".material";
		break;

	default:
		Assert(true, "ExportMaterial 실패");
	}
	
	ReadFBX_Material();
	ConvertFBX_ToBinary_Material(InSaveFileName, InOverwrite);
}

void Converter::ReadFBX_Material()
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

void Converter::ConvertFBX_ToBinary_Material(wstring InSaveFileName, bool InOverwrite)
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

void Converter::ImportFBX_Animation(wstring objectName, wstring animationName, int InClipIndex)
{
	wstring fbxPath = L"../../Contents/_Assets/" + objectName + L"/" + animationName + L".fbx";

	bIsGLTF = false;
	wstring finalPath;
	if (filesystem::exists(fbxPath))
	{
		finalPath = fbxPath;
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

	wstring exportFileName = L"../../Contents/_Models/" + objectName +
		L"/Animations/" + animationName + L".animation";
	
	shared_ptr<FClipData> clipData = ReadAnimationData(Scene->mAnimations[InClipIndex]);
	ConvertFBX_ToBinary_Animation(exportFileName, clipData);
	
}

void Converter::ReadBinary_Anim(wstring InFileName, USkeletalMeshComponent* meshComp, bool bHasCreated)
{
	if (meshComp == nullptr || bHasCreated) return;


	ifstream stream;
	stream.open(InFileName);
	Json::Value root;
	stream >> root;
	
	Json::Value animations = root["Animations"];
	stream.close();

	
	for (UINT i = 0; i < animations.size(); i++)
		ReadFBX_Animation(String::ToWString(animations[i].asString()), meshComp);
	
	if (animations.size() > 0)
	{
		ComPtr<ID3D11Texture2D> ClipTexture = nullptr;
		ComPtr<ID3D11ShaderResourceView> ClipSRV = nullptr;
		
		AnimationTexture::CreateAnimationTexture(meshComp, ClipTexture, ClipSRV);
		meshComp->AnimInstance->ClipsSRV = ClipSRV;
	}
}



/**
 * mNumPositionKeys가 duration만큼 존재하지 않을 때 중간에 보간 데이터 삽입하는 과정
 */
shared_ptr<FClipData> Converter::ReadAnimationData(aiAnimation* InAnimation)
{
	shared_ptr<FClipData> clipData = make_shared<FClipData>();

	clipData->Name = InAnimation->mName.C_Str();
	clipData->Duration = (float)InAnimation->mDuration;
	clipData->TickersPerSecond = (float)InAnimation->mTicksPerSecond;

	for (UINT i = 0; i < InAnimation->mNumChannels; i++)
	{
	    aiNodeAnim* nodeAnim = InAnimation->mChannels[i];

	    shared_ptr<FKeyFrameData> FrameData = make_shared<FKeyFrameData>();
	    FrameData->BoneName = nodeAnim->mNodeName.C_Str();

	    // 키 프레임을 임시로 rawPosKeys에 저장
	    std::vector<FFrameData<Vector3>> rawPosKeys;
	    for (UINT keyIndex = 0; keyIndex < nodeAnim->mNumPositionKeys; keyIndex++)
	    {
	        const aiVectorKey& key = nodeAnim->mPositionKeys[keyIndex];

	        FFrameData<Vector3> PosData;
	        PosData.mTime = (float)key.mTime;
	        memcpy(&PosData.mValue, &key.mValue, sizeof(Vector3));

	        rawPosKeys.push_back(PosData);
	    }

	    // 프레임 데이터의 Positon의 키를 duration만큼 할당
	    FrameData->Positions.resize((UINT)clipData->Duration);

	    for (UINT f = 0; f < (UINT)clipData->Duration; f++)
	    {
	        float currentTime = (float)f;

	    	//해당 키 프레임이 없을 경우 identity 삽입
	        if (rawPosKeys.size() == 0)
	        {
	            FrameData->Positions[f].mValue = Vector3(0, 0, 0);
	        }
	    	// 키 프레임에 데이터가 1개일 경우 마지막 틱에 동일한 데이터 삽입
	        else if (rawPosKeys.size() == 1)
	        {
	            FrameData->Positions[f].mValue = rawPosKeys[0].mValue;
	        }
	        else
	        {
	            Vector3 value = rawPosKeys.back().mValue;

	            // Find the key interval (i, i+1) where key[i].mTime <= time < key[i+1].mTime
	            for (size_t k = 0; k < rawPosKeys.size() - 1; ++k)
	            {
	                float t1 = rawPosKeys[k].mTime;
	                float t2 = rawPosKeys[k + 1].mTime;

	                if (currentTime < t1)
	                {
	                    value = rawPosKeys[0].mValue;
	                    break;
	                }
	                else if (currentTime >= t1 && currentTime < t2)
	                {
	                    float alpha = (currentTime - t1) / (t2 - t1);
	                    Vector3 a = rawPosKeys[k].mValue;
	                    Vector3 b = rawPosKeys[k + 1].mValue;
	                    value = Vector3::Lerp(a, b, alpha);
	                    break;
	                }
	            }

	            FrameData->Positions[f].mValue = value;
	        }

	        FrameData->Positions[f].mTime = currentTime;
	    }


		std::vector<FFrameData<Vector3>> rawScaleKeys;
		for (UINT k = 0; k < nodeAnim->mNumScalingKeys; k++)
		{
			const aiVectorKey& key = nodeAnim->mScalingKeys[k];

			FFrameData<Vector3> s;
			s.mTime = (float)key.mTime;
			memcpy(&s.mValue, &key.mValue, sizeof(Vector3));
			rawScaleKeys.push_back(s);
		}

		FrameData->Scalings.resize((UINT)clipData->Duration);

		for (UINT f = 0; f < (UINT)clipData->Duration; f++)
		{
			float currentTime = (float)f;

			if (rawScaleKeys.size() == 0)
			{
				FrameData->Scalings[f].mValue = Vector3(1, 1, 1); // Identity scale
			}
			else if (rawScaleKeys.size() == 1)
			{
				FrameData->Scalings[f].mValue = rawScaleKeys[0].mValue;
			}
			else
			{
				Vector3 value = rawScaleKeys.back().mValue;

				for (size_t k = 0; k < rawScaleKeys.size() - 1; ++k)
				{
					float t1 = rawScaleKeys[k].mTime;
					float t2 = rawScaleKeys[k + 1].mTime;

					if (currentTime < t1)
					{
						value = rawScaleKeys[0].mValue;
						break;
					}
					else if (currentTime >= t1 && currentTime < t2)
					{
						float alpha = (currentTime - t1) / (t2 - t1);
						value = Vector3::Lerp(rawScaleKeys[k].mValue, rawScaleKeys[k + 1].mValue, alpha);
						break;
					}
				}

				FrameData->Scalings[f].mValue = value;
			}

			FrameData->Scalings[f].mTime = currentTime;
		}

		
	    // ⏱ Rotation 처리 (Slerp)
	    {
	        std::vector<FFrameData<Quaternion>> rawRotKeys;
	        for (UINT k = 0; k < nodeAnim->mNumRotationKeys; k++)
	        {
	            const aiQuatKey& key = nodeAnim->mRotationKeys[k];

	            FFrameData<Quaternion> q;
	            q.mTime = (float)key.mTime;
	            q.mValue = Quaternion(key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w);
	            rawRotKeys.push_back(q);
	        }

	        FrameData->Rotations.resize((UINT)clipData->Duration);

	        for (UINT f = 0; f < (UINT)clipData->Duration; f++)
	        {
	            float currentTime = (float)f;

	            if (rawRotKeys.size() == 0)
	            {
	                FrameData->Rotations[f].mValue = Quaternion();
	            }
	            else if (rawRotKeys.size() == 1)
	            {
	                FrameData->Rotations[f].mValue = rawRotKeys[0].mValue;
	            }
	            else
	            {
	                Quaternion value = rawRotKeys.back().mValue;

	                for (size_t k = 0; k < rawRotKeys.size() - 1; ++k)
	                {
	                    float t1 = rawRotKeys[k].mTime;
	                    float t2 = rawRotKeys[k + 1].mTime;

	                    if (currentTime < t1)
	                    {
	                        value = rawRotKeys[0].mValue;
	                        break;
	                    }
	                    else if (currentTime >= t1 && currentTime < t2)
	                    {
	                        float alpha = (currentTime - t1) / (t2 - t1);
	                        value = Quaternion::Slerp(rawRotKeys[k].mValue, rawRotKeys[k + 1].mValue, alpha);
	                        break;
	                    }
	                }

	                FrameData->Rotations[f].mValue = value;
	            }

	            FrameData->Rotations[f].mTime = currentTime;
	        }
	    }

	    // 🟨 Scale 처리도 동일 방식으로 rawScaleKeys 처리하면 됩니다

	    clipData->Keyframes.push_back(FrameData);
	}
	

	return clipData;
}

void Converter::ImportFBX_Mesh(wstring InSaveFileName, EMeshType FileType)
{
	
	switch (FileType)
	{
	case EMeshType::StaticMeshType:
		InSaveFileName = L"../../Contents/_Objects/" + InSaveFileName + L"/" + InSaveFileName + L".mesh";
		ReadFBX_StaticMesh();
		ConvertFBX_ToBinary_Mesh(InSaveFileName);
		break;

	case EMeshType::SkeletalMeshType:
		InSaveFileName = L"../../Contents/_Models/" + InSaveFileName + L"/" + InSaveFileName + L".mesh";
		ReadFBX_Bone(Scene->mRootNode, 0, -1);
		ReadFBX_SkeletalMesh();
		ConvertFBX_ToBinary_SkeletalMesh(InSaveFileName);
		break;
	}
	
}

bool Converter::IsAssimpFbxHelperNode(const string& name)
{
	return name.find("_$AssimpFbx$_") != std::string::npos;
}

void Converter::ReadFBX_Bone(aiNode* InNode, int InIndex, int InParent)
{
	std::string nodeName = InNode->mName.C_Str();

	// 보조 노드 패턴 감지
	if (IsAssimpFbxHelperNode(nodeName))
	{
		// 부모 노드에게 현재 노드의 변환을 넘기고, 이 노드는 무시
		if (InParent >= 0)
		{
			Matrix helperTransform;
			memcpy(&helperTransform, InNode->mTransformation[0], sizeof(Matrix));
			helperTransform = helperTransform.Transpose();

			Bones[InParent]->Transform *= helperTransform;
		}

		// 자식들은 건너뛰지 않고 계속 처리
		for (UINT i = 0; i < InNode->mNumChildren; i++)
		{
			ReadFBX_Bone(InNode->mChildren[i], InIndex, InParent);
		}

		return; // 보조 노드는 추가하지 않음
	}

	// 일반 본 처리
	BoneData* bone = new BoneData();
	bone->Index = InIndex;
	bone->Parent = InParent;
	bone->Name = nodeName;

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
		ReadFBX_Bone(InNode->mChildren[i], (int)Bones.size(), InIndex);
}

void Converter::ReadFBX_SkeletalMesh()
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

		

			data->Vertices.push_back(vertex);
		}
		
		memcpy_s(&data->AABB.Max, sizeof(Vector3), &mesh->mAABB.mMax, sizeof(Vector3));
		memcpy_s(&data->AABB.Min, sizeof(Vector3), &mesh->mAABB.mMin, sizeof(Vector3));

		

		for (UINT f = 0; f < mesh->mNumFaces; f++)
		{
			aiFace& face = mesh->mFaces[f];

			for (UINT k = 0; k < face.mNumIndices; k++)
				data->Indices.push_back(face.mIndices[k]);
		}
		
		SkeletalMeshes.push_back(data);
	}
}



void Converter::ConvertFBX_ToBinary_SkeletalMesh(wstring InSaveFileName)
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
		
		w->ToByte(&data->AABB.Max, sizeof(Vector3));
		w->ToByte(&data->AABB.Min, sizeof(Vector3));
		Delete(data);
	}
	
	w->Close();
	
}

void Converter::ReadFBX_Animation(wstring InFilePath, USkeletalMeshComponent* meshComp)
{
	InFilePath = L"../../Contents/_Models/" + InFilePath + L".animation";
	
	
	shared_ptr<BinaryReader> reader = make_shared<BinaryReader>();
	reader->Open(InFilePath);

	shared_ptr<FClipData> animation = make_shared<FClipData>();
	{
		animation->Name = reader->FromString();
		animation->Duration = reader->FromFloat();
		animation->TickersPerSecond = reader->FromFloat();

		UINT count = reader->FromUInt();
		animation->Keyframes.assign(count, nullptr);

		for (UINT i = 0; i < count; i++)
		{
			shared_ptr<FKeyFrameData> data = make_shared<FKeyFrameData>();
		
			string name = reader->FromString();
			auto iter = meshComp->ReadBoneTable.find(name);

			if (iter != meshComp->ReadBoneTable.end())
			{
				data->BoneIndex = iter->second->Index;
				data->BoneName = iter->second->Name;
			}

		
			UINT frameCount = 0;

			frameCount = reader->FromUInt();
			FFrameData<Vector3>* position = new FFrameData<Vector3>[frameCount];
			reader->FromByte(position, sizeof(FFrameData<Vector3>) * frameCount);
			data->Positions.assign(position, position + frameCount);

			frameCount = reader->FromUInt();
			FFrameData<Vector3>* scaling = new FFrameData<Vector3>[frameCount];
			reader->FromByte(scaling, sizeof(FFrameData<Vector3>) * frameCount);
			data->Scalings.assign(scaling, scaling + frameCount);

			frameCount = reader->FromUInt();
			FFrameData<Quaternion>* rotation = new FFrameData<Quaternion>[frameCount];
			reader->FromByte(rotation, sizeof(FFrameData<Quaternion>) * frameCount);
			data->Rotations.assign(rotation, rotation + frameCount);

		
			if(iter != meshComp->ReadBoneTable.end())
				animation->Keyframes[i] = data;
		}
		meshComp->AnimInstance->Animations.push_back(animation);

		reader->Close();
	
	}
}

void Converter::ConvertFBX_ToBinary_Animation(wstring InSaveFileName, shared_ptr<FClipData> InClipData)
{
	Path::CreateFolders(Path::GetDirectoryName(InSaveFileName));

	shared_ptr<BinaryWriter> w = make_shared<BinaryWriter>();
	w->Open(InSaveFileName);

	w->ToString(InClipData->Name);

	w->ToFloat(InClipData->Duration);
	w->ToFloat(InClipData->TickersPerSecond);

	w->ToUInt(InClipData->Keyframes.size());
	for (shared_ptr<FKeyFrameData>& nodeData : InClipData->Keyframes)
	{
		w->ToString(nodeData->BoneName);


		UINT count = 0;

		count = nodeData->Positions.size();
		w->ToUInt(count);
		w->ToByte(&nodeData->Positions[0], sizeof(FFrameData<Vector3>) * count);

		count = nodeData->Scalings.size();
		w->ToUInt(count);
		w->ToByte(&nodeData->Scalings[0], sizeof(FFrameData<Vector3>) * count);

		count = nodeData->Rotations.size();
		w->ToUInt(count);
		w->ToByte(&nodeData->Rotations[0], sizeof(FFrameData<Quaternion>) * count);
		
	}
	
	w->Close();
}

void Converter::ReadFBX_StaticMesh()
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

		
		memcpy_s(&data->AABB.Max, sizeof(Vector3), &mesh->mAABB.mMax, sizeof(Vector3));
		memcpy_s(&data->AABB.Min, sizeof(Vector3), &mesh->mAABB.mMin, sizeof(Vector3));

		
		
		for (UINT f = 0; f < mesh->mNumFaces; f++)
		{
			aiFace& face = mesh->mFaces[f];

			for (UINT k = 0; k < face.mNumIndices; k++)
				data->Indices.push_back(face.mIndices[k]);
		}

		StaticMeshes.push_back(data);
	}
}


void Converter::ConvertFBX_ToBinary_Mesh(wstring InSaveFileName)
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
		w->ToByte(&data->AABB.Max, sizeof(Vector3));
		w->ToByte(&data->AABB.Min, sizeof(Vector3));

		Delete(data);
	}
	
	w->Close();
}


void Converter::ReadBinary_Bone(BinaryReader* InReader, USkeletalMeshComponent* meshComp)
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