//
// Created by Lukáš Blažek on 05.09.2025.
//

#pragma once
#include "render_passes/imgui_pass.h"
#include "SDL3/SDL.h"
#include <UI.h>

class UI
{
public:
	// singleton
	static UI& instance()
	{
		static UI inst;
		return inst;
	}

	void init(SDL_Window* window, SDL_GPUDevice* device);
	bool process_event(const SDL_Event& event);
	void render(ImguiPassContext& ctx);
	void shutdown();

	void main_window();

private:
	UI()                        = default;
	~UI()                       = default;
	UI(const UI&)            = delete;
	UI& operator=(const UI&) = delete;
};
