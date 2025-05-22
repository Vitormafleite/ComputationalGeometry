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

        // ImGui UI rendering
        //ImGui::Begin("Demo");
        //ImGui::Text("Welcome to the Geometric Modeling App!");
        //ImGui::End();

        // (TODO: invoke your geometry logic and visualizers here)

        renderer_m.endFrame();
    }

    renderer_m.shutdown();
}

