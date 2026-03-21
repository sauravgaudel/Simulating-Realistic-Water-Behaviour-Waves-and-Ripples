#version 330 core
in vec3 vPos; in vec2 vUV; in vec3 vNorm;
out vec4 fragColor;
uniform sampler2D uTileTex;
uniform sampler2D uCausticsTex;
uniform sampler2D uHeightTex;
uniform vec3 uLightDir, uEye;

void main(){
    vec3 N=normalize(vNorm);
    vec3 L=normalize(uLightDir);
    float diff=max(dot(N,L),0.0);

    vec3 tile=texture(uTileTex,vUV).rgb;

    // caustics on floor only
    vec3 caus=vec3(0);
    if(vNorm.y>0.5){
        vec2 cUV=vPos.xz*0.5+0.5;
        caus=texture(uCausticsTex,cUV).rgb;
    }

    // water shadow (depth-based attenuation)
    float depth=clamp((-vPos.y)/1.0,0.0,1.0);
    float shadow=exp(-depth*0.4);

    // ambient occlusion from pool corners
    float ao=smoothstep(0.0,0.6,min(1.0-abs(vPos.x),1.0-abs(vPos.z)));
    ao=mix(0.4,1.0,ao);

    vec3 ambient=vec3(0.05,0.12,0.20);
    vec3 col=tile*(ambient*ao + vec3(0.9,0.95,1.0)*diff*shadow + caus*1.6*shadow);

    // water tint by depth
    col=mix(col,vec3(0.02,0.10,0.25),depth*0.35);

    fragColor=vec4(col,1.0);
}