#include "Bezier.h"

Bezier::Bezier(const std::vector<point>& controlPoints, float speed)
	: controlPoints(controlPoints), speed(speed), t(0.f), forward(true)
{
}

glm::vec3 Bezier::getPosition(float dt)
{
	float delta = dt * speed;
	t += forward ? delta : -delta;

	if (t >= 1.f) {
		t = 1.f;
		forward = false;
	}
	if (t <= 0.f) {
		t = 0.f;
		forward = true;
	}

	point p = evaluate(t, std::list<point>(controlPoints.begin(), controlPoints.end()));
	return glm::vec3(p.x, p.y, p.z);
}

point Bezier::evaluate(float t, std::list<point> P)
{
	std::list<point>Q = P;
	
	while (Q.size() > 1) {
		std::list<point> R;
		auto it1 = Q.begin();
		auto it2 = std::next(Q.begin());

		while (it2 != Q.end()) {
			point p = ((1 - t) * (*it1)) + (t * (*it2));
			R.push_back(p);
			it1++;
			it2++;
		}
		Q.clear();
		Q = R;
	}

	return Q.front();
}
