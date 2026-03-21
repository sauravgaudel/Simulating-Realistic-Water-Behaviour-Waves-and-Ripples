#version 330 core
in  vec2 vUV;
out vec4 fragColor;

uniform sampler2D uTexture;
uniform vec2      uSphereUV;
uniform float     uSphereRadius;
uniform float     uSphereScale;

void main() {
    vec4  cur  = texture(uTexture, vUV);
    float h    = cur.r;
    float v    = cur.g;

    vec2  d    = vUV - uSphereUV;
    float dist = length(d);

    if (dist < uSphereRadius) {
        // compute the sphere surface height at this UV point
        float normDist = dist / uSphereRadius;
        float sphereH  = sqrt(max(0.0, 1.0 - normDist * normDist));

        // the sphere pushes water down to its surface level
        float targetH = -sphereH / uSphereScale;

        // if water is above sphere surface, push it down and create velocity
        if (h > targetH) {
            float push = (h - targetH) * 0.3;
            v -= push;   // push velocity downward
            h  = targetH;
        }
    }

    fragColor = vec4(h, v, 0.0, 1.0);
}