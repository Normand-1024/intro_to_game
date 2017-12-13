#pragma once
#include "Vector3.h"

enum ColliderType { BOX, POINT, NONE };
enum ObjTag { PLAYER, PLAYER2, OGREY, OBLUE, ORED, REMOVE};

class SheetSprite {
public:
	SheetSprite(unsigned int tI);
	SheetSprite(unsigned int tI, float uI, float vI, float wI, float hI, float sI);

	float size;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
};

struct GameObject {
	GameObject(Vec3& pos, Vec3& sc, unsigned int textureI, ColliderType type);
	GameObject();

	void Update(float delta);

	bool ifCollide(GameObject& theOther);

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