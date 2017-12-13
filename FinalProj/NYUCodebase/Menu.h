#pragma once

#include "GraphicManager.h"

enum GameMode;
enum PlayerMode;
enum Songs;

class Menu {
public:
	Menu();

	void ProcessInput(SDL_Event& event, GameMode& gm, Songs& songs, PlayerMode& playerMode);

	void Draw(ShaderProgram& program, const Songs songs, const PlayerMode pm);
	
private:
	GLuint backGroundTex;
	GLuint fontTex;

	GameObject background;
};