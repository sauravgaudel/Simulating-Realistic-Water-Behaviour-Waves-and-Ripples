#version 330 core
in  vec2 vUV;
out vec4 fragColor;
uniform sampler2D uTexture;
uniform float     uTexelSize;

void main() {
    float hN = texture(uTexture, vUV + vec2(0, uTexelSize)).r;
    float hS = texture(uTexture, vUV - vec2(0, uTexelSize)).r;
    float hE = texture(uTexture, vUV + vec2(uTexelSize, 0)).r;
    float hW = texture(uTexture, vUV - vec2(uTexelSize, 0)).r;
    vec3 n = normalize(vec3((hW - hE) * 10.0, 2.0 * uTexelSize, (hS - hN) * 10.0));
    fragColor = vec4(n * 0.5 + 0.5, 1.0);
}