#include <cmath>

#include "light.h"
#include "../RayTracer.h"
#include "../ui/TraceUI.h"

extern RayTracer* theRayTracer;
extern TraceUI* traceUI;

double DirectionalLight::distanceAttenuation(const vec3f& P) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	Scene * scene = theRayTracer->getScene();
	vec3f result(1, 1, 1);
	bool hasShadow = false;
	isect i;
	//from the P to the light direction, we shoot a ray and if there is no intersection, there will be no shadow
	ray r(P, -orientation);

	while (scene->intersect(r, i) && i.t > RAY_EPSILON){
		//transparent!
		if (i.getMaterial().kt.length_squared() != 0){
			result[0] *= i.getMaterial().kt[0];
			result[1] *= i.getMaterial().kt[1];
			result[2] *= i.getMaterial().kt[2];

			ray newR(r.at(i.t),r.getDirection());
			r = newR;
		}
		else{
			hasShadow = true;
			break;
		}
	}

	if (hasShadow){
		return vec3f(0, 0, 0);
	}
	else{
		//printf("%lf,%lf,%lf/n", result[0], result[1], result[2]);
		return result;
	}
}

vec3f DirectionalLight::getColor(const vec3f& P) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection(const vec3f& P) const
{
	return -orientation;
}

double PointLight::distanceAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
	double result = 0.0;
	vec3f p = this->position;
	double d = sqrt(pow(p[0] - P[0], 2) + pow(p[1] - P[1], 2) + pow(p[2] - P[2], 2));
	double a = this->constant_attenuation_coeff;
	double b = this->linear_attenuation_coeff;
	double c = this->quadratic_attenuation_coeff;
	if (traceUI->usingUISetting){
		a *= traceUI->m_nAttConstant;
		b *= traceUI->m_nAttLinear;
		c *= traceUI->m_nAttQuadratic;
	}

	result = 1.0 / (a + b * d + c * d * d);

	return min(1.0, result);
}

vec3f PointLight::getColor(const vec3f& P) const
{
	// Color doesn't depend on P 
	return color;
}

//pay attention here,this direction is from P to the pointLight
vec3f PointLight::getDirection(const vec3f& P) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.

	//from the pointLight to P, we shoot a ray, if there is any intersection between P and light, there is shadow
	Scene * scene = theRayTracer->getScene();
	vec3f result(1, 1, 1);
	bool hasShadow = false;
	isect i;
	ray r(position, -this->getDirection(P));

	while (scene->intersect(r, i) && i.t > RAY_EPSILON &&
		threePointsIsInAGoodSeqence(r.getPosition(), r.at(i.t), P)){
		//transparent!
		if (i.getMaterial().kt.length_squared() != 0){
			result[0] *= i.getMaterial().kt[0];
			result[1] *= i.getMaterial().kt[1];
			result[2] *= i.getMaterial().kt[2];

			ray newR(r.at(i.t), r.getDirection());
			r = newR;
		}
		else{
			hasShadow = true;
			break;
		}
	}

	if (hasShadow){
		return vec3f(0, 0, 0);
	}
	else{
		//printf("%lf,%lf,%lf/n", result[0], result[1], result[2]);
		return result;
	}


	//if (scene->intersect(r, i)){
	//	vec3f intersection = r.at(i.t);
	//	vec3f l_to_i1 = r.at(i.t) - position;
	//	vec3f il_to_P = P - r.at(i.t);
	//	vec3f l_to_P = P - position;

	//	//if it is the same point
	//	if (abs(il_to_P[0]) < RAY_EPSILON && abs(il_to_P[1]) < RAY_EPSILON && abs(il_to_P[2]) < RAY_EPSILON){
	//		return vec3f(1,1,1);
	//	}

	//	if (l_to_i1.dot(il_to_P) > 0 && l_to_i1.dot(l_to_P) > 0){
	//		//there is shadow
	//		return vec3f(0,0,0);
	//	}
	//	return vec3f(1,1,1);
	//}

	return vec3f(0,0,0);
}

//check if it is in a sequence and the last two point is not the same point
bool PointLight::threePointsIsInAGoodSeqence(const vec3f& p1, const vec3f& p2, const vec3f& p3)const{
	vec3f v1_to_2 = p2 - p1;
	vec3f v2_to_3 = p3 - p2;
	if (abs(v2_to_3[0]) < RAY_EPSILON  && abs(v2_to_3[1]) < RAY_EPSILON  && abs(v2_to_3[2]) < RAY_EPSILON){
		return false;
	}
	return v1_to_2.dot(v2_to_3) > 0;
}



//none of theses really matter
double AmbientLight::distanceAttenuation(const vec3f& P) const
{
	return 1.0;
}

vec3f AmbientLight::getColor(const vec3f& P) const
{
	return color;
}

vec3f AmbientLight::getDirection(const vec3f& P) const
{
	return vec3f(0, 0, 0);
}


vec3f AmbientLight::shadowAttenuation(const vec3f& P) const
{
	return vec3f(1,1,1);
}

