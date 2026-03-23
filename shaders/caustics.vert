#version 330 core
layout(location=0) in vec2 aXZ;
layout(location=1) in vec2 aUV;

uniform sampler2D uHeightTex;
uniform vec3 uLightDir;
uniform float uTexelSize;

out float vIntensity;

void main(){
    float h  = texture(uHeightTex, aUV).r;
    float hN = texture(uHeightTex, aUV + vec2(0.0, uTexelSize)).r;
    float hS = texture(uHeightTex, aUV - vec2(0.0, uTexelSize)).r;
    float hE = texture(uHeightTex, aUV + vec2(uTexelSize, 0.0)).r;
    float hW = texture(uHeightTex, aUV - vec2(uTexelSize, 0.0)).r;

    // Surface normal from height differences
    vec3 N = normalize(vec3((hW - hE) * 10.0, 2.0 * uTexelSize, (hS - hN) * 10.0));

    // Refract light through water surface
    vec3 incident = normalize(-uLightDir);
    vec3 refr = refract(incident, N, 1.0 / 1.333);
    if (length(refr) < 0.001) refr = incident;

    // Water surface position
    vec3 wPos = vec3(aXZ.x, h * 0.5, aXZ.y);

    // March refracted ray to pool floor Y=-1
    float t = (-1.0 - wPos.y) / refr.y;
    vec3 floorHit = wPos + refr * max(t, 0.0);

    // Caustic intensity using neighbour comparison instead of dFdx/dFdy
    // Compare area of original patch vs refracted patch using adjacent vertices
    float hN2 = texture(uHeightTex, aUV + vec2(0.0, uTexelSize*2.0)).r;
    float hE2 = texture(uHeightTex, aUV + vec2(uTexelSize*2.0, 0.0)).r;

    vec3 wPos2N = vec3(aXZ.x, hN2*0.5, aXZ.y + uTexelSize*2.0*2.0);
    vec3 wPos2E = vec3(aXZ.x + uTexelSize*2.0*2.0, hE2*0.5, aXZ.y);

    vec3 N2 = normalize(vec3(
        (texture(uHeightTex, aUV+vec2(0,uTexelSize)).r -
         texture(uHeightTex, aUV-vec2(0,uTexelSize)).r)*10.0,
        2.0*uTexelSize,
        (texture(uHeightTex, aUV-vec2(uTexelSize,0)).r -
         texture(uHeightTex, aUV+vec2(uTexelSize,0)).r)*10.0));

    vec3 refrN = refract(incident, N2, 1.0/1.333);
    if(length(refrN)<0.001) refrN=incident;
    float tN = (-1.0 - wPos2N.y) / refrN.y;
    vec3 floorN = wPos2N + refrN * max(tN, 0.0);

    vec3 refrE = refract(incident, N2, 1.0/1.333);
    if(length(refrE)<0.001) refrE=incident;
    float tE = (-1.0 - wPos2E.y) / refrE.y;
    vec3 floorE = wPos2E + refrE * max(tE, 0.0);

    // Area ratio = old area / new area
    vec3 oldEdge1 = wPos2N - wPos;
    vec3 oldEdge2 = wPos2E - wPos;
    float oldArea = length(cross(oldEdge1, oldEdge2));

    vec3 newEdge1 = floorN - floorHit;
    vec3 newEdge2 = floorE - floorHit;
    float newArea = length(cross(newEdge1, newEdge2));

    vIntensity = clamp(oldArea / max(newArea, 1e-6) * 0.4, 0.0, 3.0);

    // Output floor position as screen coords for caustics texture
    gl_Position = vec4(floorHit.x, floorHit.z, 0.0, 1.0);
}