#pragma once
#include "Utility/Delegate.h"


class Application
{
public:
	static void InitApplication(class IExecutable* InMain);
	static void Destroy();
	static void Run();
	static void Close();
	static bool IsRunning();
	

private:

	static void MainRender();

private:
	Application();
	~Application();
private:
	static class IExecutable* Main;
	static bool bIsRunning;
};

