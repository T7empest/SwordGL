//
// Created by Lukáš Blažek on 31.08.2025.
//

#pragma once
#include <iostream>


struct CopyPassContext
{
	SDL_GPUCommandBuffer*  cmd;
	SDL_GPUTransferBuffer* transfer_buffer;
	SDL_GPUBuffer*         vertex_buffer;
	uint32_t               size;
};

class CopyPass
{
public:
	void execute(CopyPassContext& ctx);
};

inline void CopyPass::execute(CopyPassContext& ctx)
{
	SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(ctx.cmd);

	// where is the data
	SDL_GPUTransferBufferLocation src = {
		.transfer_buffer = ctx.transfer_buffer,
		.offset = 0
	};

	// where to upload the data
	SDL_GPUBufferRegion dst = {
		.buffer = ctx.vertex_buffer,
		.offset = 0,
		.size = ctx.size
	};

	SDL_UploadToGPUBuffer(copy_pass, &src, &dst, true);

	SDL_EndGPUCopyPass(copy_pass);
}
