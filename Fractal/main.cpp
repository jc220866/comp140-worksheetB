// main.cpp : Defines the entry point for the console application.
// Basic application layout source from the following tutorial
// http://www.willusher.io/pages/sdl2/

#include "stdafx.h"
#include "main.h"

// Lower numbers result in thicker outlines, 175 is a nice result
int maxIterations = 175;

int main(int, char**) 
{
	if ( ! Setup() ) { return 0; }

	while ( ! bWantsToQuit)
	{
		while ( SDL_PollEvent( &event ) ) 
		{
			CheckForQuit();
		}
		
		GenerateAndRenderFractal();
	}

	SDLCleanUp();
	return 0;
}

bool Setup()
{
	if (!InitializeVideo()) return false;
	else if (!CreateWindow()) return false;
	else if (!CreateRenderer()) return false;
	else if (!CreateTexture()) return false;

	else return true;
}

bool InitializeVideo()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	return true;
}

bool CreateWindow()
{
	window = SDL_CreateWindow("Fractal", 100, 100, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

	if (window == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return false;
	}

	return true;
}

bool CreateRenderer()
{
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (renderer == nullptr)
	{
		SDL_DestroyWindow(window);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return false;
	}

	return true;
}

bool CreateTexture()
{
	//Create a texture, STREAMING means that we will update the texture 
	fractalTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, windowWidth, windowHeight);
	
	if (fractalTexture == nullptr)
	{
		std::cout << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return false;
	}

	return true;
}

void GenerateAndRenderFractal()
{
	SDL_RenderClear(renderer);
	SDL_LockTexture(fractalTexture, NULL, (void**)&pixels, &WidthOfTextureInBytes);
	GenerateFractal();
	SDL_UnlockTexture(fractalTexture);
	SDL_RenderCopy(renderer, fractalTexture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void GenerateFractal()
{
	for (int pixelY = 0; pixelY < windowHeight; pixelY++)
	{
		my_y0 = (((double)pixelY / windowHeight) * (maxY - minY)) + minY;

		for (int pixelX = 0; pixelX < windowWidth; pixelX++)
		{
			my_x0 = (((double)pixelX / windowWidth) * (maxX - minX)) + minX;

			currentX = 0.0f;
			currentY = 0.0f;
			currentIteration = 0;

			pixelPosition = pixelX * (WidthOfTextureInBytes / pixelFormat->BytesPerPixel) + pixelY;

			IterateThroughPixels();
			ChangePixelColour();
		}
	}
}

void IterateThroughPixels()
{
	while ((currentX * currentX) + (currentY * currentY) <= 2*2 && currentIteration < maxIterations)
	{
		nextX = ((currentX * currentX) - (currentY * currentY)) + my_x0;

		currentY = (2 * currentX * currentY) + my_y0;

		currentX = nextX;

		currentIteration++;
	}
}

void ChangePixelColour()
{
	// Background colour set to RED
	Uint32 colour = SDL_MapRGB(pixelFormat, 220, 0, 0);

	if (currentIteration > (maxIterations * 0.99f))
	{
		/// Inside of the fractal, coloured a busy black
		colour = SDL_MapRGB(pixelFormat, rand() % 30, rand() % 30, rand() % 30);
	}
	else if (currentIteration > (maxIterations * 0.2f))
	{
		/// Inner border, coloured a bright yellow
		colour = SDL_MapRGB(pixelFormat, 255, 255, 0);
	}
	else if (currentIteration > (maxIterations * 0.1f))
	{
		/// Outer border, coloured a strong orange
		colour = SDL_MapRGB(pixelFormat, 255, 140, 0);
	}

	pixels[pixelPosition] = colour;
}

void CheckForQuit()
{
	switch (event.type)
	{
	case SDL_QUIT:
	{
		bWantsToQuit = true;
		break;
	}
	case SDL_KEYDOWN:
	{
		if (event.key.keysym.sym == SDLK_ESCAPE)
		{
			bWantsToQuit = true;
		}
		break;
	}
	}
}

void SDLCleanUp()
{
	SDL_FreeFormat(pixelFormat);
	SDL_DestroyTexture(fractalTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
