#ifndef APP_H
#define APP_H

#include "Renderer.h"

class App {
public:
    App();
    ~App();

    void run();

private:
    Renderer renderer_m;

    int screenHeight;
    int screenWidth;
};  

#endif // APP_H