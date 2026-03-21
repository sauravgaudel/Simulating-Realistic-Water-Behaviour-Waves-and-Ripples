#version 330 core
in vec3 vWorldPos, vNormal;
out vec4 fragColor;
uniform samplerCube uSkybox;
uniform vec3 uLightDir, uEye;

void main(){
    vec3 N=normalize(vNormal);
    vec3 V=normalize(uEye-vWorldPos);
    vec3 L=normalize(uLightDir);
    vec3 H=normalize(L+V);

    float diff=max(dot(N,L),0.0);
    float spec=pow(max(dot(N,H),0.0),200.0);

    float F=0.05+0.95*pow(1.0-max(dot(N,V),0.0),5.0);
    vec3 refl=texture(uSkybox,reflect(-V,N)).rgb;

    vec3 base=vec3(0.9,0.38,0.06);
    vec3 col=base*(vec3(0.1,0.12,0.15)+diff*vec3(1.0,0.95,0.85))
             +vec3(spec)
             +refl*F*0.6;
    fragColor=vec4(col,1.0);
}