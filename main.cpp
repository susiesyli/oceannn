/*  =================== File Information =================
	File Name: main.cpp
	Description:
	Author: Michael Shah

	Purpose: Driver for 3D program to load .ply models
	Usage:
	===================================================== */

#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/names.h>

#include "MyGLCanvas.h"

using namespace std;

class MyAppWindow;
MyAppWindow* win;

class MyAppWindow : public Fl_Window {
public:
	// camera sliders
	Fl_Slider* rotYSlider;

	// light sliders
	Fl_Slider* lightAngleSlider;
	Fl_Slider* lightIntensitySlider;

	// wave sliders
	Fl_Slider* waveAmplitudeSlider;
	Fl_Slider* waveFrequencySlider;
	Fl_Slider* waveSpeedXSlider;
	Fl_Slider* waveSpeedYSlider;
	
	// shader button
	Fl_Button* reloadButton;

	MyGLCanvas* canvas;

public:
	// APP WINDOW CONSTRUCTOR
	MyAppWindow(int W, int H, const char* L = 0);

	static void idleCB(void* userdata) {
		win->canvas->redraw();
	}

private:
	// Someone changed one of the sliders
	static void floatCB(Fl_Widget* w, void* userdata) {
		float value = ((Fl_Slider*)w)->value();
		*((float*)userdata) = value;
	}

	static void intCB(Fl_Widget* w, void* userdata) {
		int value = ((Fl_Button*)w)->value();
		printf("value: %d\n", value);
		*((int*)userdata) = value;
	}

	static void loadFileCB(Fl_Widget* w, void* data) {
		Fl_File_Chooser G_chooser("", "", Fl_File_Chooser::MULTI, "");
		G_chooser.show();
		G_chooser.directory("./data");
		while (G_chooser.shown()) {
			Fl::wait();
		}

		// Print the results
		if (G_chooser.value() == NULL) {
			printf("User cancelled file chooser\n");
			return;
		}

		cout << "Loading new PLY file from: " << G_chooser.value() << endl;
		win->canvas->loadPLY(G_chooser.value());
		win->canvas->redraw();
	}

	static void loadEnvFileCB(Fl_Widget* w, void* data) {
		Fl_File_Chooser G_chooser("", "", Fl_File_Chooser::MULTI, "");
		G_chooser.show();
		G_chooser.directory("./data");
		while (G_chooser.shown()) {
			Fl::wait();
		}

		// Print the results
		if (G_chooser.value() == NULL) {
			printf("User cancelled file chooser\n");
			return;
		}

		cout << "Loading new PPM file from: " << G_chooser.value() << endl;
		win->canvas->loadEnvironmentTexture(G_chooser.value());
		win->canvas->redraw();
	}

	static void loadTextureFileCB(Fl_Widget* w, void* data) {
		Fl_File_Chooser G_chooser("", "", Fl_File_Chooser::MULTI, "");
		G_chooser.show();
		G_chooser.directory("./data");
		while (G_chooser.shown()) {
			Fl::wait();
		}

		// Print the results
		if (G_chooser.value() == NULL) {
			printf("User cancelled file chooser\n");
			return;
		}

		cout << "Loading new PPM file from: " << G_chooser.value() << endl;
		win->canvas->loadObjectTexture(G_chooser.value());
		win->canvas->redraw();
	}


	static void reloadCB(Fl_Widget* w, void* userdata) {
		win->canvas->reloadShaders();
	}
};

MyAppWindow::MyAppWindow(int W, int H, const char* L) : Fl_Window(W, H, L) {
    begin();
    // OpenGL window
    canvas = new MyGLCanvas(10, 10, w() - 320, h() - 20);

    // Main pack container (left column)
    Fl_Pack* packLeft = new Fl_Pack(w() - 310, 30, 150, h() - 40, "");
    packLeft->box(FL_DOWN_FRAME);
    packLeft->type(Fl_Pack::VERTICAL);
    packLeft->spacing(20);
    packLeft->begin();

    // Camera Controls Pack
    Fl_Pack* cameraPack = new Fl_Pack(0, 0, packLeft->w(), 120, "Camera Controls");
    cameraPack->box(FL_DOWN_FRAME);
    cameraPack->labelfont(FL_BOLD);
    cameraPack->type(Fl_Pack::VERTICAL);
    cameraPack->spacing(10);
    cameraPack->begin();

    Fl_Box* rotYTextbox = new Fl_Box(0, 0, cameraPack->w(), 20, "Rotate Y");
    rotYSlider = new Fl_Value_Slider(0, 0, cameraPack->w(), 20, "");
    rotYSlider->align(FL_ALIGN_TOP);
    rotYSlider->type(FL_HOR_SLIDER);
    rotYSlider->bounds(-359, 359);
    rotYSlider->step(1);
    rotYSlider->value(canvas->rotWorldVec.y);
    rotYSlider->callback(floatCB, (void*)(&(canvas->rotWorldVec.y)));

    cameraPack->end();

    // Light Controls Pack
    Fl_Pack* lightPack = new Fl_Pack(0, 0, packLeft->w(), 180, "Light Controls");
    lightPack->box(FL_DOWN_FRAME);
    lightPack->labelfont(FL_BOLD);
    lightPack->type(Fl_Pack::VERTICAL);
    lightPack->spacing(10);
    lightPack->begin();

    Fl_Box* lightAngleTextbox = new Fl_Box(0, 0, lightPack->w(), 20, "Light Angle");
    lightAngleSlider = new Fl_Value_Slider(0, 0, lightPack->w(), 20, "");
    lightAngleSlider->align(FL_ALIGN_TOP);
    lightAngleSlider->type(FL_HOR_SLIDER);
    lightAngleSlider->bounds(-120, 120);
    lightAngleSlider->step(1);
    lightAngleSlider->value(canvas->lightAngle);
    lightAngleSlider->callback(floatCB, (void*)(&(canvas->lightAngle)));

    Fl_Box* lightIntensityTextbox = new Fl_Box(0, 0, lightPack->w(), 20, "Light Intensity");
    lightIntensitySlider = new Fl_Value_Slider(0, 0, lightPack->w(), 20, "");
    lightIntensitySlider->align(FL_ALIGN_TOP);
    lightIntensitySlider->type(FL_HOR_SLIDER);
    lightIntensitySlider->bounds(0, 2.0f);
    lightIntensitySlider->step(0.01);
    lightIntensitySlider->value(canvas->lightIntensity);
    lightIntensitySlider->callback(floatCB, (void*)(&(canvas->lightIntensity)));

    lightPack->end();

    packLeft->end();

    // Right column pack for Wave and Shader Controls
    Fl_Pack* packRight = new Fl_Pack(w() - 150, 30, 150, h() - 40, "");
    packRight->box(FL_DOWN_FRAME);
    packRight->type(Fl_Pack::VERTICAL);
    packRight->spacing(20);
    packRight->begin();

    // Wave Controls Pack
    Fl_Pack* wavePack = new Fl_Pack(0, 0, packRight->w(), 100, "Wave Controls");
    wavePack->box(FL_DOWN_FRAME);
    wavePack->labelfont(FL_BOLD);
    wavePack->type(Fl_Pack::VERTICAL);
    wavePack->spacing(10);
    wavePack->begin();

    Fl_Box* waveAmplitudeTextbox = new Fl_Box(0, 0, wavePack->w(), 20, "Wave Amplitude");
    waveAmplitudeSlider = new Fl_Value_Slider(0, 0, wavePack->w(), 20, "");
    waveAmplitudeSlider->align(FL_ALIGN_TOP);
    waveAmplitudeSlider->type(FL_HOR_SLIDER);
    waveAmplitudeSlider->bounds(0, 0.1f);
    waveAmplitudeSlider->step(0.001);
    waveAmplitudeSlider->value(canvas->waveAmplitude);
    waveAmplitudeSlider->callback(floatCB, (void*)(&(canvas->waveAmplitude)));

    Fl_Box* waveFrequencyTextbox = new Fl_Box(0, 0, wavePack->w(), 20, "Wave Frequency");
    waveFrequencySlider = new Fl_Value_Slider(0, 0, wavePack->w(), 20, "");
    waveFrequencySlider->align(FL_ALIGN_TOP);
    waveFrequencySlider->type(FL_HOR_SLIDER);
    waveFrequencySlider->bounds(0.5, 5.0f);
    waveFrequencySlider->step(0.01);
    waveFrequencySlider->value(canvas->waveFrequency);
    waveFrequencySlider->callback(floatCB, (void*)(&(canvas->waveFrequency)));

    wavePack->end();

    // Shader Controls Pack
    Fl_Pack* shaderPack = new Fl_Pack(0, 0, packRight->w(), 100, "Shader Controls");
    shaderPack->box(FL_DOWN_FRAME);
    shaderPack->labelfont(FL_BOLD);
    shaderPack->type(Fl_Pack::VERTICAL);
    shaderPack->spacing(10);
    shaderPack->begin();

    std::cout<< "reload shader called" << endl;
    reloadButton = new Fl_Button(0, 0, shaderPack->w(), 30, "Reload Shaders");
    std::cout<< "seg fault 1" << endl;
    reloadButton->callback(reloadCB, (void*)this);
    std::cout<< "seg fault 2" << endl;

    shaderPack->end();
    std::cout<< "seg fault 3" << endl;

    packRight->end();
    std::cout<< "seg fault 4" << endl;

    end();
}


/**************************************** main() ********************/
int main(int argc, char** argv) {
	win = new MyAppWindow(850, 475, "Environment Mapping");
	win->resizable(win);
	Fl::add_idle(MyAppWindow::idleCB);
	win->show();
	return(Fl::run());
}
