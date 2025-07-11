#pragma once
#include "Editor/Application/guiEditor.h"

class Actor;
struct FTransform;

class TransformWidget : public Editor
{
public:
    TransformWidget(Actor* target);
    virtual void OnGUI() override;
    virtual void UpdateTransformWidget();
    
    FTransform* ActorTransform;
    Vector3 Position;
    Vector3 Rotation;
    Vector3 Scale;
    ImVec2 BoxSize;
    ImVec4 BoxColor;
};