#ifndef APP_H
#define APP_H

#include "Renderer.h"

class App {
public:
    // Constructor and Destructor
    App();
    ~App();

    // Public Methods
    void run();

private:
    
    Renderer renderer_m;

};  

#endif // APP_H