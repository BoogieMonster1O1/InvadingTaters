#ifndef SPACEINVADERS_DRAWABLEHELPER_H
#define SPACEINVADERS_DRAWABLEHELPER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "data.h"

void blit(sprite_buffer* buffer, const sprite& sprite, size_t x, size_t y, uint32_t color);

void blitNumber(sprite_buffer* buffer, const sprite& number_spritesheet, size_t number, size_t x, size_t y, uint32_t color);

void blitText(sprite_buffer* buffer, const sprite& atlas, const char* text, size_t x, size_t y, uint32_t color);

bool checkOverlap(const sprite& sp_a, size_t x_a, size_t y_a, const sprite& sp_b, size_t x_b, size_t y_b);

#endif
