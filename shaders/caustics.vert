#version 330 core
layout(location=0) in vec2 aXZ;
layout(location=1) in vec2 aUV;
uniform sampler2D uHeightTex;
uniform vec3 uLightDir;
uniform float uTexelSize;
out float vIntensity;

void main(){
    float h =texture(uHeightTex,aUV).r;
    float hN=texture(uHeightTex,aUV+vec2(0,uTexelSize)).r;
    float hS=texture(uHeightTex,aUV-vec2(0,uTexelSize)).r;
    float hE=texture(uHeightTex,aUV+vec2(uTexelSize,0)).r;
    float hW=texture(uHeightTex,aUV-vec2(uTexelSize,0)).r;

    vec3 N=normalize(vec3((hW-hE)*10.0, 2.0*uTexelSize, (hS-hN)*10.0));
    vec3 incident=normalize(-uLightDir);
    vec3 refr=refract(incident,N,1.0/1.333);
    if(length(refr)<0.001) refr=incident;

    vec3 wPos=vec3(aXZ.x, h*0.5, aXZ.y);
    float t=(-1.0-wPos.y)/refr.y;
    vec3 floor=wPos+refr*max(t,0.0);

    vec3 dx=dFdx(floor), dz=dFdy(floor);
    vec3 ox=dFdx(wPos),  oz=dFdy(wPos);
    float oldA=length(cross(ox,oz));
    float newA=length(cross(dx,dz));
    vIntensity=clamp(oldA/max(newA,1e-6)*0.4, 0.0, 3.0);

    gl_Position=vec4(floor.x, floor.z, 0.0, 1.0);
}