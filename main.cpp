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

    // fog sliders
    Fl_Slider* fogDensitySlider;
    Fl_Slider* noiseScaleSlider;
    Fl_Slider* noiseSpeedSlider;
    Fl_Button* useFogButton;

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
    packLeft->color(FL_WHITE);
    packLeft->begin();

    // Camera Controls Pack
    Fl_Pack* cameraPack = new Fl_Pack(0, 0, packLeft->w(), 120, "Camera Controls");
    cameraPack->box(FL_DOWN_FRAME);
    cameraPack->labelfont(FL_BOLD);
    cameraPack->type(Fl_Pack::VERTICAL);
    cameraPack->spacing(10);
    cameraPack->color(FL_WHITE);
    cameraPack->begin();

    Fl_Box* rotYTextbox = new Fl_Box(0, 0, cameraPack->w(), 20, "Rotate Y");
    rotYTextbox->color(FL_WHITE);
    rotYSlider = new Fl_Value_Slider(0, 0, cameraPack->w(), 20, "");
    rotYSlider->color(FL_WHITE);
    rotYSlider->align(FL_ALIGN_TOP);
    rotYSlider->type(FL_HOR_SLIDER);
    rotYSlider->bounds(-359, 359);
    rotYSlider->step(1);
    rotYSlider->value(canvas->rotWorldVec.y);
    rotYSlider->callback(floatCB, (void*)(&(canvas->rotWorldVec.y)));
    // new changes!!!
    cameraPack->end();

    // Light Controls Pack
    Fl_Pack* lightPack = new Fl_Pack(0, 0, packLeft->w(), 180, "Light Controls");
    lightPack->box(FL_DOWN_FRAME);
    lightPack->labelfont(FL_BOLD);
    lightPack->type(Fl_Pack::VERTICAL);
    lightPack->spacing(10);
    lightPack->color(FL_WHITE);
    lightPack->begin();

    Fl_Box* lightAngleTextbox = new Fl_Box(0, 0, lightPack->w(), 20, "Light Angle");
    lightAngleTextbox->color(FL_WHITE);
    lightAngleSlider = new Fl_Value_Slider(0, 0, lightPack->w(), 20, "");
    lightAngleSlider->color(FL_WHITE);
    lightAngleSlider->align(FL_ALIGN_TOP);
    lightAngleSlider->type(FL_HOR_SLIDER);
    lightAngleSlider->bounds(-360, 360);
    lightAngleSlider->step(1);
    lightAngleSlider->value(canvas->lightAngle);
    lightAngleSlider->callback(floatCB, (void*)(&(canvas->lightAngle)));

    Fl_Box* lightIntensityTextbox = new Fl_Box(0, 0, lightPack->w(), 20, "Light Intensity");
    lightIntensityTextbox->color(FL_WHITE);
    lightIntensitySlider = new Fl_Value_Slider(0, 0, lightPack->w(), 20, "");
    lightIntensitySlider->color(FL_WHITE);
    lightIntensitySlider->align(FL_ALIGN_TOP);
    lightIntensitySlider->type(FL_HOR_SLIDER);
    lightIntensitySlider->bounds(0, 2.5f);
    lightIntensitySlider->step(0.01);
    lightIntensitySlider->value(canvas->lightIntensity);
    lightIntensitySlider->callback(floatCB, (void*)(&(canvas->lightIntensity)));

    lightPack->end();

    Fl_Pack* fogPack = new Fl_Pack(0, 0, packLeft->w(), 180, "Fog Controls");
    fogPack->box(FL_DOWN_FRAME);
    fogPack->labelfont(FL_BOLD);
    fogPack->type(Fl_Pack::VERTICAL);
    fogPack->spacing(10);
    fogPack->color(FL_WHITE);
    fogPack->begin();

    useFogButton = new Fl_Check_Button(0, 100, fogPack->w() - 20, 20, "Use Fog");
    useFogButton->color(FL_WHITE);
    useFogButton->callback(intCB, (void*)(&(canvas->useFog)));
    useFogButton->value(canvas->useFog);

    Fl_Box* fogDensityTextbox = new Fl_Box(0, 0, fogPack->w(), 20, "Fog Density");
    fogDensitySlider = new Fl_Value_Slider(0, 0, fogPack->w(), 20, "");
    fogDensitySlider->color(FL_WHITE);
    fogDensitySlider->align(FL_ALIGN_TOP);
    fogDensitySlider->type(FL_HOR_SLIDER);
    fogDensitySlider->bounds(0.0f, 0.2f);
    fogDensitySlider->step(0.01f);
    fogDensitySlider->value(canvas->fogDensity);
    fogDensitySlider->callback(floatCB, (void*)(&(canvas->fogDensity)));

    Fl_Box* noiseScaleTextbox = new Fl_Box(0, 0, fogPack->w(), 20, "Wind");
    noiseScaleSlider = new Fl_Value_Slider(0, 0, fogPack->w(), 20, "");
    noiseScaleSlider->color(FL_WHITE);
    noiseScaleSlider->align(FL_ALIGN_TOP);
    noiseScaleSlider->type(FL_HOR_SLIDER);
    noiseScaleSlider->bounds(0.0f, 1.0f);
    noiseScaleSlider->step(0.01f);
    noiseScaleSlider->value(canvas->noiseScale);
    noiseScaleSlider->callback(floatCB, (void*)(&(canvas->noiseScale)));


    fogPack->end();

    packLeft->end();

    // Right column pack for Wave and Shader Controls
    Fl_Pack* packRight = new Fl_Pack(w() - 150, 30, 150, h() - 40, "");
    packRight->box(FL_DOWN_FRAME);
    packRight->type(Fl_Pack::VERTICAL);
    packRight->spacing(20);
    packRight->color(FL_WHITE);
    packRight->begin();

    // Wave Controls Pack
    Fl_Pack* wavePack = new Fl_Pack(0, 0, packRight->w(), 100, "Wave Controls");
    wavePack->box(FL_DOWN_FRAME);
    wavePack->labelfont(FL_BOLD);
    wavePack->type(Fl_Pack::VERTICAL);
    wavePack->spacing(10);
    wavePack->color(FL_WHITE);
    wavePack->begin();

    Fl_Box* waveAmplitudeTextbox = new Fl_Box(0, 0, wavePack->w(), 20, "Wave Amplitude");
    waveAmplitudeSlider = new Fl_Value_Slider(0, 0, wavePack->w(), 20, "");
    waveAmplitudeSlider->color(FL_WHITE);
    waveAmplitudeSlider->align(FL_ALIGN_TOP);
    waveAmplitudeSlider->type(FL_HOR_SLIDER);
    waveAmplitudeSlider->bounds(0, 0.1f);
    waveAmplitudeSlider->step(0.001);
    waveAmplitudeSlider->value(canvas->waveAmplitude);
    waveAmplitudeSlider->callback(floatCB, (void*)(&(canvas->waveAmplitude)));

    Fl_Box* waveFrequencyTextbox = new Fl_Box(0, 0, wavePack->w(), 20, "Wave Frequency");
    waveFrequencySlider = new Fl_Value_Slider(0, 0, wavePack->w(), 20, "");
    waveFrequencySlider->color(FL_WHITE);
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
    shaderPack->color(FL_WHITE);
    shaderPack->begin();

    reloadButton = new Fl_Button(0, 0, shaderPack->w(), 30, "Reload Shaders");
    reloadButton->color(FL_WHITE);
    reloadButton->callback(reloadCB, (void*)this);

    shaderPack->end();

    packRight->end();

    end();
}


/**************************************** main() ********************/
int main(int argc, char** argv) {
    win = new MyAppWindow(850 * 1.5, 475 * 1.5, "Ocean Simulation");
    win->resizable(win);
    Fl::add_idle(MyAppWindow::idleCB);
    win->show();
    return(Fl::run());
}