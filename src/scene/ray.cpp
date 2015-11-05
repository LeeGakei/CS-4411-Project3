#include "ray.h"
#include "material.h"
#include "scene.h"

const Material &
isect::getMaterial() const
{
    return material ? *material : obj->getMaterial();
}

ray ray::getReflectDirection(isect intersection)const{
	double t = intersection.t;
	vec3f p = this->getPosition();
	vec3f d = this->getDirection();
	vec3f intersectPoint(p[0] + d[0] * t, p[1] + d[1] * t, p[2] + d[2] * t);

	vec3f a = d;
	vec3f b = intersection.N;
	vec3f c = a - a.dot(b) * b;
	vec3f out = b - c;

	return ray(intersectPoint, out.normalize());
};
