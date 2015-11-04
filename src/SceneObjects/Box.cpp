#include <cmath>
#include <assert.h>

#include "Box.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.
	vec3f p = r.getPosition();
	vec3f d = r.getDirection();

	double t = 0.0;
	bool haveIntersection = false;
	//box is the intersection of 3 slabs
	//1. x = 0.5 or -0.5
	if (d[0] != 0){
		double _t = (0.5-p[0]) / d[0];
		if (_t > RAY_EPSILON){
			double y = p[1] + _t * d[1];
			double z = p[2] + _t * d[2];
			if (-0.5 <= y && y <= 0.5 && -0.5 <= z && z <= 0.5){
				if (!haveIntersection){
					haveIntersection = true;
					t = _t;
					i.obj = this;
					i.N = vec3f(1, 0, 0);
				}
				else if(_t < t){
					t = _t;
					i.N = vec3f(1, 0, 0);
				}
			}
		}

		_t = (-0.5 - p[0]) / d[0];
		if (_t > RAY_EPSILON){
			double y = p[1] + _t * d[1];
			double z = p[2] + _t * d[2];
			if (-0.5 <= y && y <= 0.5 && -0.5 <= z && z <= 0.5){
				if (!haveIntersection){
					haveIntersection = true;
					t = _t;
					i.obj = this;
					i.N = vec3f(-1, 0, 0);
				}
				else if (_t < t){
					t = _t;
					i.N = vec3f(-1, 0, 0);
				}
			}
		}
	}

	//2. y = 0.5 or -0.5
	if (d[1] != 0){
		double _t = (0.5 - p[1]) / d[1];
		if (_t > RAY_EPSILON){
			double x = p[0] + _t * d[0];
			double z = p[2] + _t * d[2];
			if (-0.5 <= x && x <= 0.5 && -0.5 <= z && z <= 0.5){
				if (!haveIntersection){
					haveIntersection = true;
					t = _t;
					i.obj = this;
					i.N = vec3f(0, 1, 0);
				}
				else if (_t < t){
					t = _t;
					i.N = vec3f(0, 1, 0);
				}
			}
		}

		_t = (-0.5 - p[1]) / d[1];
		if (_t > RAY_EPSILON){
			double x = p[0] + _t * d[0];
			double z = p[2] + _t * d[2];
			if (-0.5 <= x && x <= 0.5 && -0.5 <= z && z <= 0.5){
				if (!haveIntersection){
					haveIntersection = true;
					t = _t;
					i.obj = this;
					i.N = vec3f(0, -1, 0);
				}
				else if (_t < t){
					t = _t;
					i.N = vec3f(0, -1, 0);
				}
			}
		}
	}

	//3. z = 0.5 or -0.5
	if (d[2] != 0){
		double _t = (0.5 - p[2]) / d[2];
		if (_t > RAY_EPSILON){
			double x = p[0] + _t * d[0];
			double y = p[1] + _t * d[1];
			if (-0.5 <= x && x <= 0.5 && -0.5 <= y && y <= 0.5){
				if (!haveIntersection){
					haveIntersection = true;
					t = _t;
					i.obj = this;
					i.N = vec3f(0, 0, 1);
				}
				else if (_t < t){
					t = _t;
					i.N = vec3f(0, 0, 1);
				}
			}
		}

		_t = (-0.5 - p[2]) / d[2];
		if (_t > RAY_EPSILON){
			double x = p[0] + _t * d[0];
			double y = p[1] + _t * d[1];
			if (-0.5 <= x && x <= 0.5 && -0.5 <= y && y <= 0.5){
				if (!haveIntersection){
					haveIntersection = true;
					t = _t;
					i.obj = this;
					i.N = vec3f(0, 0, -1);
				}
				else if (_t < t){
					t = _t;
					i.N = vec3f(0, 0, -1);
				}
			}
		}
	}

	return haveIntersection;
}
