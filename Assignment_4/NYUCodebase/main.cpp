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

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>


#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define PI 3.14159265
#define TIMESTEP 0.00833333333
#define TILE_SIZE 15.0f

SDL_Window* displayWindow;

int mapWidth, mapHeight;
float startLocX, startLocY;
int** levelData;

float lerp(float v_ini, float v_fin, float t_acc) {
	return (1.0 - t_acc)*v_ini + t_acc*v_fin;
}

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

class SheetSprite {
public:
	SheetSprite(unsigned int tI) :textureID(tI), u(0.0f), v(0.0f), width(1.0f), height(1.0f), size(1.0f) {}
	SheetSprite(unsigned int tI, float uI, float vI, float wI, float hI, float sI) :textureID(tI), u(uI), v(vI), width(wI), height(hI), size(sI) {}
	SheetSprite(const SheetSprite& copied) :textureID(copied.textureID), u(copied.u), v(copied.v), width(copied.width), height(copied.height), size(copied.size) {}

	float size;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
};

enum ObjTag { PLAYER, STAR, STATIC};
enum ColliderType { BOX, CIRCLE, POINT, NONE };
enum GameMode { GAME, OVER };

struct GameObject {
	GameObject(Vec3& pos, Vec3& sc, GLuint textureI, ColliderType type) :position(pos), rotation(), scale(sc), sprite(textureI), cType(type){}
	GameObject(Vec3& pos, Vec3& sc, SheetSprite& spriteS, ColliderType type) :position(pos), rotation(), scale(sc), sprite(spriteS), cType(type) {}
	GameObject(const GameObject& copied) :position(copied.position), rotation(), scale(copied.scale), sprite(copied.sprite), cType(copied.cType), tag(copied.tag) {}
	GameObject() :sprite(SheetSprite(0)){}

	void Update(float delta) {
		velocity.applyDelta(acceleration, delta);
		position.applyDelta(velocity, delta);

		if ((collideBottom || collideTop) && velocity.y != 0.0f) velocity.y = 0.0f;
		if (velocity.x != 0.0f) velocity.x = lerp(velocity.x, 0.0f, 4.0f * delta);
		if (tag == PLAYER && acceleration.y != -100.0f) acceleration.y = lerp(acceleration.y, -100.0f, 100.0f * delta);
	}

	void setUV(int index, int tile_per_row = 30, float img_w = 692.0f, float img_h = 692.0f, float tile_w = 21.0f, float tile_h = 21.0f, float gap = 2.0f) {
		sprite.u = ((index % tile_per_row) * tile_w + (gap * ((index % tile_per_row ) + 1))) / img_w;
		sprite.v = ((index / tile_per_row) * tile_h + (gap * ((index / tile_per_row) + 1))) / img_h;
		sprite.width = tile_w / img_w;
		sprite.height = tile_h / img_h;
	}

	bool ifCollide(GameObject& theOther) {
		float x = position.x, y = position.y, w = scale.x, h = scale.y, ox = theOther.position.x, oy = theOther.position.y, ow = theOther.scale.x, oh = theOther.scale.y;

		if (cType == NONE || theOther.cType == NONE) return false;

		if (cType == BOX) {
			if (theOther.cType == BOX) {
				bool	a = (y - h / 2) > (oy + oh / 2),
					b = (y + h / 2) < (oy - oh / 2),
					c = (x - w / 2) > (ox + ow / 2),
					d = (x + w / 2) < (ox - ow / 2);

				return !a && !b && !c && !d;
			}
			else if (theOther.cType == POINT) {
				bool	a = (y - h / 2) < oy,
					b = (y + h / 2) > oy,
					c = (x - w / 2) < ox,
					d = (x + w / 2) > ox;
				return a && b && c && d;
			}
		}
	
		else if (cType == CIRCLE) {
			if (theOther.cType == CIRCLE)
				return sqrt(exp2(x - ox) + exp2(y - oy)) < w + ow;
			else if (theOther.cType == POINT)
				return sqrt(exp2(x - ox) + exp2(y - oy)) < w;
		}

		return theOther.ifCollide(*this);
	}

	void checkLevelCollide() {
		Vec3 right(position.x + scale.x / 2, position.y), 
			left(position.x - scale.x / 2, position.y), 
			bottom(position.x, position.y - scale.y / 2), 
			top(position.x, position.y + scale.y / 2);

		if ((int)(-top.y / TILE_SIZE) < 0)
			collideTop = true;
		else
			collideTop = (levelData[(int)(-top.y / TILE_SIZE)][(int)(top.x / TILE_SIZE)] != 0);
		if ((int)(-bottom.y / TILE_SIZE) >= 50)
			collideBottom = true;
		else
			collideBottom = (levelData[(int)(-bottom.y/ TILE_SIZE)][(int)(bottom.x / TILE_SIZE)] != 0);
		if ((int)(left.x / TILE_SIZE) < 0)
			collideLeft = true;
		else
			collideLeft = (levelData[(int)(-left.y / TILE_SIZE)][(int)(left.x / TILE_SIZE)] != 0);
		if ((int)(right.x / TILE_SIZE) >= 50)
			collideRight = true;
		else
			collideRight = (levelData[(int)(-right.y / TILE_SIZE)][(int)(right.x / TILE_SIZE)] != 0);

		if (collideTop || collideBottom)
			position.y = TILE_SIZE * ((int)(position.y / TILE_SIZE)) - TILE_SIZE / 2;
		if (collideLeft || collideRight)
			position.x = TILE_SIZE * ((int)(position.x / TILE_SIZE)) + TILE_SIZE / 2;
	}

	Vec3 position;
	Vec3 rotation;
	Vec3 scale;
	Vec3 velocity;
	Vec3 acceleration;

	SheetSprite sprite;
	ColliderType cType;

	ObjTag tag;
	bool isStatic = true, active = true;

	bool collideTop = false, collideBottom = false, collideLeft = false, collideRight = false;
};

void Draw(ShaderProgram* p, GameObject& object, Vec3& displacement) {
	Matrix matrix;
	matrix.Identity();
	matrix.Translate(object.position.x, object.position.y, 0.0f);
	//Camera Movement
	
	if (displacement.x < 0.0f)	displacement.x = 0.0f;
	else if (displacement.x > 550.0f) displacement.x = 550.0f;

	if (displacement.y > 0.0f)	displacement.y = -0.0f;
	else if (displacement.y < -550.0f)	displacement.y = -550.0f;
	matrix.Translate(-displacement.x, -displacement.y, 0.0f);

	matrix.Scale(object.scale.x/2, object.scale.y/2, 0.0f);

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

/*
==================================
GAME DATA
==================================
*/
std::vector<GameObject> default_objects(0);
std::vector<GameObject> game_objects(0);

GLuint spriteTex = LoadTexture(RESOURCE_FOLDER"spritesheet_rgba.png");

void placeEntity(std::string type, float placeX, float placeY) {
	spriteTex = LoadTexture(RESOURCE_FOLDER"spritesheet_rgba.png");
	GameObject obj = GameObject(Vec3(placeX + TILE_SIZE / 2, placeY + TILE_SIZE / 2, 0.0f), Vec3(TILE_SIZE, TILE_SIZE, 1.0f), spriteTex, BOX);
	if (type.compare("Start") == 0) {
		obj.tag = PLAYER;
		obj.setUV(20);
		//obj.acceleration = Vec3(0.0f, -9.8f, 0.0f);
		startLocX = obj.position.x;
		startLocY = obj.position.y;
	}
	else if (type.compare("star") == 0) {
		obj.tag = STAR;
		obj.setUV(106);
	}
	default_objects.push_back(obj);
}

void Render(ShaderProgram* p, GLint spriteTex) {
	Vec3 playerPos = game_objects[0].position;
	playerPos.x = playerPos.x - 100.0f;
	playerPos.y = playerPos.y + 100.f;

	GLuint backgroundTexture = LoadTexture(RESOURCE_FOLDER"backgrounds.png");
	GameObject background(Vec3(375.0f, -375.0f, 0.0f), Vec3(750.0f, 750.0f, 1.0f), backgroundTexture, NONE);
	Draw(p, background, playerPos);

	for (int y = 1; y <= mapHeight; ++y) {
		for (int x = 1; x <= mapWidth; ++x) {
			if (levelData[y-1][x-1] != 0) {
				GameObject obj = GameObject(Vec3((x - 1) * TILE_SIZE + TILE_SIZE / 2, (y - 1) * -TILE_SIZE - TILE_SIZE / 2, 0.0f), Vec3(TILE_SIZE, TILE_SIZE, 1.0f), spriteTex, BOX);
				obj.tag = STATIC;
				obj.setUV(levelData[y - 1][x - 1]);
				Draw(p, obj, playerPos);
			}
		}
	}
	for (GameObject& obj : game_objects) {
		if (obj.active) Draw(p, obj, playerPos);
	}
}

/*
==================================
READING THE MAP DATA
==================================
*/


bool readHeader(std::ifstream &stream) {
	std::string line;
	mapWidth = -1;
	mapHeight = -1;
	while (getline(stream, line)) {
		if (line == "") { break; }
		std::istringstream sStream(line);
		std::string key, value;
		std::getline(sStream, key, '=');
		std::getline(sStream, value);
		if (key == "width") {
			mapWidth = atoi(value.c_str());
		}
		else if (key == "height") {
			mapHeight = atoi(value.c_str());
		}
	}
	if (mapWidth == -1 || mapHeight == -1) {
		return false;
	}
	else { // allocate our map data
		levelData = new int*[mapHeight];
		for (int i = 0; i < mapHeight; ++i) {
			levelData[i] = new int[mapWidth];
		}
		return true;
	}
}

bool readLayerData(std::ifstream &stream) {
	std::string line;
	while (std::getline(stream, line)) {
		if (line == "") { break; }
		std::istringstream sStream(line);
		std::string key, value;
		std::getline(sStream, key, '=');
		std::getline(sStream, value);
		if (key == "data") {
			for (int y = 0; y < mapHeight; y++) {
				getline(stream, line);
				std::istringstream lineStream(line);
				std::string tile;
				for (int x = 0; x < mapWidth; x++) {
					std::getline(lineStream, tile, ',');
					int val = atoi(tile.c_str());
					if (val > 0) {
						// be careful, the tiles in this format are indexed from 1 not 0
						levelData[y][x] = val - 1;
					}
					else {
						levelData[y][x] = 0;
					}
				}
			}
		}
	}
	return true;
}

bool readEntityData(std::ifstream &stream) {
	std::string line;
	std::string type;
	while (std::getline(stream, line)) {
		if (line == "") { break; }
		std::istringstream sStream(line);
		std::string key, value;
		std::getline(sStream, key, '=');
		std::getline(sStream, value);
		if (key == "type") {
			type = value;
		}
		else if (key == "location") {
			std::istringstream lineStream(value);
			std::string xPosition, yPosition;
			std::getline(lineStream, xPosition, ',');
			std::getline(lineStream, yPosition, ',');
			float placeX = atoi(xPosition.c_str())*TILE_SIZE;
			float placeY = atoi(yPosition.c_str())*-TILE_SIZE;
			placeEntity(type, placeX, placeY);
		}
	}
	return true;
}

void readFile() {
	std::ifstream infile("mapdata.txt");
	std::string line;
	while (std::getline(infile, line)) {
		if (line == "[header]") {
			if (!readHeader(infile)) {
				assert(false);
			}
		}
		else if (line == "[layer]") {
			readLayerData(infile);
		}
		else if (line == "[Object Layer 1]") {
			readEntityData(infile);
		}
	}
}
/*
==========================================
==========================================
*/

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("A Space Odyssey", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 750, 750, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	glViewport(0, 0, 750, 750);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	float lastFrameTicks = 0.0f;
	float accumulator = 0.0f;

	Matrix projectionMatrix;
	projectionMatrix.SetOrthoProjection(0.0f, 200.0f, -200.0f, 0.0f, -1.0f, 1.0f);
	Matrix modelviewMatrix;
	program.SetModelviewMatrix(modelviewMatrix);
	program.SetProjectionMatrix(projectionMatrix);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*Game Actors*/
	GameMode GameMode = GAME;
	readFile();
	game_objects = std::vector<GameObject>(0);
	for (GameObject& obj : default_objects)
		game_objects.push_back(GameObject(obj));

	GameObject& player = game_objects[0];

	SDL_Event event;
	bool done = false;
	while (!done) {
		glUseProgram(program.programID);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float delta = ticks - lastFrameTicks;
		float physicsDelta = delta + accumulator;
		lastFrameTicks = ticks;

		/*Process Input*/
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
				done = true;
			else if (GameMode == GAME && event.key.keysym.scancode == SDL_SCANCODE_SPACE && player.collideBottom) {
				player.acceleration.y = 100000.0f;
			}
		}

		if (GameMode == GAME) {
			const Uint8* keys = SDL_GetKeyboardState(NULL);
			if (keys[SDL_SCANCODE_LEFT])	player.acceleration.x = -500.0f;

			if (keys[SDL_SCANCODE_RIGHT])	player.acceleration.x = 500.0f;

			if (!keys[SDL_SCANCODE_LEFT] && !keys[SDL_SCANCODE_RIGHT]) player.acceleration.x = 0.0f;
		}

		/*Update*/
		if (physicsDelta < TIMESTEP) {
			accumulator = physicsDelta;
			continue; // skip below code to the next iteration
		}
		while (physicsDelta >= TIMESTEP) {
			for (GameObject& obj : game_objects) {
				obj.Update(TIMESTEP);

				if (obj.tag == STAR && obj.ifCollide(player)) {
					obj.active = false;
				}
				else if (obj.tag == PLAYER) {
					obj.checkLevelCollide();
				}
			}
			physicsDelta -= TIMESTEP;
		}

		physicsDelta = delta;

		/*Draw*/
		Render(&program, spriteTex);

		//glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
