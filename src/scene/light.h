#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"

class Light
	: public SceneElement
{
public:
	virtual double shadowAttenuation(const vec3f& P) const = 0;
	virtual double distanceAttenuation( const vec3f& P ) const = 0;
	virtual vec3f getColor( const vec3f& P ) const = 0;
	virtual vec3f getDirection( const vec3f& P ) const = 0;

	string		kind;

protected:
	Light( Scene *scene, const vec3f& col )
		: SceneElement( scene ), color( col ) {}

	vec3f 		color;

};

class DirectionalLight
	: public Light
{
public:
	DirectionalLight( Scene *scene, const vec3f& orien, const vec3f& color )
		: Light(scene, color), orientation(orien) {
		kind = "DirectionalLight";
	}
	virtual double shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f 		orientation;
};

class PointLight
	: public Light
{
public:
	PointLight( Scene *scene, const vec3f& pos, const vec3f& color )
		: Light( scene, color ), position( pos ) {
		kind = "PointLight";
	}
	PointLight(Scene *scene, const vec3f& pos, const vec3f& color,double constant_,double linear_,double quadratic_)
		: Light(scene, color), position(pos), constant_attenuation_coeff(constant_), linear_attenuation_coeff(linear_), quadratic_attenuation_coeff(quadratic_){
		kind = "PointLight";
	}
	virtual double shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

	//init for avoiding divide by zero
	double constant_attenuation_coeff = 1;
	double linear_attenuation_coeff = 0;
	double quadratic_attenuation_coeff = 0;

protected:
	vec3f position;
};

class AmbientLight
	: public Light
{
public:
	AmbientLight(Scene *scene,  const vec3f& color)
		: Light(scene, color){
		kind = "AmbientLight";
	}

	//none of these really matter
	virtual double shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;
};

#endif // __LIGHT_H__
