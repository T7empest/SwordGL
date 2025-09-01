//
// Created by Lukáš Blažek on 31.08.2025.
//

#include "renderer.h"

Renderer::~Renderer()
{
	SDL_ReleaseGPUBuffer(gpu_context_->get_device(), vertex_buffer_);
}

void Renderer::init(SDL_GPUCommandBuffer* cmd)
{
	auto scene_vertices = Scene::vertices;
	auto vertex_count   = Scene::vertices.size();

	uploadScene(scene_vertices.data(), vertex_count, cmd);
}


void Renderer::render(SDL_GPUCommandBuffer* cmd_buf)
{
	auto swapchain_texture = gpu_context_->get_swapchain_texture();


	RenderPassContext ctx = {
		.cmd = cmd_buf,
		.target = swapchain_texture
	};

	present_pass_->execute(ctx);
}

void Renderer::uploadScene(Scene::Vertex*        scene_vertices, uint32_t vertex_count,
                           SDL_GPUCommandBuffer* cmd)
{
	auto vb_bytesize = static_cast<uint32_t>(vertex_count * sizeof(Scene::Vertex));
	SDL_GPUDevice* device = gpu_context_->get_device();
	SDL_GPUBufferCreateInfo buffer_info = {
		.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
		.size = vb_bytesize
	};
	vertex_buffer_ = SDL_CreateGPUBuffer(device, &buffer_info);

	SDL_GPUTransferBufferCreateInfo transfer_info = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = vb_bytesize
	};

	SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_info);

	if (void* data = SDL_MapGPUTransferBuffer(device, transfer_buffer, false))
	{
		SDL_memcpy(data, scene_vertices, vb_bytesize);
		SDL_UnmapGPUTransferBuffer(device, transfer_buffer);
		std::cout << "copy successful" << std::endl;
	}
	CopyPassContext ctx = {
		.cmd = cmd,
		.transfer_buffer = transfer_buffer,
		.vertex_buffer = vertex_buffer_,
		.size = vb_bytesize
	};

	copy_pass_->execute(ctx);

	// TODO: Po copy_pass_->execute(ctx) musí následovat submit a fence/wait.
	// TODO: Teprve pak SDL_ReleaseGPUTransferBuffer. Jinak riskuješ use-after-free na GPU.

	SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
}
