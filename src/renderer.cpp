#include "renderer.h"
#include <vector>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ── Procedural textures ─────────────────────────────────────────────
static GLuint makeTile() {
    const int S=512; std::vector<unsigned char> d(S*S*3);
    for(int y=0;y<S;y++) for(int x=0;x<S;x++){
        float fx=fmodf(x*8.f/S,1.f), fy=fmodf(y*8.f/S,1.f);
        int tx=(x*8)/S, ty=(y*8)/S;
        bool grout = fx<0.05f||fx>0.95f||fy<0.05f||fy>0.95f;
        unsigned char r,g,b;
        if(grout){r=180;g=178;b=172;}
        else{
            float base=(((tx+ty)&1)?0.82f:0.74f);
            r=(unsigned char)(base*205);
            g=(unsigned char)(base*215);
            b=(unsigned char)(base*230);
        }
        int i=(y*S+x)*3; d[i]=r;d[i+1]=g;d[i+2]=b;
    }
    GLuint t; glGenTextures(1,&t); glBindTexture(GL_TEXTURE_2D,t);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,S,S,0,GL_RGB,GL_UNSIGNED_BYTE,d.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    return t;
}

static GLuint makeSky() {
    const int S=256; GLuint t; glGenTextures(1,&t);
    glBindTexture(GL_TEXTURE_CUBE_MAP,t);
    std::vector<unsigned char> img(S*S*3);
    glm::vec3 sun=glm::normalize(glm::vec3(0.6f,0.8f,0.4f));
    for(int f=0;f<6;f++){
        for(int j=0;j<S;j++) for(int i=0;i<S;i++){
            float s=(i+.5f)/S*2-1, tt=(j+.5f)/S*2-1;
            float nx=0,ny=0,nz=0;
            if(f==0){nx=1;ny=-tt;nz=-s;}
            else if(f==1){nx=-1;ny=-tt;nz=s;}
            else if(f==2){nx=s;ny=1;nz=tt;}
            else if(f==3){nx=s;ny=-1;nz=-tt;}
            else if(f==4){nx=s;ny=-tt;nz=1;}
            else{nx=-s;ny=-tt;nz=-1;}
            glm::vec3 dir=glm::normalize(glm::vec3(nx,ny,nz));
            float r,g,b;
            if(dir.y>0){
                float u=dir.y;
                r=0.40f-u*0.25f; g=0.60f-u*0.15f; b=0.90f+u*0.08f;
                float sd=glm::dot(dir,sun);
                if(sd>0.9998f){r=1;g=1;b=0.95f;}
                else if(sd>0.998f){float a=(sd-0.998f)/0.0018f;r+=a*(1-r);g+=a*(1-g);b+=a*(0.95f-b);}
            } else {
                r=0.72f;g=0.68f;b=0.58f;
            }
            r=glm::clamp(r,0.f,1.f);g=glm::clamp(g,0.f,1.f);b=glm::clamp(b,0.f,1.f);
            int idx=(j*S+i)*3;
            img[idx]=(unsigned char)(r*255);img[idx+1]=(unsigned char)(g*255);img[idx+2]=(unsigned char)(b*255);
        }
        GLenum faces[6]={GL_TEXTURE_CUBE_MAP_POSITIVE_X,GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                         GL_TEXTURE_CUBE_MAP_POSITIVE_Y,GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                         GL_TEXTURE_CUBE_MAP_POSITIVE_Z,GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
        glTexImage2D(faces[f],0,GL_RGB,S,S,0,GL_RGB,GL_UNSIGNED_BYTE,img.data());
    }
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
    return t;
}

// ── Mesh builders ────────────────────────────────────────────────────
static void buildGrid(int N,GLuint&vao,GLuint&vbo,GLuint&ebo,int&cnt){
    std::vector<float> v; std::vector<unsigned int> idx;
    for(int j=0;j<=N;j++) for(int i=0;i<=N;i++){
        float u=(float)i/N, w=(float)j/N;
        v.push_back(u*2-1); v.push_back(w*2-1);
        v.push_back(u);     v.push_back(w);
    }
    for(int j=0;j<N;j++) for(int i=0;i<N;i++){
        unsigned a=j*(N+1)+i,b=a+1,c=a+(N+1),d=c+1;
        idx.push_back(a);idx.push_back(b);idx.push_back(c);
        idx.push_back(b);idx.push_back(d);idx.push_back(c);
    }
    cnt=(int)idx.size();
    glGenVertexArrays(1,&vao);glGenBuffers(1,&vbo);glGenBuffers(1,&ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,v.size()*4,v.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,idx.size()*4,idx.data(),GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);glVertexAttribPointer(0,2,GL_FLOAT,0,4*4,(void*)0);
    glEnableVertexAttribArray(1);glVertexAttribPointer(1,2,GL_FLOAT,0,4*4,(void*)(2*4));
    glBindVertexArray(0);
}

static void buildPool(GLuint&vao,GLuint&vbo){
    // pos(3) uv(2) normal(3) = 8 floats per vertex
    static const float D[]={
        // floor  Y=-1  normal=(0,1,0)
        -1,-1,-1, 0,0, 0,1,0,
         1,-1,-1, 4,0, 0,1,0,
         1,-1, 1, 4,4, 0,1,0,
        -1,-1,-1, 0,0, 0,1,0,
         1,-1, 1, 4,4, 0,1,0,
        -1,-1, 1, 0,4, 0,1,0,
        // back  Z=-1  normal=(0,0,1)
        -1,-1,-1, 0,0, 0,0,1,
         1,-1,-1, 2,0, 0,0,1,
         1, 0,-1, 2,1, 0,0,1,
        -1,-1,-1, 0,0, 0,0,1,
         1, 0,-1, 2,1, 0,0,1,
        -1, 0,-1, 0,1, 0,0,1,
        // front Z=+1  normal=(0,0,-1)
         1,-1, 1, 0,0, 0,0,-1,
        -1,-1, 1, 2,0, 0,0,-1,
        -1, 0, 1, 2,1, 0,0,-1,
         1,-1, 1, 0,0, 0,0,-1,
        -1, 0, 1, 2,1, 0,0,-1,
         1, 0, 1, 0,1, 0,0,-1,
        // left  X=-1  normal=(1,0,0)
        -1,-1, 1, 0,0, 1,0,0,
        -1,-1,-1, 2,0, 1,0,0,
        -1, 0,-1, 2,1, 1,0,0,
        -1,-1, 1, 0,0, 1,0,0,
        -1, 0,-1, 2,1, 1,0,0,
        -1, 0, 1, 0,1, 1,0,0,
        // right X=+1  normal=(-1,0,0)
         1,-1,-1, 0,0, -1,0,0,
         1,-1, 1, 2,0, -1,0,0,
         1, 0, 1, 2,1, -1,0,0,
         1,-1,-1, 0,0, -1,0,0,
         1, 0, 1, 2,1, -1,0,0,
         1, 0,-1, 0,1, -1,0,0,
    };
    glGenVertexArrays(1,&vao);glGenBuffers(1,&vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(D),D,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);glVertexAttribPointer(0,3,GL_FLOAT,0,8*4,(void*)0);
    glEnableVertexAttribArray(1);glVertexAttribPointer(1,2,GL_FLOAT,0,8*4,(void*)(3*4));
    glEnableVertexAttribArray(2);glVertexAttribPointer(2,3,GL_FLOAT,0,8*4,(void*)(5*4));
    glBindVertexArray(0);
}

static void buildSphere(int stk,int sli,GLuint&vao,GLuint&vbo,GLuint&ebo,int&cnt){
    std::vector<float> v; std::vector<unsigned int> idx;
    for(int i=0;i<=stk;i++){
        float phi=(float)M_PI*i/stk;
        for(int j=0;j<=sli;j++){
            float th=2*(float)M_PI*j/sli;
            float x=sinf(phi)*cosf(th),y=cosf(phi),z=sinf(phi)*sinf(th);
            v.push_back(x);v.push_back(y);v.push_back(z);
        }
    }
    for(int i=0;i<stk;i++) for(int j=0;j<sli;j++){
        unsigned a=i*(sli+1)+j,b=a+sli+1;
        idx.push_back(a);idx.push_back(b);idx.push_back(a+1);
        idx.push_back(b);idx.push_back(b+1);idx.push_back(a+1);
    }
    cnt=(int)idx.size();
    glGenVertexArrays(1,&vao);glGenBuffers(1,&vbo);glGenBuffers(1,&ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,v.size()*4,v.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,idx.size()*4,idx.data(),GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);glVertexAttribPointer(0,3,GL_FLOAT,0,3*4,(void*)0);
    glBindVertexArray(0);
}

static void buildSkybox(GLuint&vao,GLuint&vbo){
    static const float v[]={
        -1,1,-1,-1,-1,-1,1,-1,-1,1,-1,-1,1,1,-1,-1,1,-1,
        -1,-1,1,-1,-1,-1,-1,1,-1,-1,1,-1,-1,1,1,-1,-1,1,
        1,-1,-1,1,-1,1,1,1,1,1,1,1,1,1,-1,1,-1,-1,
        -1,-1,1,-1,1,1,1,1,1,1,1,1,1,-1,1,-1,-1,1,
        -1,1,-1,1,1,-1,1,1,1,1,1,1,-1,1,1,-1,1,-1,
        -1,-1,-1,-1,-1,1,1,-1,-1,1,-1,-1,-1,-1,1,1,-1,1
    };
    glGenVertexArrays(1,&vao);glGenBuffers(1,&vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(v),v,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);glVertexAttribPointer(0,3,GL_FLOAT,0,3*4,(void*)0);
    glBindVertexArray(0);
}

// ── Renderer ─────────────────────────────────────────────────────────
Renderer::Renderer(){
    buildShaders(); buildMeshes(); buildTextures(); buildCausticsFBO();
}
Renderer::~Renderer(){
    glDeleteProgram(m_progPool);glDeleteProgram(m_progWater);
    glDeleteProgram(m_progSphere);glDeleteProgram(m_progSky);
    glDeleteProgram(m_progCaustics);
    glDeleteTextures(1,&m_tileTex);glDeleteTextures(1,&m_skyTex);
    glDeleteFramebuffers(1,&m_causFBO);glDeleteTextures(1,&m_causTex);
}

void Renderer::buildShaders(){
    m_progPool    =buildProgram("shaders/pool.vert",    "shaders/pool.frag");
    m_progWater   =buildProgram("shaders/water.vert",   "shaders/water.frag");
    m_progSphere  =buildProgram("shaders/sphere.vert",  "shaders/sphere.frag");
    m_progSky     =buildProgram("shaders/sky.vert",     "shaders/sky.frag");
    m_progCaustics=buildProgram("shaders/caustics.vert","shaders/caustics.frag");
}
void Renderer::buildMeshes(){
    buildGrid(150,m_waterVAO,m_waterVBO,m_waterEBO,m_waterIdx);
    buildPool(m_poolVAO,m_poolVBO);
    buildSphere(32,32,m_sphereVAO,m_sphereVBO,m_sphereEBO,m_sphereIdx);
    buildSkybox(m_skyVAO,m_skyVBO);
}
void Renderer::buildTextures(){
    m_tileTex=makeTile();
    m_skyTex =makeSky();
}
void Renderer::buildCausticsFBO(){
    glGenTextures(1,&m_causTex);
    glBindTexture(GL_TEXTURE_2D,m_causTex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16F,CRES,CRES,0,GL_RGBA,GL_FLOAT,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glGenFramebuffers(1,&m_causFBO);
    glBindFramebuffer(GL_FRAMEBUFFER,m_causFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,m_causTex,0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void Renderer::render(Water& water,
                      float sx,float sz,
                      float lax,float lay,
                      float cax,float cay,float cdist,
                      int w,int h)
{
    // Camera
    float cy=cosf(glm::radians(cax));
    glm::vec3 eye=glm::vec3(
        sinf(glm::radians(cay))*cy,
        sinf(glm::radians(cax)),
        cosf(glm::radians(cay))*cy)*cdist;
    glm::mat4 view=glm::lookAt(eye,glm::vec3(0),glm::vec3(0,1,0));
    glm::mat4 proj=glm::perspective(glm::radians(45.f),(float)w/h,0.05f,50.f);
    glm::mat4 vp=proj*view;

    // Light direction
    glm::vec3 L=glm::normalize(glm::vec3(
        sinf(glm::radians(lay))*cosf(glm::radians(lax)),
        sinf(glm::radians(lax)),
        cosf(glm::radians(lay))*cosf(glm::radians(lax))));

    // ── Caustics pass ─────────────────────────────────────────
    {
        GLint prevVP[4]; glGetIntegerv(GL_VIEWPORT,prevVP);
        glBindFramebuffer(GL_FRAMEBUFFER,m_causFBO);
        glViewport(0,0,CRES,CRES);
        glClearColor(0,0,0,0); glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND); glBlendFunc(GL_ONE,GL_ONE);

        glUseProgram(m_progCaustics);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,water.getHeightTex());
        setUniform1i(m_progCaustics,"uHeightTex",0);
        setUniform3f(m_progCaustics,"uLightDir",L.x,L.y,L.z);
        setUniform1f(m_progCaustics,"uTexelSize",1.f/water.getSize());
        glBindVertexArray(m_waterVAO);
        glDrawElements(GL_TRIANGLES,m_waterIdx,GL_UNSIGNED_INT,0);

        glDisable(GL_BLEND);glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        glViewport(prevVP[0],prevVP[1],prevVP[2],prevVP[3]);
    }

    // ── Main pass ─────────────────────────────────────────────
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0,0,w,h);
    glClearColor(0.08f,0.10f,0.14f,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Skybox
    glDepthMask(GL_FALSE); glDisable(GL_DEPTH_TEST);
    glUseProgram(m_progSky);
    setUniformMat4(m_progSky,"uView",glm::mat4(glm::mat3(view)));
    setUniformMat4(m_progSky,"uProj",proj);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP,m_skyTex);
    setUniform1i(m_progSky,"uSkybox",0);
    glBindVertexArray(m_skyVAO);
    glDrawArrays(GL_TRIANGLES,0,36);
    glDepthMask(GL_TRUE); glEnable(GL_DEPTH_TEST);

    // Pool
    glUseProgram(m_progPool);
    setUniformMat4(m_progPool,"uVP",vp);
    setUniform3f(m_progPool,"uLightDir",L.x,L.y,L.z);
    setUniform3f(m_progPool,"uEye",eye.x,eye.y,eye.z);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D,m_tileTex);
    setUniform1i(m_progPool,"uTileTex",0);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D,m_causTex);
    setUniform1i(m_progPool,"uCausticsTex",1);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D,water.getHeightTex());
    setUniform1i(m_progPool,"uHeightTex",2);
    glBindVertexArray(m_poolVAO);
    glDrawArrays(GL_TRIANGLES,0,30);

    // Sphere
   // Sphere — use m_ballY for Y position so drop animation works
float R = 0.25f;
float sphereWorldY = m_ballY;  // animated Y during drop, resting Y otherwise
glm::mat4 sModel = glm::scale(
    glm::translate(glm::mat4(1), glm::vec3(sx, sphereWorldY, sz)),
    glm::vec3(R));
glUseProgram(m_progSphere);
setUniformMat4(m_progSphere, "uMVP",     vp * sModel);
setUniformMat4(m_progSphere, "uModel",   sModel);
setUniform3f(m_progSphere,   "uLightDir",L.x,L.y,L.z);
setUniform3f(m_progSphere,   "uEye",     eye.x,eye.y,eye.z);
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyTex);
setUniform1i(m_progSphere, "uSkybox", 0);
glBindVertexArray(m_sphereVAO);
glDrawElements(GL_TRIANGLES, m_sphereIdx, GL_UNSIGNED_INT, 0);

    // Water surface (blended last)
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(m_progWater);
    setUniformMat4(m_progWater,"uVP",vp);
    setUniform3f(m_progWater,"uLightDir",L.x,L.y,L.z);
    setUniform3f(m_progWater,"uEye",eye.x,eye.y,eye.z);
    setUniform3f(m_progWater,"uSpherePos",sx,m_ballY,sz);
    setUniform1f(m_progWater,"uSphereR",R);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D,water.getHeightTex());
    setUniform1i(m_progWater,"uHeightTex",0);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D,water.getNormalTex());
    setUniform1i(m_progWater,"uNormalTex",1);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_CUBE_MAP,m_skyTex);
    setUniform1i(m_progWater,"uSkybox",2);
    glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D,m_causTex);
    setUniform1i(m_progWater,"uCausticsTex",3);
    glBindVertexArray(m_waterVAO);
    glDrawElements(GL_TRIANGLES,m_waterIdx,GL_UNSIGNED_INT,0);
    glDisable(GL_BLEND);
}