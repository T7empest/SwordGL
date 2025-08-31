//
// Created by Lukáš Blažek on 31.08.2025.
//

#pragma once
#include "render_pass.h"

class PresentPass final : public IRenderPass
{
public:
	void execute(RenderPassContext& ctx) override;
};

inline void PresentPass::execute(RenderPassContext& ctx)
{
	SDL_GPUColorTargetInfo color_target_info = {
		.texture = ctx.target,
		.clear_color = {240/255.0f, 240/255.0f, 240/255.0f, 255/255.0f},
		.load_op = SDL_GPU_LOADOP_CLEAR,
		.store_op = SDL_GPU_STOREOP_STORE
	};

	SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(ctx.cmd, &color_target_info, 1, nullptr);



	SDL_EndGPURenderPass(render_pass);
}
