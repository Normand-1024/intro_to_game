#include "MainGame.h"

enum GameMode { MENU, GAME, OVER, SUCCESS};
enum PlayerMode { ONE, TWO };
enum Songs { INQ, UNDER, CIV };

float mapValue(float value, float srcMin, float srcMax, float dstMin, float dstMax) {
	float retVal = dstMin + ((value - srcMin) / (srcMax - srcMin) * (dstMax - dstMin));
	if (retVal < dstMin) {
		retVal = dstMin;
	}
	if (retVal > dstMax) {
		retVal = dstMax;
	}
	return retVal;
}

Explosion::Explosion(unsigned int explosionT) : GameObject(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.75f, 0.75f, 1.0f), explosionT, NONE) {
	active = false;
	sprite.u = 0.0f;
	sprite.v = 0.0f;
	sprite.width = 75.0f / 300.0f;
	sprite.height = 75.0f / 300.0f;
}

void Explosion::Explode() {
	timeElapsed = 0.0f;
	sprite.u = 0.0f;
	sprite.v = 0.0f;
	active = true;
}

void Explosion::Update(float delta) {
	if (active) {
		timeElapsed += delta;
		if (timeElapsed > frameTime * frameNum) active = false;
		else {
			currentIndex = (int)(timeElapsed / frameTime) % frameNum;
			sprite.u = (currentIndex % frameEachRow) * sprite.height;
			sprite.v = (currentIndex / frameEachRow) * sprite.width;
		}
	}
}

MainGame::MainGame() {

	fontTex = LoadTexture(RESOURCE_FOLDER"font.png");
	boardTex = LoadTexture(RESOURCE_FOLDER"board.png");

	removeTex = LoadTexture(RESOURCE_FOLDER"blockRemove.png");

	playerTex = LoadTexture(RESOURCE_FOLDER"player1.png");
	player2Tex = LoadTexture(RESOURCE_FOLDER"player2.png");

	GREYblockTex = LoadTexture(RESOURCE_FOLDER"gBlock.png");
	REDblockTex = LoadTexture(RESOURCE_FOLDER"rBlock.png");
	BLUEblockTex = LoadTexture(RESOURCE_FOLDER"bBlock.png");

	CIVTex = LoadTexture(RESOURCE_FOLDER"CIVB.png");
	UNDERTex = LoadTexture(RESOURCE_FOLDER"UNDERB.png");
	INQTex = LoadTexture(RESOURCE_FOLDER"INQB.png");

	player = GameObject(Vec3(0.0f, -1.75f/*-3.75f*/, 0.0f), Vec3(0.5f, 0.75f, 1.0f), playerTex, BOX);
	player2 = GameObject(Vec3(2.0f, -1.75f, 0.0f), Vec3(0.5f, 0.75f, 1.0f), player2Tex, BOX);
	block = std::vector<GameObject>();
	board = Gameboard();
	booms = std::vector<Explosion>(20, Explosion(removeTex));
}

int MainGame::GetScore(){ return score; }

void MainGame::DropBlock(int columnNum) {
	float start = -0.375f - 0.75f * 1.0f;
	if (board.Board.size() > 4)	start = -0.375f - 0.75f * 3.0f;

	float r = ((float)(std::rand())) / ((float)RAND_MAX);
	ObjTag t = ObjTag::REMOVE;
	GLuint tex = playerTex;

	if (r < redP) {
		t = ObjTag::ORED;
		tex = REDblockTex;
	}
	else if (r < blueP) { 
		t = ObjTag::OBLUE;
		tex = BLUEblockTex;
	}
	else if (r < greyP) {
		t = ObjTag::OGREY;
		tex = GREYblockTex;
	}

	if (t != ObjTag::REMOVE) {
		block.push_back(GameObject(Vec3(start + 0.75f * (float)columnNum, 8.0f + 0.75f, 0.0f), Vec3(0.75f, 0.75f, 1.0f), tex, BOX));
		block[block.size() - 1].tag = t;
	}

}


void MainGame::DrawBoard(ShaderProgram& program, TileType tile, float X, float Y) {
	GameObject aBlock = GameObject(Vec3(X, Y, 0.0f), Vec3(0.75f, 0.75f, 1.0f), boardTex, NONE);
	aBlock.sprite.width = 1.0f / 4.0f;
	aBlock.sprite.height = 1.0f / 4.0f;
	aBlock.sprite.u = tile * aBlock.sprite.height;

	GMDraw(&program, aBlock);
}

void MainGame::InitializeLevel(GameMode& gm, Songs& songs, PlayerMode& playerMode) {
	std::srand(std::time(0));

	if (songs == Songs::INQ) {
		background = GameObject(Vec3(0.0f, 3.5f, 0.0f), Vec3(6.0f, 15.0f, 1.0f), INQTex, NONE);
		blockDropSpeed = 2.5f;
		timeEnd = 173.0f;
	}
	else if (songs == Songs::UNDER) {
		background = GameObject(Vec3(0.0f, 3.5f, 0.0f), Vec3(6.0f, 15.0f, 1.0f), UNDERTex, NONE);
		blockDropSpeed = 5.0f;
		timeEnd = 160.0f;
	}
	else if (songs == Songs::CIV) {
		background = GameObject(Vec3(0.0f, 3.5f, 0.0f), Vec3(6.0f, 15.0f, 1.0f), CIVTex, NONE);
		blockDropSpeed = 3.0f;
		timeEnd = 231.0f;
	}

	if (playerMode == TWO)
		firstPlayerLeftBound = - (firstPlayerRightBound = 3.0f);
	else
		firstPlayerLeftBound = -(firstPlayerRightBound = 1.5f);
	
	block.clear();

	board.clear();
	board.createBoard(playerMode);
	timePassed = 0.0f;
	score = 0;
}

void MainGame::ProcessInput(SDL_Event& event, const float delta, PlayerMode& playerMode, GameMode& gm) {

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_ESCAPE])
		gm = OVER;

	if (keys[SDL_SCANCODE_LEFT] && player.position.x - player.scale.x / 2 > firstPlayerLeftBound)
		player.position.x -= playerSpeed * delta;
	if (keys[SDL_SCANCODE_RIGHT] && player.position.x + player.scale.x / 2 < firstPlayerRightBound) 
		player.position.x += playerSpeed * delta;

	if (playerMode == TWO) {
		if (keys[SDL_SCANCODE_A] && player2.position.x - player2.scale.x / 2 > -3.0f)
			player2.position.x -= playerSpeed * delta;
		if (keys[SDL_SCANCODE_D] && player2.position.x + player2.scale.x / 2 < 3.0f)
			player2.position.x += playerSpeed * delta;
	}
}

void MainGame::Update(const float delta, const Songs& song, GameMode& gm) {
	SetPossibility(song);
	timePassed += delta;

	for (Explosion& e : booms)
		if (e.active)e.Update(delta);

	if (timePassed > timeEnd) {
		gm = OVER;
		return;
	}

	for (int i = 0; i < block.size(); i++) {
		block[i].position.y -= blockDropSpeed * delta;
		if (block[i].ifCollide(player) || (board.Board.size() > 4 && block[i].ifCollide(player2))) {
			TileType insertT = TileType::EMPTY;
			int columnNum = -1;

			if (block[i].tag == ORED)
				insertT = TileType::RED;
			else if (block[i].tag == OBLUE)
				insertT = TileType::BLUE;
			else if (block[i].tag == OGREY)
				insertT = TileType::GREY;

			if (board.Board.size() > 4)
				columnNum = (int)((block[i].position.x + 0.375f + 0.75f * 3.0f) / 0.75f);
			else
				columnNum = (int)((block[i].position.x + 0.375f + 0.75f * 1.0f) / 0.75f);

			int scoreChange = board.insert(columnNum, insertT);
			score -= scoreChange;

			if (scoreChange > 0) {
				for (int j = 0; j < 5; j++) {
					for (Explosion& e : booms) {
						if (!e.active) {
							e.position = Vec3(block[i].position.x, -4.0f + j * 0.75f / 2, 0.0f);
							e.Explode();
							break;
						}
					}
				}
			}

			block.erase(block.begin() + i);
		}


		else if (block[i].position.y < -8.0f)
			block.erase(block.begin() + i);
	}

	score += board.scoreMatch();

	timeSinceDrop += delta;
	if (timeSinceDrop >= 0.75f / blockDropSpeed) {
		int columnNum = 4;
		if (board.Board.size() > 4) columnNum = 8;

		for (int i = 0; i < columnNum; i++)
			DropBlock(i);
		
		while (timeSinceDrop >= 0.75f / blockDropSpeed)
			timeSinceDrop -= 0.75f / blockDropSpeed;
	}
}

void MainGame::Draw(ShaderProgram& program) {
	float y = mapValue(timePassed, 0.0f, timeEnd, -3.5f, 3.5f);
	background.position.y = -y;
	GMDraw(&program, background);

	for (GameObject& b : block)
		GMDraw(&program, b);

	float boardDrawX = -1.125f, boardDrawY = -4.0f;
	if (board.Board.size() > 4)
		boardDrawX = -2.625f;
	
	for (int i = 0; i < board.Board.size(); i++) {
		for (int j = 0; j < 5; j++) {
			DrawBoard(program, board.Board[i][j], boardDrawX, boardDrawY);
			boardDrawY += 0.375f;
		}
		boardDrawY = -4.0f;
		boardDrawX += 0.75f;
	}

	GMDraw(&program, player);
	if (board.Board.size() > 4)	GMDraw(&program, player2);

	for (Explosion& e : booms)
		if (e.active) GMDraw(&program, e);

	DrawText(&program, fontTex, std::to_string(score), 0.4f, 0.0f, Vec3(-2.8f, -3.8f, 0.0f));
}