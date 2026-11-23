#include "modloader/modloader.hpp"

int main() {
    if (!modloader::initialize()) {
        return 1;
    }
    if (!modloader::initialize()) {
        return 2;
    }
    return 0;
}
