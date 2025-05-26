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

};  

#endif // APP_H