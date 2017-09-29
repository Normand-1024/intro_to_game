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

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define PI 3.14159265

SDL_Window* displayWindow;

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

class Collider {
public:
	Collider(char typeIn, float wIn, float hIn) :type(typeIn), w(wIn), h(hIn) {}

	void Update(float xIn, float yIn) {
		x = xIn;
		y = yIn;
	}

	bool ifCollide(Collider& theOther) {
		//box
		if (type == 'b') {
			if (theOther.type = 'b') {
				bool	a = (y - h / 2) > (theOther.y + theOther.h / 2),
					b = (y + h / 2) < (theOther.y - theOther.h / 2),
					c = (x - w / 2) > (theOther.x + theOther.w / 2),
					d = (x + w / 2) < (theOther.x - theOther.w / 2);
				
				return !(a || b || c || d) || (!a && !b && !c && !d);
			}
		}
		//circle
		else if (type == 'c') {
			if (theOther.type = 'c')
				return sqrt(exp2(x - theOther.x) + exp2(y - theOther.y)) < w + theOther.w;
		}
	}
private:
	float x, y, w, h;
	/*
		type: 'b' = box, 'c' = circle
	*/
	char type;
};

struct GameObject {
	GameObject(float xI, float yI, float widthI, float heightI, GLuint textureI, char type) :x(xI), y(yI), w(widthI), h(heightI), textureID(textureI), collider(type, w, h) {
		matrix.Scale(w / 2, h / 2, 0.0f);
		matrix.Translate(x, y, 0.0f);
	}

	void Update(ShaderProgram* p, float delta) {
		if (velocity != 0.0f) {
			x += velocity * cos(angle * PI / 180) * delta;
			y += velocity * sin(angle * PI / 180) * delta;
		}

		collider.Update(x, y);
	}

	void Draw(ShaderProgram* p) {
		matrix.Identity();
		matrix.Translate(x, y, 0.0f);
		matrix.Scale(w / 2, h / 2, 0.0f);

		glBindTexture(GL_TEXTURE_2D, textureID);

		//float vertices[] = { x-w/2, y+h/2, x-w/2, y-h/2, x+w/2, y+h/2,		x+w/2, y+h/2, x-w/2, y-h/2, x+w/2, y-h/2};
		float vertices[] = {-1, 1, -1, -1, 1, 1,	1, 1, -1, -1, 1, -1};
		glVertexAttribPointer(p->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(p->positionAttribute);

		float texCoords[] = { 0.0, 0.0, 0.0, 1.0, 1.0, 0.0,		1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
		glVertexAttribPointer(p->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(p->texCoordAttribute);
		p->SetModelviewMatrix(matrix);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(p->positionAttribute);
		glDisableVertexAttribArray(p->texCoordAttribute);
	}

	float x, y;
	float w, h;
	float velocity = 0.0f, angle = 0.0f;
	Matrix matrix;
	GLuint textureID;
	Collider collider;
};

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Ping Pong Golden Deluxe Edtion With Premium Pass Silver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	glViewport(0, 0, 1024, 768);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	GLuint pad_tex = LoadTexture(RESOURCE_FOLDER"p.png");
	GLuint ball_tex = LoadTexture(RESOURCE_FOLDER"ba.png");
	GLuint background_tex = LoadTexture(RESOURCE_FOLDER"b.png");

	GameObject background = GameObject(0.0f, 0.0f, 8.0f, 6.0f, background_tex, 'b');
	GameObject paddleLeft = GameObject(-3.5f, 0.0f, 0.2f, 1.0f, pad_tex, 'b');
	GameObject paddleRight = GameObject(3.5f, 0.0f, 0.2f, 1.0f, pad_tex, 'b');
	GameObject ball = GameObject(0.0f, 0.0f, 0.2f, 0.2f, ball_tex, 'b');

	ball.velocity = 8.0f;
	ball.angle = ball.angle = (float)(rand() % 360);;
	float pad_vel = 13.0f;

	float lastFrameTicks = 0.0f;
	float last_ball_reset = 0.0f;

	Matrix projectionMatrix;
	projectionMatrix.SetOrthoProjection(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);
	Matrix modelviewMatrix;
	program.SetModelviewMatrix(modelviewMatrix);
	program.SetProjectionMatrix(projectionMatrix);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SDL_Event event;
	bool done = false;
	while (!done) {
		glUseProgram(program.programID);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float delta = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
				done = true;
			else if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_R) {
				ball.x = 0.0f;
				ball.y = 0.0f;
				last_ball_reset = ticks;
				ball.angle = (float)(rand() % 360);
			}
		}

		const Uint8* keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_UP]) {
			if (paddleRight.y + paddleRight.h / 2 < 2.9f)
				paddleRight.y += pad_vel * delta;
		}
		if (keys[SDL_SCANCODE_DOWN]) {
			if (paddleRight.y - paddleRight.h / 2 > -2.9f)
				paddleRight.y -= pad_vel * delta;
		}
		if (keys[SDL_SCANCODE_W]) {
			if (paddleLeft.y + paddleRight.h / 2 < 2.9f)
				paddleLeft.y += pad_vel * delta;
		}
		if (keys[SDL_SCANCODE_S]) {
			if (paddleLeft.y - paddleRight.h / 2 > -2.9f)
				paddleLeft.y -= pad_vel * delta;
		}

		if (ticks - last_ball_reset >= 2.0f)
			ball.Update(&program, delta);

		paddleLeft.Update(&program, delta);
		paddleRight.Update(&program, delta);

		if (ball.y + ball.h / 2 >= 2.9f || ball.y - ball.h / 2 <= -2.9f) {
			ball.y *= 1 - ball.h / 4.0f;
			if (ball.angle >= 0.0f && ball.angle <= 180.0f)
				ball.angle = 360.0f - ball.angle;
			else if (ball.angle >= 180.0f && ball.angle <= 360.0f)
				ball.angle = 180.0f - (ball.angle - 180.0f);
		}

		if (paddleLeft.collider.ifCollide(ball.collider) || paddleRight.collider.ifCollide(ball.collider)) {
			if (abs(ball.x) >= paddleRight.x - paddleRight.w / 3 && abs(ball.x) <= paddleRight.x + paddleRight.w / 3) {
				if (ball.angle >= 0.0f && ball.angle <= 180.0f)
					ball.angle = 360.0f - ball.angle;
				else if (ball.angle >= 180.0f && ball.angle <= 360.0f)
					ball.angle = 180.0f - (ball.angle - 180.0f);
			}
			else {
				ball.x *= 1 - 0.05 / 4.0f;
				if (ball.angle >= 0.0f && ball.angle <= 90.0f)
					ball.angle = 180.0f - ball.angle;
				else if (ball.angle >= 90.0f && ball.angle <= 180.0f)
					ball.angle = -(ball.angle - 180.0f);
				else if (ball.angle >= 180.0f && ball.angle <= 270.0f)
					ball.angle = 540.0f - ball.angle;
				else if (ball.angle >= 270.0f && ball.angle <= 360.0f)
					ball.angle = 180.0f - (ball.angle - 360.0f);
			}
		}

		if (ball.x < -4.1f || ball.x > 4.1f) {
			ball.x = 0.0f;
			ball.y = 0.0f;
			last_ball_reset = ticks;
			ball.angle = (float)(rand() % 360);
		}

		background.Draw(&program);
		paddleLeft.Draw(&program);
		paddleRight.Draw(&program);
		ball.Draw(&program);

		//glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
