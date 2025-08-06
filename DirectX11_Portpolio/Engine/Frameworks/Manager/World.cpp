#include "HeaderCollection.h"
#include "World.h"
#include "ULevel.h"
#include "Editor/WindowView/ConsoleWindow.h"
#include "Frameworks/Collision/CollisionDefinition.h"
#include "Systems/Application.h"

ULevel* World::PersistentLevel = nullptr;

void World::Initialize()
{
	PersistentLevel = new ULevel();
	PersistentLevel->Initialize();
}

void World::Destroy()
{
	Delete(PersistentLevel);
}

void World::Tick(float deltaTime)
{
	PersistentLevel->Tick(deltaTime);
}

ULevel* World::GetLevel()
{
	return PersistentLevel;
}

void World::SphereTraceMulti(vector<FHitResult>& hitResults, Vector3 location, float radius)
{
	PxSphereGeometry sphereGeom(radius);
	PxVec3 position = PxVec3(location.x, location.y, location.z);  // 감지할 지점
	PxTransform transform(position);

	const int MaxHit = 32;
	PxOverlapHit hitBuffer[MaxHit];
	PxOverlapBuffer hitInfo(hitBuffer, MaxHit);

	PxQueryFilterData filter(PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC);

	if (FPhysX::Get()->GetPhysScene()->overlap(sphereGeom, transform, hitInfo, filter))
	{
		for (PxU32 i = 0; i < hitInfo.nbTouches; ++i)
		{
			const PxOverlapHit& pxHit = hitInfo.touches[i];

			FHitResult hit;
			hit.TraceStart = location;
			hit.TraceEnd = location;
			hit.HitLocation = Vector3(position.x, position.y, position.z);
			hit.bBlockingHit = true;
			hit.HitActor = static_cast<Actor*>(pxHit.actor->userData);

			hitResults.push_back(hit);
		}
	}
}

void World::PushLog(const string& msg, Color color)
{
#ifdef __DEBUG
	ConsoleWindow::AddLog(msg, ImVec4(color.R(), color.G(), color.B(), color.A()));
#endif	
}
