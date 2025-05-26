#include "headers/App.h"

int main() {
    App GeoModelingApp;  
    GeoModelingApp.run();

    GeoModelingApp.~App();

    return 0;
}