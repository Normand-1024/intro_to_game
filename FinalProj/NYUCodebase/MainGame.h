#pragma once

#include "GraphicManager.h"
#include "Gameboard.h"
#include <vector>
#include <ctime> 

enum GameMode;
enum PlayerMode;
enum Songs;

struct Explosion : GameObject {
	Explosion(unsigned int explosionT);

	void Explode();

	void Update(float delta);

	float frameTime = 0.1f, timeElapsed;
	int currentIndex = 0, frameNum = 8, frameEachRow = 4;
};

class MainGame {
public:
	MainGame();

	void InitializeLevel(GameMode& gm, Songs& songs, PlayerMode& playerMode);

	int GetScore();

	void SetPossibility(const Songs& song);

	void DropBlock(int columnNum);

	void ProcessInput(SDL_Event& event, const float delta, PlayerMode& playerMode, GameMode& gm);

	void Update(const float delta, const Songs& song, GameMode& gm);

	void Draw(ShaderProgram& program);

	void DrawBoard(ShaderProgram& program, TileType tile, float X, float Y);

private:
	Gameboard board;

	float timePassed;
	float timeEnd;
	float timeSinceDrop;

	//mdf
	float redP = 0.1f;
	float blueP = 0.2f;
	float greyP = 0.25f;

	//Game Variables
	float playerSpeed = 7.0f;
	float blockDropSpeed = 2.0f;

	int score;

	float firstPlayerLeftBound = -1.5f, firstPlayerRightBound = 1.5f;

	GameObject background;
	GameObject player;
	GameObject player2;
	std::vector<GameObject> block;
	std::vector<Explosion> booms;

	GLuint boardTex;

	GLuint REDblockTex;
	GLuint BLUEblockTex;
	GLuint GREYblockTex;

	GLuint removeTex;

	GLuint playerTex;
	GLuint player2Tex;
	
	GLuint CIVTex;
	GLuint UNDERTex;
	GLuint INQTex;
	
	GLuint fontTex;

};