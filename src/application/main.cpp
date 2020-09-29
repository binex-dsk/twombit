
#include "main.h"
#include "app.h"
#include "exception.h"

int main(int argc, char** argv) {
    try {
        App* app = new App(argc, argv);
        app->Create();
        return app->Run();
    } catch (const Exception& exception) {
        return exception.handle();
    }
}
