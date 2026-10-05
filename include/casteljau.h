#include <list>
#include <vector>
#include <algorithm>

#include "point.h"

point evaluate(float t, std::list<point> P)
{
	std::list<point>Q = P;
	
	while (Q.size() > 1) {
		std::list<point> R;
		auto it1 = Q.begin();
		auto it2 = std::next(Q.begin());

		while (it2 != Q.end()) {
			point p = (1 - t) * (*it1) + t * (*it2);
			R.push_back(p);
			it1++;
			it2++;
		}
		Q.clear();
		Q = R;
	}

	return Q.front();
}

std::vector<point> EvaluateBezierCurve(std::vector<point>ctrl_points, int num_evaluations)
{
	std::list<point> ps(ctrl_points.begin(), ctrl_points.end());
	std::vector<point> curve;
	
	for (int i = 0; i <= num_evaluations; i++) {
		float t = (float)i / (float)num_evaluations;
		curve.push_back(evaluate(t, ps));
	}
	
	return curve;
}

float* MakeFloatsFromVector(std::vector<point> curve, int &num_verts, int &num_floats, float r, float g, float b)
{
	return NULL;
}

