#ifdef _WINDOWS
	#include <GL/glew.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include <math.h>
#include <iostream>
#include <vector>

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define PI 3.14159265

SDL_Window* displayWindow;

struct Vec3 {
	Vec3(float xi = 0, float yi = 0, float zi = 0) :x(xi), y(yi), z(zi) {}
	Vec3(const Vec3& copied) :x(copied.x), y(copied.y), z(copied.z) {}

	void applyDelta(Vec3& deltaVec, float delta) {
		x += deltaVec.x * delta;
		y += deltaVec.y * delta;
		z += deltaVec.z * delta;
	}

	float x, y, z;
};

GLuint LoadTexture(const char *filePath) {
	int w, h, comp;
	unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}
	GLuint retTexture;
	glGenTextures(1, &retTexture);
	glBindTexture(GL_TEXTURE_2D, retTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(image);
	return retTexture;
}

enum ObjTag {PLAYER, ENEMY, ENEMY_BULLET, PLAYER_BULLET, EXPLOSION};
enum ColliderType {BOX, CIRCLE, POINT, NONE};
enum GameMode {MENU, GAME, OVER};

class SheetSprite {
public:
	SheetSprite(unsigned int tI) :textureID(tI), u(0.0f), v(0.0f), width(1.0f), height(1.0f), size(1.0f) {}
	SheetSprite(unsigned int tI, float uI, float vI, float wI, float hI, float sI) :textureID(tI), u(uI), v(vI), width(wI), height(hI), size(sI) {}

	float size;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
};

struct GameObject {
	GameObject(Vec3& pos, Vec3& sc, GLuint textureI, ColliderType type) :position(pos), rotation(), scale(sc), sprite(textureI), cType(type){}
	GameObject() :sprite(SheetSprite(0)){}

	void Update(float delta) {
		velocity.applyDelta(acceleration, delta);
		position.applyDelta(velocity, delta);
	}

	bool ifCollide(GameObject& theOther) {
		float x = position.x, y = position.y, w = scale.x, h = scale.y, ox = theOther.position.x, oy = theOther.position.y, ow = theOther.scale.x, oh = theOther.scale.y;

		if (cType == NONE || theOther.cType == NONE) return false;

		bool	a = (y - h / 2) > (oy + oh / 2),
				b = (y + h / 2) < (oy - oh / 2),
				c = (x - w / 2) > (ox + ow / 2),
				d = (x + w / 2) < (ox - ow / 2);

		return !a && !b && !c && !d;
	}

	Vec3 position;
	Vec3 rotation;
	Vec3 scale;
	Vec3 velocity;
	Vec3 acceleration;

	SheetSprite sprite;
	ColliderType cType;

	ObjTag tag;
	bool active = true;
};

void Draw(ShaderProgram* p, GameObject& object) {
	Matrix matrix;
	matrix.Identity();
	matrix.Translate(object.position.x, object.position.y, 0.0f);
	matrix.Scale(object.scale.x / 2, object.scale.y / 2, 0.0f);

	glBindTexture(GL_TEXTURE_2D, object.sprite.textureID);

	float width = object.sprite.width, height = object.sprite.height, aspect = width / height, size = object.sprite.size, u = object.sprite.u, v = object.sprite.v;
	float vertices[] = {
		-1.0f * size * aspect, -1.0f * size,
		1.0f * size * aspect, 1.0f * size,
		-1.0f * size * aspect, 1.0f * size,
		1.0f * size * aspect, 1.0f * size,
		-1.0f * size * aspect, -1.0f * size ,
		1.0f * size * aspect, -1.0f * size };
	glVertexAttribPointer(p->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(p->positionAttribute);

	GLfloat texCoords[] = {
		u, v + height,
		u + width, v,
		u, v,
		u + width, v,
		u, v + height,
		u + width, v + height
	};
	glVertexAttribPointer(p->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(p->texCoordAttribute);
	p->SetModelviewMatrix(matrix);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(p->positionAttribute);
	glDisableVertexAttribArray(p->texCoordAttribute);
}

void DrawText(ShaderProgram* p, int fontTexture, std::string text, float size, float spacing, const Vec3& pos) {
	Matrix matrix;
	matrix.Identity();
	matrix.Translate(pos.x, pos.y, 0.0f);

	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		int spriteIndex = (int)text[i];
		float texture_x = (float)(spriteIndex % 16) / 16.0f;
		float texture_y = (float)(spriteIndex / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}
	glBindTexture(GL_TEXTURE_2D, fontTexture);

	float* vp = vertexData.data(), *tp = texCoordData.data();
	
	for (int i = 0; i < vertexData.size() / 12; ++i) {
		glVertexAttribPointer(p->positionAttribute, 2, GL_FLOAT, false, 0, vp);
		glEnableVertexAttribArray(p->positionAttribute);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glVertexAttribPointer(p->texCoordAttribute, 2, GL_FLOAT, false, 0, tp);
		glEnableVertexAttribArray(p->texCoordAttribute);
		p->SetModelviewMatrix(matrix);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(p->positionAttribute);
		glDisableVertexAttribArray(p->texCoordAttribute);

		vp+=12;
		tp+=12;
	}
}

struct Explosion : GameObject {
	Explosion(GLuint explosionT) : GameObject(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.5f, 1.5f, 1.0f), explosionT, NONE) {
		tag = EXPLOSION;
		active = false;
		sprite.u = 0.0f;
		sprite.v = 0.0f;
		sprite.width = 1.0f / 3.0f;
		sprite.height = 1.0f / 3.0f;
	}

	void Explode() {
		timeElapsed = 0.0f;
		sprite.u = 0.0f;
		sprite.v = 0.0f;
		active = true;
	}

	void Update(float delta){
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
	
	float frameTime = 0.15f, timeElapsed;
	int currentIndex = 0, frameNum = 8, frameEachRow = 3;
};

struct GameState {
	GameState(GLuint& playerTex, GLuint& enemyTex) {
		Player = GameObject(Vec3(0.0f, -3.5f, 0.0f), Vec3(0.3f, 0.3f, 1.0f), playerTex, BOX);
		Player.active = true;

		Bullets = std::vector<GameObject>();
		for (int i = 0; i < 300; i++) {
			Bullets.push_back(GameObject(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.04f, 0.3f, 1.0f), 0, POINT));
			Bullets[i].active = false;
			Bullets[i].velocity = Vec3(0.0f, 2.0f, 0.0f);
		}

		Crates = std::vector<GameObject>();
		float HordeSpeed = 0.5f;
		for (int i = 0; i < 56; i++) {
			int j = i / 8, z = i % 8;
			Crates.push_back(GameObject(Vec3(-2.5f + z * 0.5f, 3.5f - j * 0.5f, 0.0f), Vec3(0.3f, 0.3f, 1.0f), enemyTex, BOX));
			Crates[i].active = true;
			Crates[i].tag = ENEMY;
		}
	}

	void Reset() {
		Player.position = Vec3(0.0f, -3.5f, 0.0f);
		Player.active = true;

		for (GameObject& bullet : Bullets)
			bullet.active = false;

		for (int i = 0; i < 56; i++) {
			int j = i / 8, z = i % 8;
			Crates[i].position = Vec3(-2.5f + z * 0.5f, 3.5f - j * 0.5f, 0.0f);
			Crates[i].active = true;
		}

		HordeSpeed = 0.5f;
	}

	GameObject Player;
	std::vector<GameObject> Bullets;
	std::vector<GameObject> Crates;
	float HordeSpeed = 0.5f;
};

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Space Invader Premium Pass Bronze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 800, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	glViewport(0, 0, 600, 800);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	GLuint bT = LoadTexture(RESOURCE_FOLDER"b.png");
	GLuint bmT = LoadTexture(RESOURCE_FOLDER"bm.png");
	GLuint pT = LoadTexture(RESOURCE_FOLDER"player.png");
	GLuint cT = LoadTexture(RESOURCE_FOLDER"crates.png");
	GLuint lT = LoadTexture(RESOURCE_FOLDER"laser.png");
	GLuint flT = LoadTexture(RESOURCE_FOLDER"Flaser.png");
	GLuint expT = LoadTexture(RESOURCE_FOLDER"burst.png");
	GLuint fontTex = LoadTexture(RESOURCE_FOLDER"font.png");

	float lastFrameTicks = 0.0f;

	Matrix projectionMatrix;
	projectionMatrix.SetOrthoProjection(-3.0f, 3.0f, -4.0f, 4.0f, -1.0f, 1.0f);
	Matrix modelviewMatrix;
	program.SetModelviewMatrix(modelviewMatrix);
	program.SetProjectionMatrix(projectionMatrix);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*Game Actors*/
	GameMode GameMode = MENU;

	GameState GameState(pT, cT);

	float PlayerSpeed = 3.0f;

	float& HordeSpeed = GameState.HordeSpeed;
	GameObject& Player = GameState.Player;
	std::vector<GameObject>& Crates = GameState.Crates;
	std::vector<GameObject>& Bullets = GameState.Bullets;
	GameObject background(Vec3(0.0f, 0.0f, 0.0f), Vec3(6.0f, 8.0f, 1.0f), bmT, NONE);

	std::vector<Explosion> Explosions = std::vector<Explosion>();
	for (int i = 0; i < 40; i++) 
		Explosions.push_back(Explosion(expT));


	SDL_Event event;
	bool done = false;
	while (!done) {
		glUseProgram(program.programID);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float delta = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		/*Process Input*/
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
				done = true;
			else if (GameMode != GAME && ((event.type == SDL_MOUSEBUTTONDOWN && event.button.x < 400 && event.button.x > 200 && event.button.y < 450 && event.button.y > 350)
					||(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_LCTRL))) {
				GameMode = GAME;
				GameState.Reset();
				background.sprite.textureID = bT;
			}
			else if (GameMode == GAME) {
				if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_LCTRL && Player.active) {	
					for (GameObject& bullet : Bullets) {
						if (!bullet.active) {
							bullet.position = Vec3(Player.position);
							bullet.position.y += Player.scale.y + 0.05f;
							bullet.tag = PLAYER_BULLET;
							bullet.active = true;
							bullet.sprite.textureID = flT;
							if (bullet.velocity.y < 0) bullet.velocity.y *= -1;
							break;
						}
					}
				}
			}
		}

		if (GameMode == GAME) {
			const Uint8* keys = SDL_GetKeyboardState(NULL);
			if (keys[SDL_SCANCODE_LEFT] && Player.position.x - Player.scale.x / 2 > -3.0f) {
				Player.position.x -= PlayerSpeed * delta;
			}
			if (keys[SDL_SCANCODE_RIGHT] && Player.position.x + Player.scale.x / 2 < 3.0f) {
				Player.position.x += PlayerSpeed * delta;
			}
		}

		/*Update*/
		if (GameMode == GAME) {
			if ((HordeSpeed > 0.0f && Crates[0].position.x + 7 * 0.5f >= 2.5f)
				|| (HordeSpeed < 0.0f && Crates[0].position.x <= -2.5f)) {
				if (HordeSpeed < 0.0f)
					HordeSpeed = (float)(rand() % 5) * 0.1f + 0.3f;
				else
					HordeSpeed = -((float)(rand() % 5) * 0.1f + 0.3f);
			}

			for (GameObject& crate : Crates) {
				crate.position.x += HordeSpeed * delta;
			}

			for (GameObject& bullet : Bullets) {
				if (bullet.position.y > 4.0f || bullet.position.y < -4.0f) {
					bullet.active = false;
					bullet.position.y = 0.0f;
				}
				if (bullet.active) {
					bullet.Update(delta);
					
					if (bullet.tag == PLAYER_BULLET) {
						for (int i = 0; i < Crates.size(); i++) {
							if (Crates[i].active && Crates[i].ifCollide(bullet)) {
								Crates[i].active = false;
								bullet.active = false;

								for (Explosion& exp : Explosions) {
									if (!exp.active) {
										exp.position = Vec3(Crates[i].position);
										exp.Explode();
										break;
									}
								}
							}
						}
					}
					else if (bullet.tag == ENEMY_BULLET && Player.ifCollide(bullet)) {
						GameMode = OVER;
					}
				}
			}

			for (Explosion& exp : Explosions) {
				exp.Update(delta);
			}
		}

		/*Draw*/
		if (GameMode == MENU) {
			Draw(&program, background);
			DrawText(&program, fontTex, "Space Invader", 0.3f, 0.02f, Vec3(-1.9f, 1.2f, 0.0f));
			DrawText(&program, fontTex, "Attack of the Loot Crates", 0.1f, 0.0f, Vec3(0.1f, 1.0f, 0.0f));
			DrawText(&program, fontTex, "[CTRL to ENGAGE]", 0.2f, 0.0f, Vec3(-1.4f, 0.0f, 0.0f));
		}
		else if (GameMode == GAME) {
			Draw(&program, background);
			if (GameState.Player.active)
				Draw(&program, GameState.Player);
			for (GameObject& crate : Crates)
				if (crate.active)	Draw(&program, crate);
			for (GameObject& bullet : Bullets)
				if (bullet.active)	Draw(&program, bullet);
			for (Explosion& exp : Explosions)
				if (exp.active)	Draw(&program, exp);
		}
		else if (GameMode == OVER) {
			Draw(&program, background);
			if (GameState.Player.active)
				Draw(&program, GameState.Player);
			for (GameObject& crate : Crates)
				if (crate.active)	Draw(&program, crate);
			for (GameObject& bullet : Bullets)
				if (bullet.active)	Draw(&program, bullet);
			for (Explosion& exp : Explosions)
				if (exp.active)	Draw(&program, exp);

			DrawText(&program, fontTex, "Game Over", 0.3f, 0.02f, Vec3(-1.9f, 1.2f, 0.0f));
			DrawText(&program, fontTex, "[CTRL to REENGAGE]", 0.2f, 0.0f, Vec3(-1.4f, 0.0f, 0.0f));
		}


		//glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
