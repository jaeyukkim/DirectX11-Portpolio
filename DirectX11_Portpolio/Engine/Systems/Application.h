#pragma once
#include "Utility/Delegate.h"


class Application
{
public:
	static WPARAM Run(class IExecutable* InMain);
	

private:
	static void Create();
	static void Destroy();

	static LRESULT CALLBACK WndProc(HWND InHandle, UINT InMessage, WPARAM InwParam, LPARAM InlParam);

	static void MainRender();

	
	
private:
	Application();
	~Application();
private:
	static class IExecutable* Main;
};

