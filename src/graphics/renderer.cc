//
// Created by Lukáš Blažek on 31.08.2025.
//

#include "renderer.h"

#include "SDL3/SDL.h"
#include "SDL_shadercross/include/SDL3_shadercross/SDL_shadercross.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "graphics/UI.h"
#include "graphics/render_passes/copy_pass.h"
#include "graphics/render_passes/present_pass.h"

Renderer::~Renderer()
{
    SDL_WaitForGPUIdle(gpu_context_->get_device());
    SDL_ReleaseGPUBuffer(gpu_context_->get_device(), vertex_buffer_);
    SDL_ReleaseGPUGraphicsPipeline(gpu_context_->get_device(),
                                   graphics_pipeline_);
    SDL_ReleaseGPUShader(gpu_context_->get_device(), vertex_shader_);
    SDL_ReleaseGPUShader(gpu_context_->get_device(), fragment_shader_);
    SDL_ReleaseGPUShader(gpu_context_->get_device(), particle_fs_);
    SDL_ReleaseGPUShader(gpu_context_->get_device(), particle_vs_);

    UI::instance().shutdown();
}

void Renderer::init(SDL_GPUCommandBuffer *cmd)
{
    UI::instance().init(gpu_context_->get_window(), gpu_context_->get_device());

    auto scene_vertices = Scene::vertices;
    vertex_count_ = Scene::vertices.size();

    uploadScene(scene_vertices.data(), vertex_count_, cmd);

    vertex_shader_ = load_shader("../res/shaders/shader.vert.spv",
                                 SDL_SHADERCROSS_SHADERSTAGE_VERTEX, 0);
    fragment_shader_ = load_shader("../res/shaders/shader.frag.spv",
                                   SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT, 1);
    particle_vs_ = load_shader("../res/shaders/particle.vert.spv",
                               SDL_SHADERCROSS_SHADERSTAGE_VERTEX, 0);
    particle_fs_ = load_shader("../res/shaders/particle.frag.spv",
                               SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT, 0);

    create_pipeline();

    create_particle_pipeline();

    // particles
    constexpr ParticleVertex quad[4] = {
        {{-0.5f, -0.5f}, {0, 0}},
        {{0.5f, -0.5f}, {1, 0}},
        {{0.5f, 0.5f}, {1, 1}},
        {{-0.5f, 0.5f}, {0, 1}},
    };
    const uint16_t quad_idx[6] = {0, 1, 2, 0, 2, 3};

    SDL_GPUBufferCreateInfo vb_info{.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
                                    .size = sizeof(quad)};
    particle_vb_ = SDL_CreateGPUBuffer(gpu_context_->get_device(), &vb_info);

    SDL_GPUBufferCreateInfo ib_info{.usage = SDL_GPU_BUFFERUSAGE_INDEX,
                                    .size = sizeof(quad_idx)};
    particle_ib_ = SDL_CreateGPUBuffer(gpu_context_->get_device(), &ib_info);

    CopyPass::Context ctx_vb = {.dst = particle_vb_,
                                .dst_offset = 0,
                                .src = quad,
                                .size_bytes = sizeof(quad)};
    CopyPass::enqueue_upload(gpu_context_->get_device(), cmd, ctx_vb);

    CopyPass::Context ctx_ib = {.dst = particle_ib_,
                                .dst_offset = 0,
                                .src = quad_idx,
                                .size_bytes = sizeof(quad_idx)};
    CopyPass::enqueue_upload(gpu_context_->get_device(), cmd, ctx_ib);

    SDL_GPUBufferCreateInfo inst_info = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size =
            static_cast<uint32_t>(max_particles_ * sizeof(ParticleInstance)),
    };
    instance_vb_ = SDL_CreateGPUBuffer(gpu_context_->get_device(), &inst_info);
}

void Renderer::update(SDL_GPUCommandBuffer *cmd,
                      std::shared_ptr<ParticleSystem> particle_system)
{
    auto *particles = particle_system->get_particles();
    const auto num_particles = particles->size();
    if (num_particles == 0)
    {
        particle_count_ = 0;
        return;
    }

    const size_t count = std::min((uint32_t)num_particles, max_particles_);

    std::vector<ParticleInstance> instances(count);
    for (size_t i = 0; i < count; ++i)
    {
        const Particle &p = (*particles)[i];
        instances[i].world_pos = p.position;
        instances[i].size = p.scale;
        instances[i].color = p.color;
        instances[i].remaining_life = p.lifetime;
    }

    CopyPass::Context ctx = {.dst = instance_vb_,
                             .dst_offset = 0,
                             .src = instances.data(),
                             .size_bytes = count * sizeof(ParticleInstance)};
    CopyPass::enqueue_upload(gpu_context_->get_device(), cmd, ctx);

    int W, H;
    SDL_GetWindowSizeInPixels(gpu_context_->get_window(), &W, &H);
    float mx, my;
    SDL_GetMouseState(&mx, &my);

    if (!instances.empty())
    {
        instances[0].world_pos = {(float)mx, (float)my};
        instances[0].size = 64.0f;
        instances[0].color = {1, 1, 1, 1};
    }

    particle_count_ = (uint32_t)count;
}

void Renderer::render(SDL_GPUCommandBuffer *cmd_buf)
{
    auto swapchain_texture = gpu_context_->get_swapchain_texture();
    int W, H;
    SDL_GetWindowSizeInPixels(gpu_context_->get_window(), &W, &H);

    // FS uniform (time) - set=3, binding=0
    // TODO: handle unfiroms elsewhere
    Scene::UniformBuffer uniform_buffer{};
    uniform_buffer.resolution = glm::vec2(W, H);
    uniform_buffer.time = static_cast<float>(SDL_GetTicksNS()) / 1e9f;
    uniform_buffer._pad = 0.0f;

    SDL_PushGPUFragmentUniformData(cmd_buf, 0, &uniform_buffer,
                                   sizeof(Scene::UniformBuffer));

    // glm::ortho(left, right, bottom, top, zNear, zFar)
    glm::mat4 proj = glm::ortho(0.0f, (float)W, (float)H, 0.0f, -1.0f, 1.0f);
    struct OrthoUBO
    {
        glm::mat4 proj;
    } ubo{proj};
    SDL_PushGPUVertexUniformData(cmd_buf, /*slot*/ 0, &ubo,
                                 sizeof(ubo)); // -> set=1,binding=0

    // triangle pass
    {
        RenderPassContext ctx{};
        ctx.cmd = cmd_buf;
        ctx.target = swapchain_texture;
        ctx.pipeline = graphics_pipeline_;

        ctx.vertex_buffers[0] = vertex_buffer_;
        ctx.vertex_offsets[0] = 0;
        ctx.num_vertex_buffers = 1;

        ctx.vertex_count = vertex_count_;
        ctx.num_instances = 1;

        ctx.clear = true;
        ctx.clear_color = {0.f, 0.f, 0.f, 1.f};

        PresentPass::execute(ctx);
    }

    // particle pass
    {
        RenderPassContext ctx{};
        ctx.cmd = cmd_buf;
        ctx.target = swapchain_texture;
        ctx.pipeline = particle_pipeline_; // ✅ správná pipeline

        ctx.vertex_buffers[0] = particle_vb_;
        ctx.vertex_offsets[0] = 0;
        ctx.vertex_buffers[1] = instance_vb_;
        ctx.vertex_offsets[1] = 0;
        ctx.num_vertex_buffers = 2;

        ctx.index_buffer = particle_ib_; // ✅ indexovaný draw
        ctx.index_offset = 0;
        ctx.index_type = SDL_GPU_INDEXELEMENTSIZE_16BIT;
        ctx.index_count = 6;

        ctx.num_instances = particle_count_;
        ctx.clear = false; // nenečistit, triangle zůstane pod tím

        PresentPass::execute(ctx);
    }

    // imgui pass
    ImguiPassContext imgui_ctx = {cmd_buf, swapchain_texture};
    UI::instance().render(imgui_ctx);
}

SDL_GPUShader *Renderer::load_shader(const char *path,
                                     SDL_ShaderCross_ShaderStage stage,
                                     uint32_t num_UBOs)
{
    size_t byte_size = 0;
    void *bytes = SDL_LoadFile(path, &byte_size);
    std::cout << "bytesize of vertex shader: " << byte_size << std::endl;
    if (!bytes)
        throw std::runtime_error(std::string("failed to load shader file\n") +
                                 SDL_GetError());

    SDL_ShaderCross_SPIRV_Info info = {.bytecode =
                                           static_cast<uint8_t *>(bytes),
                                       .bytecode_size = byte_size,
                                       .entrypoint = "main",
                                       .shader_stage = stage};

    SDL_ShaderCross_GraphicsShaderMetadata *metadata =
        SDL_ShaderCross_ReflectGraphicsSPIRV(info.bytecode, info.bytecode_size,
                                             0);
    SDL_ShaderCross_GraphicsShaderResourceInfo resource_info = {
        .num_samplers = 0,
        .num_storage_textures = 0,
        .num_storage_buffers = 0,
        .num_uniform_buffers = 1};

    SDL_GPUShader *shader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(
        gpu_context_->get_device(), &info, &resource_info, 0);

    if (!shader)
    {
        std::string err = SDL_GetError(); // ShaderCross chybová hláška
        SDL_free(metadata);

        SDL_free(bytes);
        throw std::runtime_error(std::string("Shader compile failed: ") + path +
                                 " :: " + err);
    }

    SDL_free(metadata);
    SDL_free(bytes);

    return shader;
}

void Renderer::create_pipeline()
{
    SDL_GPUVertexBufferDescription vertex_buffer_descriptions[1];
    vertex_buffer_descriptions[0].slot = 0;
    vertex_buffer_descriptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertex_buffer_descriptions[0].instance_step_rate = 0;
    vertex_buffer_descriptions[0].pitch = sizeof(Scene::Vertex);

    SDL_GPUVertexAttribute vertex_attributes[2];
    vertex_attributes[0].buffer_slot = 0;
    vertex_attributes[0].location = 0;
    vertex_attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    vertex_attributes[0].offset = 0;

    vertex_attributes[1].buffer_slot = 0;
    vertex_attributes[1].location = 1;
    vertex_attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    vertex_attributes[1].offset = sizeof(float) * 3;

    SDL_GPUColorTargetDescription color_target_descriptions[1];
    color_target_descriptions[0] = {};
    color_target_descriptions[0].format = SDL_GetGPUSwapchainTextureFormat(
        gpu_context_->get_device(), gpu_context_->get_window());
    // blending, later need two pipelines:
    // 1. blending off, depth-test on  == opaque pipeline
    // 2. blending on,  depth-test off == transparent pipeline
    color_target_descriptions[0].blend_state.enable_blend = true;
    color_target_descriptions[0].blend_state.color_blend_op =
        SDL_GPU_BLENDOP_ADD;
    color_target_descriptions[0].blend_state.alpha_blend_op =
        SDL_GPU_BLENDOP_ADD;
    color_target_descriptions[0].blend_state.src_color_blendfactor =
        SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    color_target_descriptions[0].blend_state.dst_color_blendfactor =
        SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    color_target_descriptions[0].blend_state.src_alpha_blendfactor =
        SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    color_target_descriptions[0].blend_state.dst_alpha_blendfactor =
        SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

    SDL_GPUGraphicsPipelineCreateInfo create_info = {
        .vertex_shader = vertex_shader_,
        .fragment_shader = fragment_shader_,
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .vertex_input_state.num_vertex_buffers = 1,
        .vertex_input_state.vertex_buffer_descriptions =
            vertex_buffer_descriptions,
        .vertex_input_state.num_vertex_attributes = 2,
        .vertex_input_state.vertex_attributes = vertex_attributes,
        .target_info.num_color_targets = 1,
        .target_info.color_target_descriptions = color_target_descriptions,
        .rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE};

    graphics_pipeline_ =
        SDL_CreateGPUGraphicsPipeline(gpu_context_->get_device(), &create_info);

    // release shaders, not needed anymore
    SDL_ReleaseGPUShader(gpu_context_->get_device(), vertex_shader_);
    SDL_ReleaseGPUShader(gpu_context_->get_device(), fragment_shader_);
}

void Renderer::create_particle_pipeline()
{
    SDL_GPUVertexBufferDescription vbs[2]{};
    vbs[0].slot = 0;
    vbs[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vbs[0].pitch = sizeof(ParticleVertex);
    vbs[0].instance_step_rate = 0;

    vbs[1].slot = 1;
    vbs[1].input_rate = SDL_GPU_VERTEXINPUTRATE_INSTANCE;
    vbs[1].instance_step_rate = 0;
    vbs[1].pitch = sizeof(ParticleInstance);

    SDL_GPUVertexAttribute attrs[] = {
        // slot 0 (quad)
        {.buffer_slot = 0,
         .location = 0,
         .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
         .offset = 0},
        {.buffer_slot = 0,
         .location = 1,
         .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
         .offset = sizeof(float) * 2},

        // slot 1 (instance) — POZOR: offsetof!
        {.buffer_slot = 1,
         .location = 2,
         .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
         .offset = (uint32_t)offsetof(ParticleInstance, world_pos)},

        {.buffer_slot = 1,
         .location = 3,
         .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT,
         .offset = (uint32_t)offsetof(ParticleInstance, size)},

        {.buffer_slot = 1,
         .location = 4,
         .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
         .offset = (uint32_t)offsetof(ParticleInstance, color)},

        {.buffer_slot = 1,
         .location = 5,
         .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT,
         .offset = (uint32_t)offsetof(ParticleInstance, remaining_life)},

    };

    SDL_GPUColorTargetDescription ct = {
        .format = SDL_GetGPUSwapchainTextureFormat(gpu_context_->get_device(),
                                                   gpu_context_->get_window()),
        .blend_state.enable_blend = true,
        .blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD,
        .blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD,
        .blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
        .blend_state.dst_color_blendfactor =
            SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        .blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
        .blend_state.dst_alpha_blendfactor =
            SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA};

    SDL_GPUGraphicsPipelineCreateInfo ci = {
        .vertex_shader = particle_vs_,
        .fragment_shader = particle_fs_,
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,

        .vertex_input_state.num_vertex_buffers = 2,
        .vertex_input_state.vertex_buffer_descriptions = vbs,
        .vertex_input_state.num_vertex_attributes = (uint32_t)std::size(attrs),
        .vertex_input_state.vertex_attributes = attrs,

        .target_info.num_color_targets = 1,
        .target_info.color_target_descriptions = &ct,

        .depth_stencil_state.enable_depth_test = false,
        .depth_stencil_state.enable_depth_write = false};

    // for test
    ci.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;

    particle_pipeline_ =
        SDL_CreateGPUGraphicsPipeline(gpu_context_->get_device(), &ci);
}

void Renderer::uploadScene(Scene::Vertex *scene_vertices, uint32_t vertex_count,
                           SDL_GPUCommandBuffer *cmd)
{
    auto vb_bytesize =
        static_cast<uint32_t>(vertex_count * sizeof(Scene::Vertex));
    SDL_GPUDevice *device = gpu_context_->get_device();

    SDL_GPUBufferCreateInfo buffer_info = {.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
                                           .size = vb_bytesize};

    vertex_buffer_ = SDL_CreateGPUBuffer(device, &buffer_info);

    CopyPass::Context ctx = {.dst = vertex_buffer_,
                             .dst_offset = 0,
                             .src = scene_vertices,
                             .size_bytes = vb_bytesize};

    auto transfer_buffer = CopyPass::enqueue_upload(device, cmd, ctx);
    // TODO: make deferred
    SDL_WaitForGPUIdle(device);
    SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
}
