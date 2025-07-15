#pragma once
#include "HeaderCollection.h"
#include "FileSystem/BinaryFile.h"


class Skeletal
{

private:
    int Index = 0;
    string Name = "";

    int ParentIndex = -1;
    shared_ptr<Skeletal> Parent = nullptr;

    Matrix Transform;
    vector<shared_ptr<Skeletal>> Children;

    vector<UINT> MeshNumbers;

private:
    friend class USkeletalMeshComponent;
    friend class Converter;
    friend class FClipData;
};
