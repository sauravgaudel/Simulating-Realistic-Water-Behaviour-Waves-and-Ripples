#include "water.h"
#include <vector>
#include <cstdio>

static const float kQuad[] = {
    -1,-1, 0,0,  1,-1, 1,0,  1,1, 1,1,
    -1,-1, 0,0,  1, 1, 1,1, -1,1, 0,1
};

void Water::beginOffscreen(GLint vp[4]) {
    glGetIntegerv(GL_VIEWPORT, vp);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glViewport(0, 0, m_size, m_size);
}

void Water::endOffscreen(const GLint vp[4]) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glViewport(vp[0], vp[1], vp[2], vp[3]);
}

Water::Water(int size) : m_size(size) {
    initTextures();
    initShaders();
    initQuad();

    // seed drops so waves start immediately
    addDrop(0.5f, 0.5f, 0.05f, 0.12f);
    addDrop(0.3f, 0.3f, 0.04f, 0.10f);
    addDrop(0.7f, 0.6f, 0.04f, 0.10f);
    addDrop(0.2f, 0.8f, 0.03f, 0.08f);

    // prime normal map
    GLint vp[4];
    beginOffscreen(vp);
    computeNormals();
    endOffscreen(vp);

    printf("Water OK: size=%d tex=%u,%u normal=%u\n",
           m_size, m_tex[0], m_tex[1], m_normalTex);
}

Water::~Water() {
    glDeleteFramebuffers(2, m_fbo);
    glDeleteTextures(2, m_tex);
    glDeleteFramebuffers(1, &m_normalFBO);
    glDeleteTextures(1, &m_normalTex);
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteBuffers(1, &m_quadVBO);
    glDeleteProgram(m_progUpdate);
    glDeleteProgram(m_progDrop);
    glDeleteProgram(m_progNormal);
    glDeleteProgram(m_progSphere);
}

void Water::initTextures() {
    std::vector<float> zero(m_size * m_size * 4, 0.f);
    for (int i = 0; i < 2; i++) {
        glGenTextures(1, &m_tex[i]);
        glBindTexture(GL_TEXTURE_2D, m_tex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_size, m_size,
                     0, GL_RGBA, GL_FLOAT, zero.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glGenFramebuffers(1, &m_fbo[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, m_tex[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            fprintf(stderr, "FBO[%d] incomplete!\n", i);
    }
    glGenTextures(1, &m_normalTex);
    glBindTexture(GL_TEXTURE_2D, m_normalTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_size, m_size,
                 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenFramebuffers(1, &m_normalFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_normalFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, m_normalTex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "normalFBO incomplete!\n");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Water::initShaders() {
    m_progUpdate = buildProgram("shaders/quad.vert","shaders/water_update.frag");
    m_progDrop   = buildProgram("shaders/quad.vert","shaders/water_drop.frag");
    m_progNormal = buildProgram("shaders/quad.vert","shaders/water_normal.frag");
    m_progSphere = buildProgram("shaders/quad.vert","shaders/water_sphere.frag");
}

void Water::initQuad() {
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kQuad), kQuad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*4, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*4, (void*)(2*4));
    glBindVertexArray(0);
}



void Water::update(float sphereX, float sphereZ, float sphereR) {
    GLint vp[4];
    beginOffscreen(vp);

    // wave propagation
    {
        int src = m_current, dst = 1 - m_current;
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[dst]);
        glUseProgram(m_progUpdate);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_tex[src]);
        setUniform1i(m_progUpdate, "uTexture",   0);
        setUniform1f(m_progUpdate, "uTexelSize", 1.f / m_size);
        setUniform1f(m_progUpdate, "uGravity",   m_gravity ? 0.008f : 0.0f);
        glBindVertexArray(m_quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_current = dst;
    }

    // sphere pushes water outward — add a ring of drops around sphere edge
    if (sphereX > -1.5f && sphereX < 1.5f) {
        float u = (sphereX + 1.f) * 0.5f;
        float v = (sphereZ + 1.f) * 0.5f;
        float r = sphereR * 0.5f;

        // add a circular drop at sphere radius to create bow wave
        int src = m_current, dst = 1 - m_current;
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[dst]);
        glUseProgram(m_progDrop);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_tex[src]);
        setUniform1i(m_progDrop, "uTexture",  0);
        setUniform2f(m_progDrop, "uCenter",   u, v);
        setUniform1f(m_progDrop, "uRadius",   r);
        setUniform1f(m_progDrop, "uStrength", -0.04f); // negative = depression
        glBindVertexArray(m_quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_current = dst;
    }

    computeNormals();
    endOffscreen(vp);
}

void Water::computeNormals() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_normalFBO);
    glUseProgram(m_progNormal);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex[m_current]);
    setUniform1i(m_progNormal, "uTexture",   0);
    setUniform1f(m_progNormal, "uTexelSize", 1.f / m_size);
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Water::addDrop(float x, float y, float radius, float strength) {
    GLint vp[4];
    beginOffscreen(vp);
    int src = m_current, dst = 1 - m_current;
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[dst]);
    glUseProgram(m_progDrop);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex[src]);
    setUniform1i(m_progDrop, "uTexture",  0);
    setUniform2f(m_progDrop, "uCenter",   x, y);
    setUniform1f(m_progDrop, "uRadius",   radius);
    setUniform1f(m_progDrop, "uStrength", strength);
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    m_current = dst;
    endOffscreen(vp);
}