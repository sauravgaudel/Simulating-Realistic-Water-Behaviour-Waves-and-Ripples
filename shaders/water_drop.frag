#version 330 core
in  vec2 vUV;
out vec4 fragColor;
uniform sampler2D uTexture;
uniform vec2  uCenter;
uniform float uRadius;
uniform float uStrength;

void main() {
    vec4  c = texture(uTexture, vUV);
    float d = distance(vUV, uCenter);
    float t = clamp(1.0 - d / uRadius, 0.0, 1.0);
    t = t * t * (3.0 - 2.0 * t);
    c.r += t * uStrength;
    fragColor = c;
}