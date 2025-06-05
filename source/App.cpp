#include "../headers/App.h"

App::App(){}

App::~App(){}

//General Structure to run the app
void App::run(){

    if (!renderer_m.init(1200, 900, "Geometric Modeling")) {
        return;
    }
    
    while (!renderer_m.shouldClose()) {
        renderer_m.beginFrame();
        renderer_m.loadUIElements();
        renderer_m.renderMesh();
        renderer_m.endFrame();
    }

    renderer_m.shutdown();
}

