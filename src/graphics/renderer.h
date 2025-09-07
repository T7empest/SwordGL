//
// Created by Lukáš Blažek on 31.08.2025.
//

#pragma once
#include <memory>

#include "gpu_context.h"
#include "../scenes/scene.h"
#include "SDL3/SDL_gpu.h"
#include <iostream>

#include "../../dependencies/SDL_shadercross/include/SDL3_shadercross/SDL_shadercross.h"
#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

struct ParticleVertex
{
	glm::vec2 pos;
	glm::vec2 uv;
};

struct ParticleInstance
{
	glm::vec2 world_pos;
	glm::vec4 color;
	float       size;
	float       pad;
};

class Renderer
{
public:
	explicit Renderer(GPUContext* gpu_context)
		: gpu_context_(gpu_context)
	{
	}

	~Renderer();
	void           init(SDL_GPUCommandBuffer* cmd);
	void           update();
	void           render(SDL_GPUCommandBuffer* cmd_buf);
	SDL_GPUShader* load_shader(const char* path, SDL_ShaderCross_ShaderStage stage,
	                           uint32_t    num_UBOs);
	void create_pipeline();

private:
	void uploadScene(Scene::Vertex*        scene_vertices,
	                 uint32_t              vertex_count,
	                 SDL_GPUCommandBuffer* cmd);

	GPUContext*              gpu_context_;
	SDL_GPUBuffer*           vertex_buffer_     = nullptr;
	uint32_t                 vertex_count_      = 0;
	SDL_GPUShader*           vertex_shader_     = nullptr;
	SDL_GPUShader*           fragment_shader_   = nullptr;
	SDL_GPUGraphicsPipeline* graphics_pipeline_ = nullptr;

	// particles
	SDL_GPUBuffer* particle_vb_   = nullptr;
	SDL_GPUBuffer* particle_ib_   = nullptr;
	SDL_GPUBuffer* instance_vb_   = nullptr;
	uint32_t       max_particles_ = 10000;

};
