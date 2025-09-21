//
// Created by Lukáš Blažek on 31.08.2025.
//

#pragma once

struct RenderPassContext
{
	SDL_GPUCommandBuffer*    cmd;
	SDL_GPUTexture*          target;
	SDL_GPUGraphicsPipeline* pipeline;

	std::array<SDL_GPUBuffer*, 2> vertex_buffers{nullptr, nullptr};
	std::array<uint64_t, 2>       vertex_offsets{0, 0};
	uint32_t                      num_vertex_buffers = 2;

	SDL_GPUBuffer*          index_buffer = nullptr;
	uint64_t                index_offset = 0;
	SDL_GPUIndexElementSize index_type   = SDL_GPU_INDEXELEMENTSIZE_16BIT;

	uint32_t vertex_count  = 0;
	uint32_t index_count   = 0;
	uint32_t first_vertex  = 0;
	uint32_t first_index   = 0;
	uint32_t num_instances = 1;

	bool clear = true;
	SDL_FColor clear_color = {0.f, 0.f, 240.f/255.f, 1.f};
};

class PresentPass
{
public:
	static void execute(RenderPassContext& ctx)
	{
		SDL_GPUColorTargetInfo color_target_info = {
			.texture = ctx.target,
			.clear_color = ctx.clear_color,
			.load_op = ctx.clear ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD,
			.store_op = SDL_GPU_STOREOP_STORE
		};

		SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(ctx.cmd, &color_target_info, 1, nullptr);

		SDL_BindGPUGraphicsPipeline(pass, ctx.pipeline);

		if (ctx.num_vertex_buffers > 0)
		{
			SDL_GPUBufferBinding vbs[2]{};
			for (size_t i = 0; i < ctx.num_vertex_buffers; ++i)
			{
				vbs[i].buffer = ctx.vertex_buffers[i];
				vbs[i].offset = ctx.vertex_offsets[i];
			}
			SDL_BindGPUVertexBuffers(pass, 0, vbs, ctx.num_vertex_buffers);
		}

		if (ctx.index_buffer && ctx.index_count > 0)
		{
			SDL_GPUBufferBinding ib{};
			ib.buffer = ctx.index_buffer;
			ib.offset = ctx.index_offset;
			SDL_BindGPUIndexBuffer(pass, &ib, ctx.index_type);

			SDL_DrawGPUIndexedPrimitives(
				pass,
				ctx.index_count,
				ctx.num_instances,
				ctx.first_index,
				0,
				0
			);
		}
		else
		{
			SDL_DrawGPUPrimitives(
				pass,
				ctx.vertex_count,
				ctx.num_instances,
				ctx.first_vertex,
				0
			);
		}

		SDL_EndGPURenderPass(pass);
	}
};
