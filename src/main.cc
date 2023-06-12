#include "models/controller.hh"

int main(int argc, char *argv[]) {
  const int attack_interval_ms = 10000;
  Controller controller(attack_interval_ms);

  while (true) {
    controller.ShowTargets();
    std::vector<size_t> indices = controller.GetTargets();

    for (size_t index : indices) {
      controller.Action(index);
    }
  }

  return 0;
}