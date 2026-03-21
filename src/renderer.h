#pragma once
#include "common.h"
#include "water.h"

class Renderer {
public:
    Renderer();
    ~Renderer();
    void render(Water& water,
                float sx, float sz,
                float lax, float lay,
                float cax, float cay, float cdist,
                int w, int h);
    void setBallY(float y) { m_ballY = y; }
private:
    void buildShaders();
    void buildMeshes();
    void buildTextures();
    void buildCausticsFBO();

    // shaders
    GLuint m_progPool=0, m_progWater=0, m_progSphere=0, m_progSky=0, m_progCaustics=0;

    // meshes
    GLuint m_waterVAO=0, m_waterVBO=0, m_waterEBO=0; int m_waterIdx=0;
    GLuint m_poolVAO=0,  m_poolVBO=0;
    GLuint m_sphereVAO=0,m_sphereVBO=0,m_sphereEBO=0; int m_sphereIdx=0;
    GLuint m_skyVAO=0,   m_skyVBO=0;

    // textures
    GLuint m_tileTex=0, m_skyTex=0;

    // caustics FBO
    GLuint m_causFBO=0, m_causTex=0;
    static const int CRES=512;
    float m_ballY = -0.25f;  // add this with other private members
};