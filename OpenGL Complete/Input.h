#pragma once

#include <SDL.h>

class Input
{
public:
	Input();

	bool GetQuit();
	bool GetW();
	bool GetS();
	bool GetA();
	bool GetD();
	float GetYaw();
	float GetPitch();

	bool GetSpaceDown();

	void HandleInput();

private:
	float sensitivity = 0.1f;

	float yaw = -90.0f;
	float pitch = 0.0f;

	bool quit = false;
	bool w = false;
	bool s = false;
	bool a = false;
	bool d = false;

	bool space = false;
};


