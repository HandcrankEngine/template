#define SDL_MAIN_HANDLED

#include "Handcrank/Handcrank.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace Handcrank;

auto game = std::make_unique<Game>();

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nShowCmd)
#else
auto main() -> int
#endif
{
    game->SetTitle("Handcrank Engine");

    return game->Run();
}
