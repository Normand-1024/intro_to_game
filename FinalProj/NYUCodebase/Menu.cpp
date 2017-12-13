#include "Menu.h"

enum GameMode { MENU, GAME, OVER };
enum PlayerMode { ONE, TWO };
enum Songs { INQ, UNDER, CIV };

Menu::Menu() {
	backGroundTex = LoadTexture(RESOURCE_FOLDER"background1.png");
	background = GameObject(Vec3(0.0f, 0.0f, 0.0f), Vec3(6.0f, 8.0f, 1.0f), backGroundTex, NONE);

	fontTex = LoadTexture(RESOURCE_FOLDER"font.png");
}

void Menu::ProcessInput(SDL_Event& event, GameMode& gm, Songs& songs, PlayerMode& pm) {
	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.y < 800 - 530 && event.button.y > 800 - 560) pm = ONE;
	else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.y <= 800 - 500 && event.button.y > 800 - 530) pm = TWO;
	else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.y <= 800 - 340 && event.button.y > 800 - 370) songs = INQ;
	else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.y <= 800 - 310 && event.button.y > 800 - 340) songs = UNDER;
	else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.y <= 800 - 280 && event.button.y > 800 - 310) songs = CIV;
	else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.y <= 800 - 110 && event.button.y > 800 - 200) gm = GAME;
}

void Menu::Draw(ShaderProgram& program, const Songs songs, const PlayerMode pm) {
	GMDraw(&program, background);

	DrawText(&program, fontTex, "-Choose Mode-", 0.2f, 0.0f, Vec3(-2.0f, 2.0f, 0.0f));
	DrawText(&program, fontTex, "Single", 0.2f, 0.0f, Vec3(-2.0f, 1.5f, 0.0f), pm == ONE);
	DrawText(&program, fontTex, "Co-op", 0.2f, 0.0f, Vec3(-2.0f, 1.2f, 0.0f), pm == TWO);

	DrawText(&program, fontTex, "-Choose Song-", 0.2f, 0.0f, Vec3(-2.0f, 0.0f, 0.0f));
	DrawText(&program, fontTex, "Dragon Age: Inquisition", 0.2f, 0.0f, Vec3(-2.0f, -0.5f, 0.0f), songs == INQ);
	DrawText(&program, fontTex, "Undertale", 0.2f, 0.0f, Vec3(-2.0f, -0.8f, 0.0f), songs == UNDER);
	DrawText(&program, fontTex, "Civilization VI", 0.2f, 0.0f, Vec3(-2.0f, -1.1f, 0.0f), songs == CIV);

	DrawText(&program, fontTex, "[START GAME]", 0.3f, 0.02f, Vec3(-1.8f, -2.5f, 0.0f));
}