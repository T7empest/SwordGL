//
// Created by Lukáš Blažek on 31.08.2025.
//

#include "sword_app.h"

#include <cassert>
#include <memory>

void SwordApp::init(int argc, char** argv)
{
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
		throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());

	create_window();
	gpu_context_ = std::make_unique<GPUContext>(window_);
	renderer_    = std::make_unique<Renderer>(gpu_context_.get());

	SDL_GPUCommandBuffer* cmd = gpu_context_->begin_cmd();
	renderer_->init(cmd);
	gpu_context_->end_cmd(cmd);

	particle_system_ = std::make_shared<ParticleSystem>();
}

void SwordApp::tick()
{
	prev_ns_ = now_ns_;
	calculate_dt();

	particle_system_->update(dt_);

	// TODO: buggy
	SDL_GPUCommandBuffer* cmd = gpu_context_->begin_cmd();
	if (!particle_system_->get_particles()->empty())
		renderer_->update(cmd, particle_system_);
	renderer_->render(cmd);
	gpu_context_->end_cmd(cmd);
}

bool SwordApp::handle_event(const SDL_Event& event)
{
	// request to quit
	if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) return false;

	// skip if ImGui wants to process this event
	UI::instance().process_event(event);
	if (ImGui::GetIO().WantCaptureMouse) return true;

	// handle events
	if (event.type == SDL_EVENT_MOUSE_MOTION)
	{
		std::cout << "emitting!" << std::endl;
		auto mouse_pos = glm::vec2(event.button.x, event.button.y);
		particle_system_->emit(dt_, mouse_pos);
	}

	return true;
}

void SwordApp::shutdown()
{
	SDL_DestroyWindow(window_);
	SDL_Quit();
}

void SwordApp::calculate_dt()
{
	now_ns_ = SDL_GetTicksNS();
	dt_     = static_cast<float>(now_ns_ - prev_ns_) * 1e-9f;
	if (dt_ > 0.1f) dt_ = 0.1f;
}

void SwordApp::create_window()
{
#ifdef __APPLE__
	window_ = SDL_CreateWindow(window_title.data(), window_start_width, window_start_height,
	                           SDL_WINDOW_RESIZABLE | SDL_WINDOW_METAL);
#else
	window_ = SDL_CreateWindow(window_title.data(), window_start_width, window_start_height,
	                           SDL_WINDOW_RESIZABLE);
#endif

	if (!window_)
		throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
}
