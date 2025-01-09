#include "Input.h"

Input::Input()
{
}

bool Input::GetQuit()
{
	return quit;
}

bool Input::GetW()
{
	return w;
}

bool Input::GetS()
{
	return s;
}

bool Input::GetA()
{
	return a;
}

bool Input::GetD()
{
	return d;
}

bool Input::GetSpaceDown()
{
	return space;
}

float Input::GetYaw()
{
	return yaw;
}

float Input::GetPitch()
{
	return pitch;
}

void Input::HandleInput()
{
	space = false;
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			quit = true;
		}

		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_w)
			{
				w = true;
			}
			if (event.key.keysym.sym == SDLK_s)
			{
				s = true;
			}
			if (event.key.keysym.sym == SDLK_a)
			{
				a = true;
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				d = true;
			}
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				quit = true;
			}
			if (event.key.keysym.sym == SDLK_SPACE)
			{
				space = true;
			}
		}

		if (event.type == SDL_KEYUP)
		{
			if (event.key.keysym.sym == SDLK_w)
			{
				w = false;
			}
			if (event.key.keysym.sym == SDLK_s)
			{
				s = false;
			}
			if (event.key.keysym.sym == SDLK_a)
			{
				a = false;
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				d = false;
			}
		}

		if (event.type == SDL_MOUSEMOTION)
		{
			float relativeX = event.motion.xrel * sensitivity;
			float relativeY = event.motion.yrel * sensitivity;

			yaw += relativeX;
			pitch -= relativeY;

			if (pitch > 89.0f)
			{
				pitch = 89.0f;
			}
			if (pitch < -89.0f)
			{
				pitch = -89.0f;
			}
		}
	}
}
