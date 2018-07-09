#include"Game.hpp"
#include<time.h>
const int thickness = 15;
const float paddleH = 100.0f;
const float numberOfBalls = 3;
int randomSign = 1;

Game::Game(): mWindow(nullptr), mRenderer(nullptr), mTicksCount(0), mIsRunning(true), mPaddle1Dir(0), mPaddle2Dir(0) {}

bool Game::Initialize()
{
	srand(time(NULL));

	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow("Pong Game", 100, 100, 1024, 768, 0);
	if (!mWindow)
	{
		SDL_Log("Failed to create a window: %s", SDL_GetError());
		return false;
	}
	
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	mPaddle1Pos.x = 10.0f;
	mPaddle1Pos.y = 768.0f / 2.0f;
	mPaddle2Pos.x = 1024.0f - thickness - 10.0f;
	mPaddle2Pos.y = 768.0f / 2.0f;

	for (int i = 0; i < numberOfBalls; ++i)
	{
		int posx = rand() % 900 + 100;
		int posy = rand() % 670 + 100;
		int velx = -150 * randomSign;
		randomSign *= (-1);
		int vely = 200 * randomSign;
		randomSign *= (-1);

		Ball newBall;
		newBall.mPosition.x = posx;
		newBall.mPosition.y = posy;
		newBall.mVelocity.x = velx;
		newBall.mVelocity.y = vely;
		mBalls.push_back(newBall);
	}
	return true;
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}

	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	mPaddle1Dir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddle1Dir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddle1Dir += 1;
	}

	mPaddle2Dir = 0;
	if (state[SDL_SCANCODE_I])
	{
		mPaddle2Dir -= 1;
	}
	if (state[SDL_SCANCODE_K])
	{
		mPaddle2Dir += 1;
	}
}

void Game::UpdateGame()
{
	//ensure at least 16ms elapses between frames
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

	//converted to seconds
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	mTicksCount = SDL_GetTicks();

	if (mPaddle1Dir != 0)
	{
		mPaddle1Pos.y += mPaddle1Dir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mPaddle1Pos.y < (paddleH / 2.0f + thickness))
		{
			mPaddle1Pos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddle1Pos.y >(768.0f - paddleH / 2.0f - thickness))
		{
			mPaddle1Pos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	if (mPaddle2Dir != 0)
	{
		mPaddle2Pos.y += mPaddle2Dir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mPaddle2Pos.y < (paddleH / 2.0f + thickness))
		{
			mPaddle2Pos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddle2Pos.y >(768.0f - paddleH / 2.0f - thickness))
		{
			mPaddle2Pos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	for (int i = 0; i < mBalls.size(); ++i)
	{
		mBalls[i].mPosition.x += mBalls[i].mVelocity.x * deltaTime;
		mBalls[i].mPosition.y += mBalls[i].mVelocity.y * deltaTime;

		float diff1 = mPaddle1Pos.y - mBalls[i].mPosition.y;
		float diff2 = mPaddle2Pos.y - mBalls[i].mPosition.y;

		diff1 = (diff1 > 0.0f) ? diff1 : -diff1;
		diff2 = (diff2 > 0.0f) ? diff2 : -diff2;

		if (diff1 <= paddleH / 2.0f && mBalls[i].mPosition.x <= 25.0f && mBalls[i].mPosition.x >= 20.0f && mBalls[i].mVelocity.x < 0.0f)
		{
			mBalls[i].mVelocity.x *= -1.0f;
		}
		if (diff2 <= paddleH / 2.0f && mBalls[i].mPosition.x >= 1000.0f && mBalls[i].mPosition.x <= 1005.0f && mBalls[i].mVelocity.x > 0.0f)
		{
			mBalls[i].mVelocity.x *= -1.0f;
		}
		//off screen?
		else if (mBalls[i].mPosition.x <= 0.0f || mBalls[i].mPosition.x >= 1024.0f)
		{
			mIsRunning = false;
		}
		//ball collided with the top wall
		if (mBalls[i].mPosition.y <= thickness && mBalls[i].mVelocity.y < 0.0f)
		{
			mBalls[i].mVelocity.y *= -1;
		}
		//ball collided with the bottom wall?
		else if (mBalls[i].mPosition.y >= (768 - thickness) &&
			mBalls[i].mVelocity.y > 0.0f)
		{
			mBalls[i].mVelocity.y *= -1;
		}
	}
	

}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(mRenderer, 18, 19, 15, 255);
	SDL_RenderClear(mRenderer);

	SDL_SetRenderDrawColor(mRenderer, 91, 146, 121, 255);
	//top wall
	SDL_Rect wall
	{
		0,
		0,
		1024,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &wall);

	//bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	SDL_SetRenderDrawColor(mRenderer, 243, 230, 229, 255);
	//ball
	for (int i = 0; i < mBalls.size(); ++i)
	{
		SDL_Rect ball{
			static_cast<int>(mBalls[i].mPosition.x - thickness / 2),
			static_cast<int>(mBalls[i].mPosition.y - thickness / 2),
			thickness,
			thickness
		};
		SDL_RenderFillRect(mRenderer, &ball);
	}


	SDL_SetRenderDrawColor(mRenderer, 143, 203, 155, 255);
	//paddle1
	SDL_Rect paddle1{
		static_cast<int>(mPaddle1Pos.x),
		static_cast<int>(mPaddle1Pos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle1);

	SDL_SetRenderDrawColor(mRenderer, 143, 128, 115, 255);
	//paddle2
	SDL_Rect paddle2{
		static_cast<int>(mPaddle2Pos.x),
		static_cast<int>(mPaddle2Pos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle2);


	SDL_RenderPresent(mRenderer);

}

