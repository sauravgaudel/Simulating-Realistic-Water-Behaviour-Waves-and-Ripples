#version 330 core
in float vIntensity;
out vec4 fragColor;
void main(){
    float c=vIntensity;
    fragColor=vec4(c*0.6, c*0.8, c*1.0, 1.0);
}