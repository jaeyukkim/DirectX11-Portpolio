#pragma once
#include "Frameworks/Objects/Actor.h"

class ACharacter;
class APlayerController;

class AGameMode : public Actor
{
public:
    AGameMode();
    virtual void Tick(float deltaTime) override;
    virtual void StartPlay();
    
    virtual void SpawnDefaultPawn(class APlayerController* NewPlayer);
    virtual void SpawnPlayerController(int32 PlayerIndex);

    // Pawn, Controller의 기본 클래스 지정
    void SetDefaultPawnClass(ClassID InClass); // 커스텀 클래스로 지정
    void SetDefaultControllerClass(ClassID InClass);
    
private:
    ClassID DefaultPawnClass = string("ACharacter");
    ClassID DefaultControllerClass = string("APlayerController");
};
