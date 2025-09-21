#version 450

// slot 0 (per-vertex)
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inUV;

// slot 1 (per-instance)
layout(location = 2) in vec2 iWorldPos;
layout(location = 3) in float iSize;
layout(location = 4) in vec4 iColor;

layout(location = 0) out vec2 vUV;
layout(location = 1) out vec4 vColor;

// VS uniformy se pushují přes SDL_PushGPUVertexUniformData(...)
// => MUSÍ být v setu 1, binding = slot (tady 0)
layout(set = 1, binding = 0) uniform UBO {
    mat4 proj;
} u;

void main() {
    vec2 world = iWorldPos + inPos * iSize;   // jednoduchý billboard
    gl_Position = u.proj * vec4(world, 0.0, 1.0);
    vUV    = inUV;
    vColor = iColor;
}