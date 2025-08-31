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
#ifdef __APPLE__
	window_ = SDL_CreateWindow(window_title.data(), window_start_width, window_start_height,
								   SDL_WINDOW_RESIZABLE | SDL_WINDOW_METAL);
#else
	window_ = SDL_CreateWindow(window_title.data(), window_start_width, window_start_height,
	                           SDL_WINDOW_RESIZABLE);
#endif
	if (!window_)
		throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());

	gpu_context_ = std::make_unique<GPUContext>(window_);
	assert(gpu_context_ != nullptr);

	renderer_    = std::make_unique<Renderer>(gpu_context_.get());
	SDL_GPUCommandBuffer* cmd = gpu_context_->begin_cmd();
	renderer_->init(cmd);
	gpu_context_->end_cmd(cmd);
}

void SwordApp::tick()
{

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
	// handle keyboard/mouse/resize...
	return true;
}

void SwordApp::shutdown()
{
	SDL_DestroyWindow(window_);

	if (SDL_WasInit(SDL_INIT_STATUS_INITIALIZED))
	{
		SDL_Quit();
	}
}

/*
	src
		main.cc
		core/
			Time.hpp                       // delta time & FPS helper
			Input.hpp / Input.cc          // keyboard/mouse mapping
			sword_app.hpp / sword_app.cc
		gfx/
			GpuContext.hpp / GpuContext.cc  // wraps SDL_gpu init, device, swap chain
			Renderer.hpp / Renderer.cc      // frame graph-ish: begin/end frame, passes
			Pipeline.hpp / Pipeline.cc      // shaders + fixed state bundle
			Texture.hpp / Texture.cc        // texture resource
			Mesh.hpp / Mesh.cc              // vertex/index buffers
			Camera.hpp                       // view/proj builder
			Resources.hpp / Resources.cc    // on-disk loading, caching
		scenes/
			Scene.hpp / Scene.cc            // list of renderables, update()
			DemoScene.hpp / DemoScene.cc
*/
