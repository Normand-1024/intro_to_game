#include <math.h>
#include "GraphicManager.h"
#include "Menu.h"
#include "MainGame.h"
#include "OverMenu.h"

#define PI 3.14159265

SDL_Window* displayWindow;

enum GameMode { MENU, GAME, OVER, SUCCESS};
enum PlayerMode { ONE, TWO };
enum Songs { INQ, UNDER, CIV };

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
	displayWindow = SDL_CreateWindow("Space Invader Premium Pass Bronze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 800, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	glViewport(0, 0, 600, 800);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	float lastFrameTicks = 0.0f;

	Matrix projectionMatrix;
	projectionMatrix.SetOrthoProjection(-3.0f, 3.0f, -4.0f, 4.0f, -1.0f, 1.0f);
	Matrix modelviewMatrix;
	program.SetModelviewMatrix(modelviewMatrix);
	program.SetProjectionMatrix(projectionMatrix);

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		std::cout << "Error:" << Mix_GetError() << std::endl;

	Mix_Music* UNDER = Mix_LoadMUS("UNDER.mp3"), *CIV = Mix_LoadMUS("CIV.mp3"), *INQ = Mix_LoadMUS("INQ.wav");
	Mix_Chunk* GREY_SOUND = Mix_LoadWAV("grey.wav"), *EXP_SOUND = Mix_LoadWAV("Explode.wav"), *MAT_SOUND = Mix_LoadWAV("Match.wav");
	Mix_VolumeChunk(MAT_SOUND, 20);
	Mix_VolumeChunk(GREY_SOUND, 70);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*Game Variables*/
	GameMode gameMode = GameMode::MENU;
	PlayerMode playerMode = PlayerMode::ONE;
	Songs song = Songs::INQ;

	/*Game Scenes*/
	Menu MenuScene = Menu();
	MainGame GameScene = MainGame();
	OverMenu OverScene = OverMenu();


	SDL_Event event;
	bool done = false;
	while (!done) {
		glUseProgram(program.programID);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float delta = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		/*Process Input*/
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE || (gameMode == GameMode::MENU && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE))
				done = true;

			if (gameMode == GameMode::MENU) {
				MenuScene.ProcessInput(event, gameMode, song, playerMode);
				if (gameMode == GameMode::GAME) {
					GameScene.InitializeLevel(gameMode, song, playerMode);
					if (song == Songs::CIV)
						Mix_PlayMusic(CIV, 0);
					else if (song == Songs::INQ)
						Mix_PlayMusic(INQ, 0);
					else if (song == Songs::UNDER)
						Mix_PlayMusic(UNDER, 0);
				}
			}

			if (gameMode == GameMode::OVER || gameMode == GameMode::SUCCESS) {
				OverScene.ProcessInput(event, gameMode);
				if (gameMode == GameMode::GAME) {
					GameScene.InitializeLevel(gameMode, song, playerMode);
					if (song == Songs::CIV)
						Mix_PlayMusic(CIV, 0);
					else if (song == Songs::INQ)
						Mix_PlayMusic(INQ, 0);
					else if (song == Songs::UNDER)
						Mix_PlayMusic(UNDER, 0);
				}
			}
		
		}

		if (gameMode == GameMode::GAME) {
			GameScene.ProcessInput(event, delta, playerMode, gameMode);

			if (gameMode == GameMode::OVER || gameMode == GameMode::SUCCESS)
				Mix_HaltMusic();
		}

		/*Update*/
		if (gameMode == GameMode::GAME) {
			GameScene.Update(delta, song, gameMode, GREY_SOUND, EXP_SOUND, MAT_SOUND);

			if (gameMode == GameMode::OVER || gameMode == GameMode::SUCCESS)
				Mix_HaltMusic();
		}

		/*Draw*/
		if (gameMode == GameMode::MENU) {
			MenuScene.Draw(program, song, playerMode);
		}
		else if (gameMode == GameMode::GAME) {
			GameScene.Draw(program);
		}
		else if (gameMode == GameMode::OVER || gameMode == GameMode::SUCCESS) {
			GameScene.Draw(program);
			OverScene.Draw(program, gameMode, GameScene.GetScore());
		}

		//glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(displayWindow);
	}

	Mix_FreeMusic(CIV);
	Mix_FreeMusic(INQ);
	Mix_FreeMusic(UNDER);
	Mix_FreeChunk(GREY_SOUND);
	Mix_FreeChunk(EXP_SOUND);
	Mix_FreeChunk(MAT_SOUND);
	SDL_Quit();
	return 0;
}
