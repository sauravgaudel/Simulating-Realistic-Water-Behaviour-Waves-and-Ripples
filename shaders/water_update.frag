#version 330 core
in  vec2 vUV;
out vec4 fragColor;

uniform sampler2D uTexture;
uniform float     uTexelSize;
uniform float     uGravity;

void main() {
    float h  = texture(uTexture, vUV).r;
    float v  = texture(uTexture, vUV).g;

    float hN = texture(uTexture, vUV + vec2(0.0,        uTexelSize)).r;
    float hS = texture(uTexture, vUV - vec2(0.0,        uTexelSize)).r;
    float hE = texture(uTexture, vUV + vec2(uTexelSize,  0.0      )).r;
    float hW = texture(uTexture, vUV - vec2(uTexelSize,  0.0      )).r;

    v += (hN + hS + hE + hW - 4.0 * h) * 0.5;
    v *= 0.99;
    v -= h * uGravity;
    h += v;

    float e = uTexelSize * 3.0;
    if (vUV.x < e || vUV.x > 1.0-e || vUV.y < e || vUV.y > 1.0-e) {
        h = 0.0; v = 0.0;
    }

    fragColor = vec4(h, v, 0.0, 1.0);
}