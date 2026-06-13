#include "storage.h"
#include "ui.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main(void) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    load_tasks();
    run_app();
    return 0;
}
