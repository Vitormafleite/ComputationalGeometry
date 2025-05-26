#include "../headers/App.h"

App::App(){}

App::~App(){}

//General Structure to run the app
void App::run(){

    if (!renderer_m.init(800, 600, "Geometric Modeling")) {
        return;
    }
    
    while (!renderer_m.shouldClose()) {
        renderer_m.beginFrame();
        renderer_m.loadUIElements();
        //renderer_m.drawTestTriangle();
        renderer_m.renderMesh();
        renderer_m.endFrame();
    }

    renderer_m.shutdown();
}

