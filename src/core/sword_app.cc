//
// Created by Lukáš Blažek on 31.08.2025.
//

#include "sword_app.h"

#include <cassert>
#include <memory>

ComponentController controller;

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

	init_particle_system();
}

void SwordApp::tick()
{
	prev_ns_ = now_ns_;
	calculate_dt();

	SDL_GPUCommandBuffer* cmd = gpu_context_->begin_cmd();
	renderer_->render(cmd);
	gpu_context_->end_cmd(cmd);
}

bool SwordApp::handle_event(const SDL_Event& event)
{
	if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
	{
		return false; // request to quit
	}

	if (UI::instance().process_event(event))
	{
		// don't process app events when imgui wants to
		return true;
	}

	// TODO: the event determining doesnt work and the emit function throws errors
	// handle app events...
	if (event.button.down && event.button.button == 0)
	{
		std::cout << "emitting!" << std::endl;
		particle_system_->emit(dt_);
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
	dt_     = static_cast<float> (now_ns_ - prev_ns_);
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

void SwordApp::init_particle_system()
{
	controller.Init();
	controller.RegisterComponent<Transform>();
	controller.RegisterComponent<RigidBody>();
	controller.RegisterComponent<Temporary>();

	particle_system_ = controller.RegisterSystem<ParticleSystem>();
	{
		Signature signature;
		signature.set(controller.GetComponentType<Transform>());
		signature.set(controller.GetComponentType<RigidBody>());
		signature.set(controller.GetComponentType<Temporary>());
		controller.SetSystemSignature<ParticleSystem>(signature);
	}
}
