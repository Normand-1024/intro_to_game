
#include "GraphicManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

void GMDraw(ShaderProgram* p, GameObject& object) {
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

void DrawText(ShaderProgram* p, int fontTexture, std::string text, float size, float spacing, const Vec3& pos, bool selected) {
	Matrix matrix;
	matrix.Identity();

	if (selected) {
		matrix.Translate(pos.x - size - spacing, pos.y, 0.0f);
		std::string new_text = ">";
		new_text.append(text);
		new_text.append("<");
		text = new_text;
	}
	else {
		matrix.Translate(pos.x, pos.y, 0.0f);
	}

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

		vp += 12;
		tp += 12;
	}
}