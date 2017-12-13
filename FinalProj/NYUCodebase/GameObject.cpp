#include "GameObject.h"

SheetSprite::SheetSprite(unsigned int tI) :textureID(tI), u(0.0f), v(0.0f), width(1.0f), height(1.0f), size(1.0f) {}

SheetSprite::SheetSprite(unsigned int tI, float uI, float vI, float wI, float hI, float sI) :textureID(tI), u(uI), v(vI), width(wI), height(hI), size(sI) {}

GameObject::GameObject(Vec3& pos, Vec3& sc, unsigned int textureI, ColliderType type) :position(pos), rotation(), scale(sc), sprite(textureI), cType(type) {}
GameObject::GameObject() :sprite(SheetSprite(0)) {}

void GameObject::Update(float delta) {
	velocity.applyDelta(acceleration, delta);
	position.applyDelta(velocity, delta);
}

bool GameObject::ifCollide(GameObject& theOther) {
	float x = position.x, y = position.y, w = scale.x, h = scale.y, ox = theOther.position.x, oy = theOther.position.y, ow = theOther.scale.x, oh = theOther.scale.y;

	if (cType == NONE || theOther.cType == NONE) return false;

	bool	a = (y - h / 2) > (oy + oh / 2) - 0.2f,
		b = (y + h / 2) < (oy - oh / 2) + 0.2f,
		c = (x - w / 2) > (ox + ow / 2) - 0.2f,
		d = (x + w / 2) < (ox - ow / 2) + 0.2f;

	return !a && !b && !c && !d;
}
