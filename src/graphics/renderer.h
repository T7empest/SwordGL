//
// Created by Lukáš Blažek on 31.08.2025.
//

#pragma once
#include <memory>

#include "gpu_context.h"
#include "../scenes/scene.h"
#include "render_passes/copy_pass.h"
#include "render_passes/present_pass.h"
#include "SDL3/SDL_gpu.h"
#include <iostream>


class Renderer
{
public:
	explicit Renderer(GPUContext* gpu_context)
		: gpu_context_(gpu_context),
		  present_pass_(std::make_unique<PresentPass>()),
		  copy_pass_(std::make_unique<CopyPass>())
	{
	}

	~Renderer();
	void init(SDL_GPUCommandBuffer* cmd);
	void render(SDL_GPUCommandBuffer* cmd_buf);

private:
	void uploadScene(Scene::Vertex*        scene_vertices, uint32_t vertex_count,
	                 SDL_GPUCommandBuffer* cmd);

	GPUContext*                  gpu_context_;
	std::unique_ptr<PresentPass> present_pass_;
	std::unique_ptr<CopyPass>    copy_pass_;
	SDL_GPUBuffer*               vertex_buffer_ = nullptr;
};
