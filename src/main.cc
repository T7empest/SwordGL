//
// Created by Lukáš Blažek on 31.08.2025.
//

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "core/sword_app.h"

int main(int argc, char* argv[])
{
	try
	{
		SwordApp app;
		app.init(argc, argv);

		SDL_Event event;

		bool running = true;
		while (running)
		{
			while (SDL_PollEvent(&event))
			{
				if (!app.handle_event(event))
				{
					running = false;
					break;
				}
			}
			app.tick();
		}
		return 0;
	}
	catch (const std::exception& ex)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fatal error: %s", ex.what());
		std::cerr << "Fatal error: " << ex.what() << "\n";
		return 1;
	}
	catch (...)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fatal error: unknown exception");
		std::cerr << "Fatal error: unknown exception\n";
		return 1;
	}
}
