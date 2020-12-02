#ifndef SPACEINVADERS_GLUTIL_H
#define SPACEINVADERS_GLUTIL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define GL_ERROR_CASE(err)\
    case err: snprintf(error, sizeof(error), "%s", #err)

void gl_debug(const char *file, int line);

void windowHints();

void texParams();

void validateShader(GLuint shader, const char *file = nullptr);

bool validateProgram(GLuint program);

#endif
