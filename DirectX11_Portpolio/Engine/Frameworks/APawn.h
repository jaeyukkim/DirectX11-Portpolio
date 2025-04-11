#pragma once
#include "Actor.h"
#include <memory>
using namespace std;

class ESceneComponent;


class APawn : public Actor
{
public:
	APawn();
	~APawn();

	shared_ptr<ESceneComponent> RootComponent;
}; 