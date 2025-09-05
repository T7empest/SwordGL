//
// Created by Lukáš Blažek on 31.08.2025.
//

#pragma once

struct RenderPassContext
{
	SDL_GPUCommandBuffer*    cmd;
	SDL_GPUTexture*          target;
	SDL_GPUGraphicsPipeline* pipeline;
	SDL_GPUBuffer*           vertex_buffer;
	uint32_t                 vertex_count;
	uint32_t                 num_instances;
};

class PresentPass
{
public:
	static void execute(RenderPassContext& ctx);
};

inline void PresentPass::execute(RenderPassContext& ctx)
{
	SDL_GPUColorTargetInfo color_target_info = {
		.texture = ctx.target,
		.clear_color = {0 / 255.0f, 0 / 255.0f, 240 / 255.0f, 255 / 255.0f},
		.load_op = SDL_GPU_LOADOP_CLEAR,
		.store_op = SDL_GPU_STOREOP_STORE
	};

	SDL_GPURenderPass* render_pass =
		SDL_BeginGPURenderPass(ctx.cmd, &color_target_info, 1, nullptr);

	SDL_BindGPUGraphicsPipeline(render_pass, ctx.pipeline);

	SDL_GPUBufferBinding buffer_bindings[1];
	buffer_bindings[0].buffer = ctx.vertex_buffer;
	buffer_bindings[0].offset = 0;

	SDL_BindGPUVertexBuffers(render_pass, 0, buffer_bindings, 1);

	// issue a draw call
	SDL_DrawGPUPrimitives(render_pass, ctx.vertex_count, ctx.num_instances, 0, 0);

	SDL_EndGPURenderPass(render_pass);
}
