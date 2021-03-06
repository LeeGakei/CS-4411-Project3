//
// TraceUI.h
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <FL/fl_ask.h>

#include "TraceUI.h"
#include "../RayTracer.h"

static bool done;

//------------------------------------- Help Functions --------------------------------------------
TraceUI* TraceUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ( (TraceUI*)(o->parent()->user_data()) );
}

//--------------------------------- Callback Functions --------------------------------------------
void TraceUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			done=true;	// terminate the previous rendering
		} else{
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
	}
}

void TraceUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void TraceUI::cb_exit(Fl_Menu_* o, void* v)
{
	TraceUI* pUI=whoami(o);

	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_exit2(Fl_Widget* o, void* v) 
{
	TraceUI* pUI=(TraceUI *)(o->user_data());
	
	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project, FLTK version for CS 341 Spring 2002. Latest modifications by Jeff Maurer, jmaurer@cs.washington.edu");
}

void TraceUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI=(TraceUI*)(o->user_data());
	
	pUI->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow( pUI->m_nSize, height );
}

void TraceUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_AttConstantSlides(Fl_Widget* o, void* v){
	((TraceUI*)(o->user_data()))->m_nAttConstant = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_AttLinearSlides(Fl_Widget* o, void* v){
	((TraceUI*)(o->user_data()))->m_nAttLinear = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_AttQuadraticSlides(Fl_Widget* o, void* v){
	((TraceUI*)(o->user_data()))->m_nAttQuadratic = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_AmbientLightSlides(Fl_Widget* o, void* v){
	((TraceUI*)(o->user_data()))->m_nAmbientLight = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_IntensityScaleSlides(Fl_Widget* o, void* v){
	((TraceUI*)(o->user_data()))->m_nIntensityScale = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_IntensityThresholdSlides(Fl_Widget* o, void* v){
	((TraceUI*)(o->user_data()))->m_nIntensityThreshold = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_usingUISettingButton(Fl_Widget* o, void* v)
{
	TraceUI *pUI = ((TraceUI*)(o->user_data()));

	if (pUI->usingUISetting == true) pUI->usingUISetting = false;
	else pUI->usingUISetting = true;
}

void TraceUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	TraceUI* pUI=((TraceUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow( width, height );

		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);
		
		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		done=false;
		clock_t prev, now;
		prev=clock();
		
		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();

		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
				if (done) break;
				
				// current time
				now = clock();

				// check event every 1/2 second
				if (((double)(now-prev)/CLOCKS_PER_SEC)>0.5) {
					prev=now;

					if (Fl::ready()) {
						// refresh
						pUI->m_traceGlWindow->refresh();
						// check event
						Fl::check();

						if (Fl::damage()) {
							Fl::flush();
						}
					}
				}

				pUI->raytracer->tracePixel( x, y );
		
			}
			if (done) break;

			// flush when finish a row
			if (Fl::ready()) {
				// refresh
				pUI->m_traceGlWindow->refresh();

				if (Fl::damage()) {
					Fl::flush();
				}
			}
			// update the window label
			sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
			pUI->m_traceGlWindow->label(buffer);
			
		}
		done=true;
		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);		
	}
}

void TraceUI::cb_stop(Fl_Widget* o, void* v)
{
	done=true;
}

void TraceUI::show()
{
	m_mainWindow->show();
}

void TraceUI::setRayTracer(RayTracer *tracer)
{
	raytracer = tracer;
	m_traceGlWindow->setRayTracer(tracer);
}

int TraceUI::getSize()
{
	return m_nSize;
}

int TraceUI::getDepth()
{
	return m_nDepth;
}

// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)TraceUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)TraceUI::cb_about },
		{ 0 },

	{ 0 }
};

TraceUI::TraceUI() {
	// init.
	m_nDepth = 0;
	m_nSize = 150;
	m_nAttConstant = 0.25;
	m_nAttLinear = 0.25;
	m_nAttQuadratic = 0.50;
	m_nAmbientLight = 0.20;
	m_nIntensityScale = 1.0;
	m_nIntensityThreshold = 0.1;
	usingUISetting = false;

	m_mainWindow = new Fl_Window(100, 40, 320, 300, "Ray <Not Loaded>");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 320, 25);
		m_menubar->menu(menuitems);

		// install slider depth
		m_depthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
		m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_depthSlider->type(FL_HOR_NICE_SLIDER);
        m_depthSlider->labelfont(FL_COURIER);
        m_depthSlider->labelsize(12);
		m_depthSlider->minimum(0);
		m_depthSlider->maximum(10);
		m_depthSlider->step(1);
		m_depthSlider->value(m_nDepth);
		m_depthSlider->align(FL_ALIGN_RIGHT);
		m_depthSlider->callback(cb_depthSlides);

		// install slider size
		m_sizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
		m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sizeSlider->type(FL_HOR_NICE_SLIDER);
        m_sizeSlider->labelfont(FL_COURIER);
        m_sizeSlider->labelsize(12);
		m_sizeSlider->minimum(64);
		m_sizeSlider->maximum(512);
		m_sizeSlider->step(1);
		m_sizeSlider->value(m_nSize);
		m_sizeSlider->align(FL_ALIGN_RIGHT);
		m_sizeSlider->callback(cb_sizeSlides);

		m_renderButton = new Fl_Button(240, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		m_stopButton = new Fl_Button(240, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

		// install attenuation constant
		m_attConstantSlider = new Fl_Value_Slider(10, 80, 150, 20, "Attenuation,Constant");
		m_attConstantSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_attConstantSlider->type(FL_HOR_NICE_SLIDER);
		m_attConstantSlider->labelfont(FL_COURIER);
		m_attConstantSlider->labelsize(12);
		m_attConstantSlider->minimum(0);
		m_attConstantSlider->maximum(1);
		m_attConstantSlider->step(0.01);
		m_attConstantSlider->value(m_nAttConstant);
		m_attConstantSlider->align(FL_ALIGN_RIGHT);
		m_attConstantSlider->callback(cb_AttConstantSlides);

		// install attenuation linear
		m_attLinearSlider = new Fl_Value_Slider(10, 105, 150, 20, "Attenuation,Linear");
		m_attLinearSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_attLinearSlider->type(FL_HOR_NICE_SLIDER);
		m_attLinearSlider->labelfont(FL_COURIER);
		m_attLinearSlider->labelsize(12);
		m_attLinearSlider->minimum(0);
		m_attLinearSlider->maximum(1);
		m_attLinearSlider->step(0.01);
		m_attLinearSlider->value(m_nAttLinear);
		m_attLinearSlider->align(FL_ALIGN_RIGHT);
		m_attLinearSlider->callback(cb_AttLinearSlides);

		// install attenuation quadratic
		m_attQuadraticSlider = new Fl_Value_Slider(10, 130, 150, 20, "Attenuation,Quadratic");
		m_attQuadraticSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_attQuadraticSlider->type(FL_HOR_NICE_SLIDER);
		m_attQuadraticSlider->labelfont(FL_COURIER);
		m_attQuadraticSlider->labelsize(12);
		m_attQuadraticSlider->minimum(0);
		m_attQuadraticSlider->maximum(1);
		m_attQuadraticSlider->step(0.01);
		m_attQuadraticSlider->value(m_nAttQuadratic);
		m_attQuadraticSlider->align(FL_ALIGN_RIGHT);
		m_attQuadraticSlider->callback(cb_AttQuadraticSlides);

		// install Ambient Light
		m_ambientLightSlider = new Fl_Value_Slider(10, 155, 150, 20, "Ambient Light");
		m_ambientLightSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ambientLightSlider->type(FL_HOR_NICE_SLIDER);
		m_ambientLightSlider->labelfont(FL_COURIER);
		m_ambientLightSlider->labelsize(12);
		m_ambientLightSlider->minimum(0);
		m_ambientLightSlider->maximum(1);
		m_ambientLightSlider->step(0.01);
		m_ambientLightSlider->value(m_nAmbientLight);
		m_ambientLightSlider->align(FL_ALIGN_RIGHT);
		m_ambientLightSlider->callback(cb_AmbientLightSlides);

		// install intensityScaleSlider
		m_intensityScaleSlider = new Fl_Value_Slider(10, 180, 150, 20, "Intensity Scale");
		m_intensityScaleSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_intensityScaleSlider->type(FL_HOR_NICE_SLIDER);
		m_intensityScaleSlider->labelfont(FL_COURIER);
		m_intensityScaleSlider->labelsize(12);
		m_intensityScaleSlider->minimum(0.5);
		m_intensityScaleSlider->maximum(3);
		m_intensityScaleSlider->step(0.01);
		m_intensityScaleSlider->value(m_nIntensityScale);
		m_intensityScaleSlider->align(FL_ALIGN_RIGHT);
		m_intensityScaleSlider->callback(cb_IntensityScaleSlides);

		// install m_intensityThresholdSlider
		m_intensityThresholdSlider = new Fl_Value_Slider(10, 205, 150, 20, "Intensity Threshold");
		m_intensityThresholdSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_intensityThresholdSlider->type(FL_HOR_NICE_SLIDER);
		m_intensityThresholdSlider->labelfont(FL_COURIER);
		m_intensityThresholdSlider->labelsize(12);
		m_intensityThresholdSlider->minimum(0);
		m_intensityThresholdSlider->maximum(1);
		m_intensityThresholdSlider->step(0.01);
		m_intensityThresholdSlider->value(m_nIntensityThreshold);
		m_intensityThresholdSlider->align(FL_ALIGN_RIGHT);
		m_intensityThresholdSlider->callback(cb_IntensityThresholdSlides);

		usingUISettingButton = new Fl_Light_Button(10, 230, 300, 20, "using UI Attenuation and Ambient Settings");
		usingUISettingButton->user_data((void*)(this));   // record self to be used by static callback functions
		usingUISettingButton->callback(cb_usingUISettingButton);
		usingUISettingButton->clear_changed();

		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
    m_mainWindow->end();
	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);
}