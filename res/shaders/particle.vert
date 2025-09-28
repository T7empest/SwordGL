#version 450

// slot 0 (quad)
layout(location=0) in vec2 inPos;      // [-0.5, 0.5]
layout(location=1) in vec2 inUV;       // 0..1

// slot 1 (instance)
layout(location=2) in vec2 iWorldPos;  // pixel space
layout(location=3) in float iSize;     // pixels
layout(location=4) in vec4  iColor;    // rgba
layout(location=5) in float iAge01;    // 0..1 (age/life)

layout(location=0) out vec2 vUV;
layout(location=1) out flat vec4 vColor;   // flat = bez interpolace
layout(location=2) out flat uint vSeed;    // seed per instance
layout(location=3) out flat float vAge01;  // 0..1

layout(set=1, binding=0) uniform OrthoUBO {
    mat4 proj;
} U;

void main(){
    vUV    = inUV;
    vColor = iColor;
    vSeed  = gl_InstanceIndex;
    vAge01 = iAge01;

    vec2 posPx = iWorldPos + inPos * iSize;     // billboard v px
    gl_Position = U.proj * vec4(posPx, 0.0, 1.0);
}
