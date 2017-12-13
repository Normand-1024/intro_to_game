#include "OverMenu.h"

enum GameMode { MENU, GAME, OVER, SUCCESS };
enum PlayerMode { ONE, TWO };
enum Songs { INQ, UNDER, CIV };

OverMenu::OverMenu() {
	successB = LoadTexture(RESOURCE_FOLDER"background2.png");
	overB = LoadTexture(RESOURCE_FOLDER"background2.png");
	success = GameObject(Vec3(0.0f, 1.5f, 0.0f), Vec3(4.0f, 3.0f, 1.0f), successB, NONE);
	over = GameObject(Vec3(0.0f, 1.5f, 0.0f), Vec3(4.0f, 3.0f, 1.0f), overB, NONE);

	fontTex = LoadTexture(RESOURCE_FOLDER"font.png");
}

void OverMenu::ProcessInput(SDL_Event& event, GameMode& gm) {
	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.y >325 && event.button.y <= 375) gm = MENU;
	else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.y >= 275 && event.button.y <= 325) gm = GAME;
}

void OverMenu::Draw(ShaderProgram& program, const GameMode gm, int score) {

	GMDraw(&program, success);

	DrawText(&program, fontTex, std::to_string(score), 0.3f, 0.0f, Vec3(-1.4f, 1.8f, 0.0f));

	DrawText(&program, fontTex, "[RESTART]", 0.2f, 0.0f, Vec3(-0.85f, 1.0f, 0.0f));
	DrawText(&program, fontTex, "[BACK TO MENU]", 0.2f, 0.0f, Vec3(-1.3f, 0.4f, 0.0f));
}