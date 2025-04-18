#include "Pch.h"
#include "TestGui.h"

#include "AKachujin.h"
#include "Systems/Application.h"



void TestGui::Initialize()
{
	Actors.push_back(make_shared<AKachujin>());

	testval++;
}

void TestGui::Tick(float deltaTime)
{
	ImGui::SliderFloat("PositionY", &testval, -100.0f, 100.0f);
	
	for (shared_ptr<Actor>& actor : Actors)
	{
		actor->Tick(deltaTime);
	}
}

void TestGui::Destroy()
{

}

void TestGui::Render()
{
	for (shared_ptr<Actor>& actor : Actors)
	{
		actor->Render();
	}
}

void TestGui::Interact()
{
	
}
