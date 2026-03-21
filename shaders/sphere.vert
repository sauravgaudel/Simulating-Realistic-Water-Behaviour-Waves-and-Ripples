#version 330 core
layout(location=0) in vec3 aPos;
uniform mat4 uMVP, uModel;
out vec3 vWorldPos, vNormal;
void main(){
    vWorldPos=vec3(uModel*vec4(aPos,1));
    vNormal=normalize(mat3(uModel)*aPos);
    gl_Position=uMVP*vec4(aPos,1);
}