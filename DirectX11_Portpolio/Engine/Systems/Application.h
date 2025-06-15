#pragma once


class Application
{
public:
	static void InitApplication(class IExecutable* InMain);
	static void Destroy();
	static void Run();
	static void Close();
	static bool IsRunning();


private:
	Application();
	~Application();
private:
	static class IExecutable* Main;
	static bool bIsRunning;
};

