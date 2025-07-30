#include "HeaderCollection.h"
#include "AGameMode.h"
#include "APlayerController.h"
#include "../../../Game/AKachujin.h"
#include "../../../Game/System/World.h"
#include "../../../Game/System/ULevel.h"


AGameMode::AGameMode()
{
    Actor* controller = SpawnActor<APlayerController>(World::GetLevel());

    FTransform transform;
    transform.SetPosition(0.0f, 200.0f, 0.0f);
    Actor* playerCharacter = SpawnActorAtLocation<AKachujin>(World::GetLevel(), transform);

    
    if(APlayerController* PC = dynamic_cast<APlayerController*>(controller))
    {
        if(ACharacter* character = dynamic_cast<ACharacter*>(playerCharacter))
        {
            PC->Possess(character);
            World::GetLevel()->SetPlayerCharacter(character);
        }
    }
}

void AGameMode::Tick(float deltaTime)
{
    Actor::Tick(deltaTime);
}

void AGameMode::StartPlay()
{
}

void AGameMode::SpawnDefaultPawn(APlayerController* NewPlayer)
{
}

void AGameMode::SpawnPlayerController(INT32 PlayerIndex)
{
}

void AGameMode::SetDefaultPawnClass(ClassID InClass)
{
}

void AGameMode::SetDefaultControllerClass(ClassID InClass)
{
}
