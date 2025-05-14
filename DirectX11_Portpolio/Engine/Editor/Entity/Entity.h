#pragma once

class Entity
{
public:
	Entity() = default;
	virtual ~Entity() = default;

	void SetName(const string& name) { mName = name; }
	string& GetName() { return mName; }

private:
	string mName;
};