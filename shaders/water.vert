#version 330 core
layout(location=0) in vec2 aXZ;
layout(location=1) in vec2 aUV;
uniform sampler2D uHeightTex;
uniform sampler2D uNormalTex;
uniform mat4      uVP;
out vec3 vPos;
out vec2 vUV;
out vec3 vNormal;

void main() {
    float h  = texture(uHeightTex, aUV).r;
    vec3  n  = texture(uNormalTex, aUV).rgb * 2.0 - 1.0;
    vNormal  = normalize(n);
    vPos     = vec3(aXZ.x, h * 0.5, aXZ.y);
    vUV      = aUV;
    gl_Position = uVP * vec4(vPos, 1.0);
}