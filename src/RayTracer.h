#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/scene.h"
#include "scene/ray.h"

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, double x, double y );
	vec3f traceRay( Scene *scene, const ray& r, const vec3f& thresh, int depth,vec3f cumulativeK);


	void getBuffer( unsigned char *&buf, int &w, int &h );
	double aspectRatio();
	void traceSetup( int w, int h );
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );
	Scene *getScene();

	bool loadScene( char* fn );

	bool sceneLoaded();

private:
	unsigned char *buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	Scene *scene;

	bool m_bSceneLoaded;

	ray getReflectDirection(ray input, isect intersection);

	ray getRetractionDirection(const ray input,const isect intersection,double n_i, double n_t);

	bool isEnteringObject(ray input, isect intersection);

	bool notTIR(ray input, isect intersection, double n_i,double n_t);
};

#endif // __RAYTRACER_H__
