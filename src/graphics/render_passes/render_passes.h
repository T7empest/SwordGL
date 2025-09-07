//
// Created by Lukáš Blažek on 07.09.2025.
//

#pragma once
#include <stdexcept>

#include "SDL3/SDL_gpu.h"

namespace CopyPass
{
	struct Context
	{
		SDL_GPUBuffer* dst        = nullptr;
		size_t         dst_offset = 0;
		const void*    src        = nullptr;
		size_t         size_bytes = 0;
	};

	// Enqueue a deferred upload to a GPU buffer.
	// Returns the created staging buffer. DO NOT release it until the GPU finished this command buffer.
	inline SDL_GPUTransferBuffer* enqueue_upload(SDL_GPUDevice*        device,
	                                             SDL_GPUCommandBuffer* cmd,
	                                             const Context&        ctx)
	{
		if (!device || !cmd || !ctx.dst || !ctx.src || ctx.size_bytes == 0)
			throw std::runtime_error("enqueue_upload: invalid arguments\n");

		SDL_GPUTransferBufferCreateInfo tbi = {
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = static_cast<uint32_t>(ctx.size_bytes)
		};
		SDL_GPUTransferBuffer* staging = SDL_CreateGPUTransferBuffer(device, &tbi);

		if (void* dst = SDL_MapGPUTransferBuffer(device, staging, false))
		{
			SDL_memcpy(dst, ctx.src, ctx.size_bytes);
			SDL_UnmapGPUTransferBuffer(device, staging);
		}
		else
		{
			SDL_ReleaseGPUTransferBuffer(device, staging);
			throw std::runtime_error("enqueue_upload: failed to map transfer buffer");
		}

		SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd);

		SDL_GPUTransferBufferLocation src{
			.transfer_buffer = staging,
			.offset = 0
		};
		SDL_GPUBufferRegion dst{
			.buffer = ctx.dst,
			.offset = static_cast<uint32_t>(ctx.dst_offset),
			.size = static_cast<uint32_t>(ctx.size_bytes)
		};

		SDL_UploadToGPUBuffer(copy_pass, &src, &dst, false);

		SDL_EndGPUCopyPass(copy_pass);

		return staging;
	}
}
