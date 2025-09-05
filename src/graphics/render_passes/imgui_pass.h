//
// Created by Lukáš Blažek on 05.09.2025.
//

#pragma once
#include <UI.h>
#include <iostream>
#include <memory>

#include "UI.h"
#include "imgui_impl_sdlgpu3.h"
#include "SDL3/SDL_gpu.h"

// at the end of render loop

struct ImguiPassContext
{
	SDL_GPUCommandBuffer* cmd;
	SDL_GPUTexture* target;
};

class ImguiPass
{
public:
	static void execute(ImguiPassContext& ctx);
};

inline void ImguiPass::execute(ImguiPassContext& ctx)
{
	ImDrawData* dd = ImGui::GetDrawData();
	ImGui_ImplSDLGPU3_PrepareDrawData(dd, ctx.cmd);

	SDL_GPUColorTargetInfo target_info = {
		.texture = ctx.target,
		.clear_color = SDL_FColor{1.0f, 0.0f, 0.0f, 1.0f},
		.load_op = SDL_GPU_LOADOP_LOAD,
		.store_op = SDL_GPU_STOREOP_STORE,
		.mip_level = 0,
		.layer_or_depth_plane = 0,
		.cycle = false
	};
	SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(ctx.cmd, &target_info, 1, nullptr);

	ImGui_ImplSDLGPU3_RenderDrawData(dd, ctx.cmd, render_pass);

	SDL_EndGPURenderPass(render_pass);
}
