#include "HeaderCollection.h"
#include "AnimationData.h"

shared_ptr<FClipData::ClipTransform> FClipData::CalcClipTransform(const vector<shared_ptr<Skeletal>>& InBones)
{
    map<string, shared_ptr<FKeyFrameData>> keyframeNameTable;
	
    for (shared_ptr<FKeyFrameData>& data : Keyframes)
    {
        keyframeNameTable[data->BoneName] = data;
    }


    Matrix* bones = new Matrix[MAX_MODEL_TRANSFORM];
    shared_ptr<ClipTransform> transform = make_shared<ClipTransform>();

    for (UINT f = 0; f <= (UINT)Duration; f++)
    {
        for (UINT b = 0; b < InBones.size(); b++)
        {
            shared_ptr<Skeletal> bone = InBones[b];

            Matrix invGlobal = bone->Transform;
            invGlobal = invGlobal.Invert();

			
            Matrix parent;
			
            if (bone->ParentIndex < 0)
                parent = Matrix::Identity;
            else
                parent = bones[bone->ParentIndex];


            auto iter = keyframeNameTable.find(bone->Name);

            Matrix animation = Matrix::Identity;
            if (iter != keyframeNameTable.end())
            {
                shared_ptr<FKeyFrameData> data = iter->second;

                Vector3 position;
                if (data->Positions.size() - 1 >= f)
                {
                    position = data->Positions[f].mValue;
                }
                else
                {
                    UINT last = data->Positions.size() - 1;
                    position = data->Positions[last].mValue;
                }

                Vector3 scale;
                if (data->Scalings.size() - 1 >= f)
                {
                    scale = data->Scalings[f].mValue;
                }
                else
                {
                    UINT last = data->Scalings.size() - 1;
                    scale = data->Scalings[last].mValue;
                }


                Quaternion rotation;
                if (data->Rotations.size() - 1 >= f)
                {
                    rotation = data->Rotations[f].mValue;
                }
                else
                {
                    UINT last = data->Rotations.size() - 1;
                    rotation = data->Rotations[last].mValue;
                }

                Matrix S = Matrix::CreateScale(scale);
                Matrix R = Matrix::CreateFromQuaternion(rotation);
                Matrix T = Matrix::CreateTranslation(position);

                animation = S * R * T;
            }
			
            bones[b] = animation * parent;
            transform->Transform[f][b] = invGlobal * bones[b];
        }//for(b)
    }//for(f)

    return transform;
}
