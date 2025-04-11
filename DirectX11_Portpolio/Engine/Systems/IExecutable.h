#pragma once

class IExecutable
{
public:
	virtual void Initialize() {}
	virtual void Destroy() {}

	virtual void Tick(float deltaTime) {}
	virtual void Render() {}
};
