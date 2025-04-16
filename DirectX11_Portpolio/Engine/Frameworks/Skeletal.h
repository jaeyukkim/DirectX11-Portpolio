#pragma once
#include "Utility/BinaryFile.h"

class Skeletal
{
public:
    Skeletal();
    ~Skeletal();

private:
    static void ReadFile(BinaryReader* InReader, vector<shared_ptr<Skeletal>>& OutBones);

private:
    int Index = 0;
    string Name = "";

    int ParentIndex = -1;
    shared_ptr<Skeletal> Parent = nullptr;

    Matrix Transform;
    vector<shared_ptr<Skeletal>> Children;

    vector<UINT> MeshNumbers;

private:
    friend class ESkeletalMeshComponent;
};
