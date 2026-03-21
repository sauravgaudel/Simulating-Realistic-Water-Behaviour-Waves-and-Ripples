#version 330 core
in  vec3 vPos;
in  vec2 vUV;
in  vec3 vNormal;
out vec4 fragColor;

uniform samplerCube uSkybox;
uniform sampler2D   uCausticsTex;
uniform vec3        uLightDir;
uniform vec3        uEye;
uniform vec3        uSpherePos;
uniform float       uSphereR;

bool hitSphere(vec3 ro, vec3 rd, vec3 c, float r, out float t) {
    vec3 oc = ro - c;
    float b = dot(oc, rd);
    float disc = b*b - dot(oc,oc) + r*r;
    if (disc < 0.0) return false;
    t = -b - sqrt(disc);
    return t > 0.001;
}

bool hitFloor(vec3 ro, vec3 rd, out vec2 uv) {
    if (rd.y >= -0.001) return false;
    float t = (-1.0 - ro.y) / rd.y;
    vec3  p = ro + rd * t;
    if (abs(p.x) > 1.0 || abs(p.z) > 1.0) return false;
    uv = p.xz * 0.5 + 0.5;
    return true;
}

void main() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(uEye - vPos);
    vec3 L = normalize(uLightDir);

    // Fresnel
    float F = 0.02 + 0.98 * pow(1.0 - max(dot(N,V), 0.0), 5.0);
    F = clamp(F, 0.04, 1.0);

    // Reflection
    vec3 R   = reflect(-V, N);
    vec3 refl = texture(uSkybox, R).rgb;
    refl += vec3(1.0,0.97,0.88) * pow(max(dot(R,L),0.0), 400.0) * 4.0;

    // Refraction
    vec3 refrDir = refract(-V, N, 1.0/1.333);
    vec3 refr;
    float tS; vec2 flUV;

    if (hitSphere(vPos, refrDir, uSpherePos, uSphereR, tS)) {
        vec3 hp = vPos + refrDir * tS;
        vec3 sN = normalize(hp - uSpherePos);
        float d = max(dot(sN, L), 0.0);
        float s = pow(max(dot(reflect(-L,sN),V),0.0), 80.0);
        refr = vec3(0.92,0.40,0.05)*(0.15+d*0.85) + vec3(0.8)*s;
    } else if (hitFloor(vPos, refrDir, flUV)) {
        vec3  caus  = texture(uCausticsTex, flUV).rgb;
        float cx    = floor(flUV.x * 8.0);
        float cy    = floor(flUV.y * 8.0);
        float tile  = mod(cx+cy, 2.0);
        vec3  col   = mix(vec3(0.68,0.74,0.82), vec3(0.82,0.87,0.95), tile);
        float depth = (-1.0 - vPos.y) / max(-refrDir.y, 0.001);
        vec3  att   = exp(-depth * vec3(0.28,0.12,0.03));
        refr = (col + caus * 1.8) * att;
    } else {
        refr = vec3(0.03, 0.12, 0.28);
    }

    refr = mix(refr, vec3(0.02,0.12,0.26), 0.12);
    vec3 col = mix(refr, refl, F);
    col += vec3(0.5,0.8,1.0) * clamp(vPos.y*8.0, 0.0, 1.0) * 0.10;

    fragColor = vec4(col, mix(0.35, 0.75, F));
}