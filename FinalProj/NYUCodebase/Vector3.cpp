#include "Vector3.h"


Vec3::Vec3(float xi, float yi, float zi) :x(xi), y(yi), z(zi) {}
Vec3::Vec3(const Vec3& copied) :x(copied.x), y(copied.y), z(copied.z) {}

void Vec3::applyDelta(Vec3& deltaVec, float delta) {
	x += deltaVec.x * delta;
	y += deltaVec.y * delta;
	z += deltaVec.z * delta;
}
