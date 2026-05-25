#include "ui/app.h"
#include <cstdio>
#include <cstdlib>

int main(int, char**) {
    grip::App app;

    if (!app.init(1400, 900)) {
        std::fprintf(stderr, "Failed to initialize application\n");
        return 1;
    }

    while (app.running()) {
        app.frame();
    }

    return 0;
}
