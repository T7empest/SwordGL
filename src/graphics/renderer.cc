//
// Created by Lukáš Blažek on 31.08.2025.
//

#include "renderer.h"

#include "../../dependencies/SDL_shadercross/include/SDL3_shadercross/SDL_shadercross.h"
#include "SDL3/SDL_log.h"

Renderer::~Renderer()
{
	SDL_ReleaseGPUBuffer(gpu_context_->get_device(), vertex_buffer_);
	SDL_WaitForGPUIdle(gpu_context_->get_device());
	SDL_ReleaseGPUGraphicsPipeline(gpu_context_->get_device(), graphics_pipeline_);
}

void Renderer::init(SDL_GPUCommandBuffer* cmd)
{
	auto scene_vertices = Scene::vertices;
	vertex_count_       = Scene::vertices.size();

	uploadScene(scene_vertices.data(), vertex_count_, cmd);

	vertex_shader_ = load_shader("../res/shaders/vert.spv", SDL_SHADERCROSS_SHADERSTAGE_VERTEX, 0);
	fragment_shader_ = load_shader("../res/shaders/frag.spv", SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT,
	                               1);

	create_pipeline();
}


void Renderer::render(SDL_GPUCommandBuffer* cmd_buf)
{
	auto swapchain_texture = gpu_context_->get_swapchain_texture();


	RenderPassContext ctx = {
		.cmd = cmd_buf,
		.target = swapchain_texture,
		.pipeline = graphics_pipeline_,
		.vertex_buffer = vertex_buffer_,
		.vertex_count = vertex_count_,
		.num_instances = 1
	};

	Scene::UniformBuffer time_uniform{};
	time_uniform.time = SDL_GetTicksNS() / 1e9f;
	SDL_PushGPUFragmentUniformData(cmd_buf, 0, &time_uniform, sizeof(Scene::UniformBuffer));

	present_pass_->execute(ctx);
}

SDL_GPUShader* Renderer::load_shader(const char* path, SDL_ShaderCross_ShaderStage stage,
                                     uint32_t    num_UBOs)
{
	size_t byte_size = 0;
	void*  bytes     = SDL_LoadFile(path, &byte_size);
	std::cout << "bytesize of vertex shader: " << byte_size << std::endl;
	if (!bytes)
		throw std::runtime_error(std::string("failed to load shader file\n") + SDL_GetError());

	SDL_ShaderCross_SPIRV_Info info = {
		.bytecode = static_cast<uint8_t*>(bytes),
		.bytecode_size = byte_size,
		.entrypoint = "main",
		.shader_stage = stage
	};

	SDL_ShaderCross_GraphicsShaderMetadata* metadata = SDL_ShaderCross_ReflectGraphicsSPIRV(
		info.bytecode, info.bytecode_size, 0);

	SDL_GPUShader* shader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(
		gpu_context_->get_device(), &info, metadata, 0);

	SDL_free(metadata);
	SDL_free(bytes);

	return shader;
}

void Renderer::create_pipeline()
{
	SDL_GPUVertexBufferDescription vertex_buffer_descriptions[1];
	vertex_buffer_descriptions[0].slot               = 0;
	vertex_buffer_descriptions[0].input_rate         = SDL_GPU_VERTEXINPUTRATE_VERTEX;
	vertex_buffer_descriptions[0].instance_step_rate = 0;
	vertex_buffer_descriptions[0].pitch              = sizeof(Scene::Vertex);

	SDL_GPUVertexAttribute vertex_attributes[2];
	vertex_attributes[0].buffer_slot = 0;
	vertex_attributes[0].location    = 0;
	vertex_attributes[0].format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
	vertex_attributes[0].offset      = 0;

	vertex_attributes[1].buffer_slot = 0;
	vertex_attributes[1].location    = 1;
	vertex_attributes[1].format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
	vertex_attributes[1].offset      = sizeof(float) * 3;

	SDL_GPUColorTargetDescription color_target_descriptions[1];
	color_target_descriptions[0]        = {};
	color_target_descriptions[0].format = SDL_GetGPUSwapchainTextureFormat(
		gpu_context_->get_device(), gpu_context_->get_window());
	// blending, later need two pipelines:
	// 1. blending off, depth-test on  == opaque pipeline
	// 2. blending on,  depth-test off == transparent pipeline
	color_target_descriptions[0].blend_state.enable_blend          = true;
	color_target_descriptions[0].blend_state.color_blend_op        = SDL_GPU_BLENDOP_ADD;
	color_target_descriptions[0].blend_state.alpha_blend_op        = SDL_GPU_BLENDOP_ADD;
	color_target_descriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
	color_target_descriptions[0].blend_state.dst_color_blendfactor =
		SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
	color_target_descriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
	color_target_descriptions[0].blend_state.dst_alpha_blendfactor =
		SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

	SDL_GPUGraphicsPipelineCreateInfo create_info = {
		.vertex_shader = vertex_shader_,
		.fragment_shader = fragment_shader_,
		.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		.vertex_input_state.num_vertex_buffers = 1,
		.vertex_input_state.vertex_buffer_descriptions = vertex_buffer_descriptions,
		.vertex_input_state.num_vertex_attributes = 2,
		.vertex_input_state.vertex_attributes = vertex_attributes,
		.target_info.num_color_targets = 1,
		.target_info.color_target_descriptions = color_target_descriptions
	};

	graphics_pipeline_ = SDL_CreateGPUGraphicsPipeline(gpu_context_->get_device(), &create_info);

	// release shaders, not needed anymore
	SDL_ReleaseGPUShader(gpu_context_->get_device(), vertex_shader_);
	SDL_ReleaseGPUShader(gpu_context_->get_device(), fragment_shader_);
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
