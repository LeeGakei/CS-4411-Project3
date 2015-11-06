#include "ray.h"
#include "material.h"
#include "light.h"
#include "../ui/TraceUI.h"

extern TraceUI* traceUI;

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.
	Material m = i.getMaterial();
	vec3f I = m.ke;
	vec3f intersectionPoint = r.at(i.t);

	for (list<Light*>::const_iterator iterater = scene->beginLights(); iterater != scene->endLights(); iterater++){
		Light* l = *iterater;
		if (l->kind == "DirectionalLight" || l->kind == "PointLight"){
			//diffused reflection
			vec3f dir = l->getDirection(intersectionPoint);
			double cosTheta = dir.dot(i.N);
			//cosTheta = abs(cosTheta);
			cosTheta = max(0.0, cosTheta);
			vec3f I1 = m.kd * cosTheta;

			I1[0] *= (1.0 - m.kt[0]);
			I1[1] *= (1.0 - m.kt[1]);
			I1[2] *= (1.0 - m.kt[2]);


			//specular reflection
			vec3f R = r.getReflectDirection(i).getDirection();
			vec3f eye = scene->getCamera()->getEye();
			vec3f V = (eye - intersectionPoint).normalize();
			double cosCigama = R.dot(V);
			cosCigama = max(0.0, cosCigama);
			//double nShiniess = 1 / (1.0000005 - m.shininess);
			double nShiniess = m.shininess * 128;
			vec3f I2 = m.ks * pow(cosCigama, nShiniess);
			//distance Attenuation and plus intensity of the light
			vec3f total_I = (I1 + I2)*l->distanceAttenuation(intersectionPoint);
			//shadowAttenuation
			{
				vec3f shadow = l->shadowAttenuation(intersectionPoint);
				total_I[0] *= shadow[0];
				total_I[1] *= shadow[1];
				total_I[2] *= shadow[2];
			}
			vec3f intensityOfLight = l->getColor(intersectionPoint);
			total_I[0] *= intensityOfLight[0];
			total_I[1] *= intensityOfLight[1];
			total_I[2] *= intensityOfLight[2];

			I += total_I;
		}
		else if (l->kind == "AmbientLight"){
			vec3f I3 = l->getColor(intersectionPoint);
			I3[0] *= m.ka[0];
			I3[1] *= m.ka[1];
			I3[2] *= m.ka[2];

			I3[0] *= (1.0 - m.kt[0]);
			I3[1] *= (1.0 - m.kt[1]);
			I3[2] *= (1.0 - m.kt[2]);

			if (traceUI->usingUISetting){
				I3 *= traceUI->m_nAmbientLight;
			}
			I += I3;
		}
		else{
			printf("unrecognized light format!");
		}
	}

	I = I.clamp();

	return I;
}
