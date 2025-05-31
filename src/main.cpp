#include "base_window.h"
#include <iostream>

int main(int argc, char* argv[]) {
    BaseWindow window(800, 600, "STEP Viewer");
    
    if (!window.initialize()) {
        std::cerr << "Failed to initialize window" << std::endl;
        return -1;
    }
    
    window.run();
    return 0;
}