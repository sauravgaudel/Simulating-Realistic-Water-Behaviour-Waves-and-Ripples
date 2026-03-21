#pragma once
#include "common.h"

class Water {
public:
    explicit Water(int size = 256);
    ~Water();

    void update(float sphereX = 9999.f, float sphereZ = 9999.f, float sphereR = 0.25f);
    void addDrop(float x, float y, float radius, float strength);
    void toggleGravity() { m_gravity = !m_gravity; }

    GLuint getHeightTex() const { return m_tex[m_current]; }
    GLuint getNormalTex()  const { return m_normalTex; }
    int    getSize()       const { return m_size; }

private:
    void initTextures();
    void initShaders();
    void initQuad();
    void computeNormals();
    void beginOffscreen(GLint vp[4]);
    void endOffscreen(const GLint vp[4]);

    int    m_size;
    bool   m_gravity = false;
    int    m_current = 0;

    GLuint m_fbo[2]     = {};
    GLuint m_tex[2]     = {};
    GLuint m_normalTex  = 0;
    GLuint m_normalFBO  = 0;
    GLuint m_quadVAO    = 0;
    GLuint m_quadVBO    = 0;

    GLuint m_progUpdate = 0;
    GLuint m_progDrop   = 0;
    GLuint m_progNormal = 0;
    GLuint m_progSphere = 0;
};