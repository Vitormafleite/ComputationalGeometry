#include "../headers/App.h"

App::App(){
    this->screenWidth = 1200;
    this->screenHeight = 900;
}

App::~App(){}

//General Structure to run the app
void App::run(){

    if (!renderer_m.init(screenWidth, screenHeight, "Geometric Modeling")) {
        return;
    }
    
    while (!renderer_m.shouldClose()) {
        renderer_m.beginFrame();
        renderer_m.loadUIElements();
        renderer_m.renderGeometry();
        renderer_m.endFrame();
    }

    renderer_m.shutdown();
}

