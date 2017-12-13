#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include <vector>
#include "GameObject.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

GLuint LoadTexture(const char *filePath);

void GMDraw(ShaderProgram* p, GameObject& object);

void DrawText(ShaderProgram* p, int fontTexture, std::string text, float size, float spacing, const Vec3& pos, bool selected = false);