#pragma once
#include "Frameworks/Actor.h"

class UStaticMeshComponent;


class ACubeMap : public Actor
{
public:
	ACubeMap(wstring InTextureFileName, wstring InObjectFileName);
	virtual ~ACubeMap() = default;

	virtual void Tick(float deltaTime) override;
	virtual void Render() override;

	void InitCubeMapTexture();
public:
	shared_ptr<UStaticMeshComponent> Cube;
	ComPtr<ID3D11ShaderResourceView> SRV;

};