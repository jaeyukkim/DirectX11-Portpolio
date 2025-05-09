#include "HeaderCollection.h"
#include "Skeletal.h"

Skeletal::Skeletal()
{
}

Skeletal::~Skeletal()
{
}

void Skeletal::ReadFile(BinaryReader* InReader, vector<shared_ptr<Skeletal>>& OutBones)
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
        OutBones.push_back(bone);
    }

    //부모-자식 관계 설정
    for (shared_ptr<Skeletal>& bone : OutBones)
    {
        if (bone->ParentIndex < 0)
            continue;

        bone->Parent = OutBones[bone->ParentIndex];
        bone->Parent->Children.push_back(bone);
    }
}