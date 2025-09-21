#version 450
layout(location = 0) in vec2 vUV;
layout(location = 1) in vec4 vColor;
layout(location = 0) out vec4 outColor;

/*
layout(set = 3, binding = 0) uniform TimeUBO {
    float time;
} t;
*/

void main() {
    outColor = vColor; // texturu přidáme později
}