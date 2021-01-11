#include <iostream>
#include <chrono>
#include "shashki-engine/common.hpp"
#include "shashki-engine/move-generation.hpp"
#include "shashki-engine/evaluation.hpp"
#include "shashki-engine/engine.hpp"

unsigned long long time()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

int main()
{
    unsigned long long before = time();

    shashki::Game game = shashki::Game();
    shashki::Move best_move = shashki::best_move(game, 15);
    std::cout << best_move.description() << std::endl;

    unsigned long long after = time();
    unsigned long long duration = after - before;
    std::cout << "Time (millis): " << duration << std::endl;
    return 0;
}
