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
    // 1. Bone 개수를 읽어옴
    UINT count = InReader->FromUInt();

    // 2. Bone 개수만큼 반복하여 데이터를 읽음
    for (UINT i = 0; i < count; i++)
    {
        // 2.1 새로운 Bone 객체 생성
        auto bone = make_shared<Skeletal>();

        // 2.2 Bone의 인덱스 (몇 번째 Bone인지)
        bone->Index = InReader->FromUInt();

        // 2.3 Bone의 이름 (문자열로 저장됨)
        bone->Name = InReader->FromString();

        // 2.4 Bone의 부모 인덱스 (없다면 -1)
        bone->ParentIndex = InReader->FromInt();

        // 2.5 Bone의 변환 행렬(Transform Matrix) 읽기
        bone->Transform = InReader->FromMatrix();

        // 2.6 Bone이 포함하는 Mesh 개수 읽기
        UINT meshCount = InReader->FromUInt();

        // 2.7 Mesh 개수가 1개 이상이면 Mesh 정보를 저장
        if (meshCount > 0)
        {
            // 2.7.1 Mesh 개수만큼 공간을 할당 (초기화)
            bone->MeshNumbers.assign(meshCount, UINT());

            // 2.7.2 Mesh 인덱스 정보를 바이너리 데이터에서 읽어옴
            void* ptr = (void*)&(bone->MeshNumbers[0]);
            InReader->FromByte(&ptr, sizeof(UINT) * meshCount);
        }

        // 2.8 읽은 Bone을 Bone 리스트에 추가
        OutBones.push_back(bone);
    }

    // 3. 부모-자식 관계 설정
    for (shared_ptr<Skeletal>& bone : OutBones)
    {
        // 3.1 부모 인덱스가 없는(-1) 본은 최상위(Bone Root)이므로 건너뜀
        if (bone->ParentIndex < 0)
            continue;

        // 3.2 부모 Bone을 찾아서 설정
        bone->Parent = OutBones[bone->ParentIndex];

        // 3.3 부모 Bone의 자식 목록에 현재 Bone을 추가
        bone->Parent->Children.push_back(bone);
    }
}
