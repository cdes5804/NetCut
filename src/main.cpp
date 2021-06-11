#include "models/controller.hpp"

int main(int argc, char *argv[]) {
    Controller controller;

    while (true) {
        controller.show_targets();
        std::vector<size_t> indices = controller.get_targets();

        for (size_t index : indices) {
            controller.action(index);
        }
    }

    return 0;
}