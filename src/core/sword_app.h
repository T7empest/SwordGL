//
// Created by Lukáš Blažek on 31.08.2025.
//

#pragma once

#include <memory>
#include <string>
#include <SDL3/SDL.h>

#include "../graphics/gpu_context.h"
#include "../graphics/renderer.h"

constexpr uint32_t         window_start_width  = 400;
constexpr uint32_t         window_start_height = 400;
constexpr std::string_view window_title        = "sword particles";

class SwordApp
{
public:
	SwordApp() = default;
	~SwordApp() { shutdown(); }

	// non-copyable, non-movable (holds raw SDL handles)
	SwordApp(const SwordApp&)            = delete;
	SwordApp& operator=(const SwordApp&) = delete;
	SwordApp(SwordApp&&)                 = delete;
	SwordApp& operator=(SwordApp&&)      = delete;

	void init(int argc, char** argv);
	void tick();
	bool handle_event(const SDL_Event& event); // false when requesting quit
	void shutdown();

private:
	SDL_Window*                 window_      = nullptr;
	std::unique_ptr<GPUContext> gpu_context_ = nullptr;
	std::unique_ptr<Renderer>	renderer_	 = nullptr;
};
