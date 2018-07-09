#pragma once

#include<SDL.h>
#include<vector>

struct Vector2
{
	float x;
	float y;
};

struct Ball
{
	Vector2 mPosition;
	Vector2 mVelocity;
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
	Uint32 mTicksCount;
	int mPaddle1Dir;
	int mPaddle2Dir;
	std::vector<Ball> mBalls;
};