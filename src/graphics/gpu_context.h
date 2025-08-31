//
// Created by Lukáš Blažek on 31.08.2025.
//

#pragma once
#include "SDL3/SDL_gpu.h"


class GPUContext
{
public:
	explicit GPUContext(SDL_Window* window);


	~GPUContext() { SDL_DestroyGPUDevice(device_); }

	[[nodiscard]] SDL_GPUCommandBuffer* begin_cmd();

	void end_cmd(SDL_GPUCommandBuffer* cmd);

	SDL_GPUTexture* get_swapchain_texture() const { return swapchain_texture_; }
	SDL_GPUDevice* get_device() const { return device_; }

private:
	SDL_GPUDevice* create_device(SDL_GPUShaderFormat shader_format) const;


	SDL_Window*     window_;
	SDL_GPUDevice*  device_;
	SDL_GPUTexture* swapchain_texture_;

	// constant
	const SDL_GPUShaderFormat shader_format_ = SDL_GPU_SHADERFORMAT_MSL;
	const char*               graphics_api_  = nullptr; // nullptr = automatic by SDL
};
