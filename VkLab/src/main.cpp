#include "core/Renderer.h"

int main() {
    Renderer renderer;

    try {
        renderer.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get(); // Waits for user input
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}