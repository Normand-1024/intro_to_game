#pragma once

struct Vec3 {
	Vec3(float xi = 0, float yi = 0, float zi = 0);
	Vec3(const Vec3& copied);

	void applyDelta(Vec3& deltaVec, float delta);
	float x, y, z;
};
