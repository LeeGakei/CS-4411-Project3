// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "ui/TraceUI.h"

extern TraceUI* traceUI;
extern int global_depth;

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	return traceRay(scene, r, vec3f(1.0, 1.0, 1.0), traceUI->getDepth()).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth )
{
	//for debug,we can see how many times the recursive run.
	if (depth < global_depth){
		global_depth = depth;
		printf("new depth: %d", global_depth);
	}

	isect i;

	if( scene->intersect( r, i ) ) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		const Material& m = i.getMaterial();
		vec3f I = m.shade(scene, r, i);		//phone model

		if (depth >= 1){
			//reflection
			ray R = getReflectDirection(r, i);
			vec3f I1 = traceRay(scene, R, vec3f(1.0, 1.0, 1.0), depth - 1);
			I1[0] *= m.kr[0];
			I1[1] *= m.kr[1];
			I1[2] *= m.kr[2];
			I += I1;

			//retraction
			double index_of_air = 1.000277;
			double n_i = 0.0;
			double n_t = 0.0;
			if (isEnteringObject(r, i)){
				n_i = index_of_air;
				n_t = m.index;
			}
			else{
				n_i = m.index;
				n_t = index_of_air;
			}
			if (notTIR(r, i, n_i, n_t)){
				ray T = getRetractionDirection(r, i, n_i, n_t);
				vec3f I2 = traceRay(scene, T, vec3f(1.0, 1.0, 1.0), depth - 1);
				I2[0] *= m.kt[0];
				I2[1] *= m.kt[1];
				I2[2] *= m.kt[2];
				I += I2;
			}
		}

		return I;
	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		return vec3f( 0.0, 0.0, 0.0 );
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}

Scene *RayTracer::getScene(){
	return scene;
}

ray RayTracer::getReflectDirection(ray input,isect intersection){
	double t = intersection.t;
	vec3f p = input.getPosition();
	vec3f d = input.getDirection();
	vec3f intersectPoint = input.at(t);

	vec3f a = -d;
	vec3f b = intersection.N;
	vec3f c = a - a.dot(b) * b;
	vec3f out = a.dot(b) * b - c;

	return ray(intersectPoint, out);
}

bool RayTracer::isEnteringObject(ray input, isect intersection){
	if (input.getDirection().dot(intersection.N) < 0){
		return true;
	}
	return false;
}

bool RayTracer::notTIR(ray input, isect intersection, double n_i, double n_t){
	//n_i : index of the material which light comes from
	if (n_i <= n_t){
		return true;
	}
	vec3f _d = -input.getDirection();
	vec3f n = intersection.N;
	double cos_input_angle = _d.dot(n);
	double sin_input_angle = sqrt(1 - cos_input_angle * cos_input_angle);
	if (sin_input_angle > n_t / n_i){
		return false;
	}
	return true;
}

ray RayTracer::getRetractionDirection(const ray input, const isect intersection, double n_i, double n_t){
	vec3f _d = -input.getDirection();
	vec3f n = intersection.N;
	double cos_input_angle = _d.dot(n);
	double sin_input_angle = sqrt(1 - cos_input_angle * cos_input_angle);
	double sin_output_angle = n_i * sin_input_angle / n_t;

	double input_angle = asin(sin_input_angle);
	double output_angle = asin(sin_output_angle);

	vec3f output;
	double M_PI = 3.1415926;

	//the normal vecter is not always on the input ray side
	if (isEnteringObject(input, intersection)){
		_d.rotateArbitraryLine(n.cross(_d), output, -input_angle - M_PI + output_angle);
	}
	else{
		_d.rotateArbitraryLine(_d.cross(n), output, -input_angle - M_PI + output_angle);
	}

	return ray(input.at(intersection.t),output);
}