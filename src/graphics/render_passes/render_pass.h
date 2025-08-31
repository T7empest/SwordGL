//
// Created by Lukáš Blažek on 31.08.2025.
//

#pragma once
#include "SDL3/SDL_gpu.h"

struct RenderPassContext
{
	SDL_GPUCommandBuffer* cmd;
	SDL_GPUTexture*       target;
};

class IRenderPass
{
public:
	virtual ~IRenderPass() = default;

	virtual void execute(RenderPassContext& ctx) = 0;
};
