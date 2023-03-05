#include <windows.h>

Vec2
createVec2 (float x, float y) {
	Vec2 vec;
	vec.x = x;
	vec.y = y;
	return vec;
}

Vec3
createVec3 (float x, float y, float z) {
	Vec3 vec;
	vec.x = x;
	vec.y = y;
	vec.z = z;
	return vec;
}

bool
vec4ContainsVec2 (Vec4 box, Vec2 location) {
	return location.x > box.x && location.x < box.y && location.y > box.z && location.y < box.w;
}
