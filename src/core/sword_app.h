//
// Created by Lukáš Blažek on 31.08.2025.
//

#pragma once

#include <memory>
#include <string>

#include "../graphics/gpu_context.h"
#include "../graphics/particle_system.h"
#include "../graphics/renderer.h"
#include "../graphics/UI.h"

constexpr uint32_t         window_start_width  = 800;
constexpr uint32_t         window_start_height = 800;
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
	std::unique_ptr<Renderer>   renderer_    = nullptr;

	void     calculate_dt();
	uint64_t prev_ns_ = 0;
	uint64_t now_ns_  = 0;
	float    dt_      = 0;

	std::shared_ptr<ParticleSystem> particle_system_ = nullptr;

	void create_window();
};
