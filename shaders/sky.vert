#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 uView;
uniform mat4 uProj;

out vec3 vTexCoord;

void main() {
    vTexCoord   = aPos;
    vec4 pos    = uProj * uView * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // set z = w so depth = 1.0 always
}
