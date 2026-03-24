#include "HandcrankEngine/HandcrankEngine.hpp"

using namespace HandcrankEngine;

auto main(int argc, char *argv[]) -> int
{
    auto game = std::make_unique<Game>();

    game->SetTitle("Handcrank Engine");

    return game->Run();
}
