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
	Fl_Slider* rotYSlider;
	Fl_Slider* lightAngleSlider;
	Fl_Slider* lightIntensitySlider;
	Fl_Button* openFileButton;
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

	Fl_Pack* pack = new Fl_Pack(w() - 310, 30, 150, h(), "");
	pack->box(FL_DOWN_FRAME);
	pack->type(Fl_Pack::VERTICAL);
	pack->spacing(30);
	pack->begin();

	Fl_Pack* envpack = new Fl_Pack(w() - 100, 30, 100, h(), "Environment");
	envpack->box(FL_DOWN_FRAME);
	envpack->labelfont(1);
	envpack->type(Fl_Pack::VERTICAL);
	envpack->spacing(0);
	envpack->begin();

	Fl_Box* rotYTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "RotateY");
	rotYSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	rotYSlider->align(FL_ALIGN_TOP);
	rotYSlider->type(FL_HOR_SLIDER);
	rotYSlider->bounds(-359, 359);
	rotYSlider->step(1);
	rotYSlider->value(canvas->rotWorldVec.y);
	rotYSlider->callback(floatCB, (void*)(&(canvas->rotWorldVec.y)));

	pack->end();

	Fl_Pack* packShaders = new Fl_Pack(w() - 100, 30, 100, h(), "Shader");
	packShaders->box(FL_DOWN_FRAME);
	packShaders->labelfont(1);
	packShaders->type(Fl_Pack::VERTICAL);
	packShaders->spacing(0);
	packShaders->begin();

	Fl_Box* lightAngleTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "Light Angle");
	lightAngleSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	lightAngleSlider->align(FL_ALIGN_TOP);
	lightAngleSlider->type(FL_HOR_SLIDER);
	lightAngleSlider->bounds(-180, 180);
	lightAngleSlider->step(1);
	lightAngleSlider->value(canvas->lightAngle);
	lightAngleSlider->callback(floatCB, (void*)(&(canvas->lightAngle)));

	Fl_Box* lightIntensityTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "Light Intensity");
	lightIntensitySlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	lightIntensitySlider->align(FL_ALIGN_TOP);
	lightIntensitySlider->type(FL_HOR_SLIDER);
	lightIntensitySlider->bounds(-180, 180);
	lightIntensitySlider->step(1);
	lightIntensitySlider->value(canvas->lightAngle);
	lightIntensitySlider->callback(floatCB, (void*)(&(canvas->lightIntensity)));

	reloadButton = new Fl_Button(0, 0, pack->w() - 20, 20, "Reload");
	reloadButton->callback(reloadCB, (void*)this);

	packShaders->end();

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