#include "Pch.h"
#include "TestGui.h"

#include "AKachujin.h"
#include "Systems/Application.h"



void TestGui::Initialize()
{
	AKachujin* kachujin = new AKachujin();

	testval++;
}

void TestGui::Tick(float deltaTime)
{
	if(Keyboard::Get()->Press('D'))
		testval += 1.0f * deltaTime;
	
	ImGui::SliderFloat("PositionY", &testval, -100.0f, 100.0f);
	
}

void TestGui::Destroy()
{

}

void TestGui::Render()
{

}

void TestGui::Interact()
{
	
}
