#pragma once

// Windows OpenGL
#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif

// GLEW must come before any GL header
#include <GL/glew.h>
#include <GL/freeglut.h>

// GLM
#include "math3d.h"
// STL
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

// ---------------------------------------------------------------
// Shader utility: load, compile, link
// ---------------------------------------------------------------
inline std::string loadFile(const char* path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        fprintf(stderr, "ERROR: cannot open file: %s\n", path);
        return "";
    }
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

inline GLuint compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetShaderInfoLog(s, sizeof(log), nullptr, log);
        fprintf(stderr, "Shader compile error:\n%s\n", log);
    }
    return s;
}

inline GLuint buildProgram(const char* vertPath, const char* fragPath) {
    std::string vs = loadFile(vertPath);
    std::string fs = loadFile(fragPath);
    if (vs.empty() || fs.empty()) return 0;

    GLuint vert = compileShader(GL_VERTEX_SHADER,   vs.c_str());
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fs.c_str());
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    GLint ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
        fprintf(stderr, "Program link error:\n%s\n", log);
    }
    glDeleteShader(vert);
    glDeleteShader(frag);
    return prog;
}

// Convenience uniform setters
inline void setUniform1i(GLuint p, const char* n, int v)          { glUniform1i(glGetUniformLocation(p,n),v); }
inline void setUniform1f(GLuint p, const char* n, float v)        { glUniform1f(glGetUniformLocation(p,n),v); }
inline void setUniform2f(GLuint p, const char* n, float a,float b){ glUniform2f(glGetUniformLocation(p,n),a,b); }
inline void setUniform3f(GLuint p, const char* n, float a,float b,float c){ glUniform3f(glGetUniformLocation(p,n),a,b,c); }
inline void setUniformMat4(GLuint p, const char* n, const Mat4& m) {
    glUniformMatrix4fv(glGetUniformLocation(p,n),1,GL_FALSE,m.ptr());
}
