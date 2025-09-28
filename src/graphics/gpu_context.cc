//
// Created by Lukáš Blažek on 31.08.2025.
//

#include "graphics/gpu_context.h"

#include <iostream>
#include <string>

GPUContext::GPUContext(SDL_Window* window)
	: window_(window)
{
	device_ = create_device(shader_format_);
	if (!device_)
		throw std::runtime_error(std::string("failed to create gpu device\n") + SDL_GetError());

	if (!SDL_ClaimWindowForGPUDevice(device_, window_))
		throw std::runtime_error(std::string("failed to claim window for gpu\n") + SDL_GetError());
}

SDL_GPUCommandBuffer* GPUContext::begin_cmd()
{
	auto cmd = SDL_AcquireGPUCommandBuffer(device_);
	if (!cmd)
		throw std::runtime_error(std::string("failed to create command buffer\n") + SDL_GetError());

	// acquire swapchain texture
	if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window_, &swapchain_texture_, nullptr, nullptr))
		throw std::runtime_error(std::string("failed to acquire swapchain texture\n") + SDL_GetError());

	return cmd;
}

void GPUContext::end_cmd(SDL_GPUCommandBuffer* cmd)
{
	if (!SDL_SubmitGPUCommandBuffer(cmd))
		throw std::runtime_error(std::string("submitting empty command buffer!") + SDL_GetError());
}

SDL_GPUDevice* GPUContext::create_device(SDL_GPUShaderFormat shader_format) const
{
#ifndef NDEBUG
	bool debug = true;
#else
	bool debug = false;
#endif
	std::cout << "debug mode: " << debug << std::endl;
	return SDL_CreateGPUDevice(shader_format, debug, graphics_api_);
}
