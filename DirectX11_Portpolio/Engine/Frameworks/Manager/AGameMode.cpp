#include "HeaderCollection.h"
#include "AGameMode.h"
#include "APlayerController.h"
#include "../../../Game/AKachujin.h"
#include "../../../Game/System/World.h"
#include "../../../Game/System/ULevel.h"


AGameMode::AGameMode()
{
    Actor* controller = SpawnActor<APlayerController>(World::GetLevel());
    Actor* playerCharacter = SpawnActor<AKachujin>(World::GetLevel());

    if(APlayerController* PC = dynamic_cast<APlayerController*>(controller))
    {
        if(ACharacter* character = dynamic_cast<ACharacter*>(playerCharacter))
        PC->Possess(character);
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
