#pragma once
#include "Frameworks/Actor.h"

class UStaticMeshComponent;
class AFloor : public Actor
{
public:
	AFloor();
	virtual ~AFloor() = default;

	virtual void Tick(float deltaTime) override;
	virtual void Render() override;

	float Roughness = 1.0f;     //4
	float Metallic = 1.0f;      //4
	int bUseAlbedoMap = false;  //4
	int bUseNormalMap = false;  //4
	int bInvertNormalMapY = false;  //4
	int bUseMetallicMap = false;    //4
	int bUseRoughnessMap = false;    //4
public:
	shared_ptr<UStaticMeshComponent> floor;

};