#pragma once

#include "GraphicManager.h"

enum GameMode;
enum PlayerMode;
enum Songs;

class OverMenu {
public:
	OverMenu();

	void ProcessInput(SDL_Event& event, GameMode& gm);

	void Draw(ShaderProgram& program, const GameMode gm, int score);
	
private:
	GLuint successB;
	GLuint overB;
	GLuint fontTex;

	GameObject success;
	GameObject over;
};