#include "../include/ui/console_ui.h"
#include <iostream>

int main() {
    try {
        netscanner::ConsoleUI ui;
        ui.run();
    } catch (const std::exception& e) {
        std::cerr << "[FATAL] " << e.what() << std::endl;
        return 1;
    }

    return 0;
};