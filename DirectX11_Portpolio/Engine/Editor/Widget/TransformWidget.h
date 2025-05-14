#pragma once
#include "Editor/Application/guiEditor.h"

class Actor;
struct FTransform;

class TransformWidget : public Editor
{
public:
    TransformWidget(Actor* target);
    virtual void OnGUI() override;

private:
    FTransform* mTransform;
    ImVec2 BoxSize;
    ImVec4 BoxColor;
};