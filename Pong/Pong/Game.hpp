#pragma once

#include<SDL.h>

struct Vector2
{
	float x;
	float y;
};

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	SDL_Window* mWindow;
	bool mIsRunning;
	SDL_Renderer* mRenderer;
	Vector2 mPaddle1Pos;
	Vector2 mPaddle2Pos;
	Vector2 mBallPos;
	Uint32 mTicksCount;
	int mPaddleDir;
	Vector2 mBallVel;
};