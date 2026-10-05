#pragma once

#include <list>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>

#include "point.h"

class Bezier
{
public:
	
	Bezier(const std::vector<point>& controlPoints, float speed);

	glm::vec3 getPosition(float dt);

private:
	float t; // current time
	float speed;
	bool forward; // movement direction
	std::vector<point> controlPoints;

	point evaluate(float t, std::list<point> P);


};

