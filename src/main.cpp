#include <memory>
#include "App.hpp"

int main(int argc, char** argv) {
    std::unique_ptr<App> app = std::make_unique<App>();

    app->run();
}