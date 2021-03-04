#include <iostream>
#include <vector>
#include <chrono>
#include "shashki-engine/common.hpp"
#include "shashki-engine/move-generation.hpp"
#include "shashki-engine/engine.hpp"

const int MAX_BOARDS_MOVE_GENERATION = 10000000;

void benchmark_move_generation()
{
    std::cout << "Preparing for move-generation benchmark...\n";
    
    std::vector<shashki::BitBoard> test_bit_boards = std::vector<shashki::BitBoard>();
    shashki::Game game = shashki::Game();

    while (test_bit_boards.size() < MAX_BOARDS_MOVE_GENERATION) {
        if (shashki::generate_moves_for_game(game).size() == 0) {
            game = shashki::Game();
        }

        shashki::Move random_move = shashki::random_move(game);
        game.execute_move(random_move);
        test_bit_boards.push_back(game.get_bit_board());
    }

    std::cout << "Preparation for move-generation benchmark finished.\n";
    std::cout << "Starting move-generation benchmark...\n";

    std::chrono::duration before_benchmark = std::chrono::high_resolution_clock::now().time_since_epoch();

    for (const shashki::BitBoard& bit_board : test_bit_boards) {
        shashki::generate_moves_for_side(bit_board, shashki::Side::WHITE);
    }

    std::chrono::duration after_benchmark = std::chrono::high_resolution_clock::now().time_since_epoch();
    std::chrono::duration benchmark_duration = after_benchmark - before_benchmark;
    unsigned long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(benchmark_duration).count();

    std::cout << "Move-generation benchmark finished.\n";
    std::cout << "Moves generated for " << MAX_BOARDS_MOVE_GENERATION << " boards.\n";
    std::cout << "Move-generation benchmark took " << millis << " milliseconds.\n";
    std::cout << "Moves calculated for " << (int) (MAX_BOARDS_MOVE_GENERATION / (millis / 1000.0)) << " board constellations per second.\n\n";
}

void benchmark_engine_level(int level, int repititions)
{
    std::cout << "Benchmark engine level " << level << "...\n";

    std::chrono::duration before_benchmark = std::chrono::high_resolution_clock::now().time_since_epoch();
    shashki::Game game = shashki::Game();

    for (int count = 0; count < repititions; count++) {
        if (shashki::generate_moves_for_game(game).size() == 0) {
            game = shashki::Game();
        }

        game.execute_move(shashki::best_move(game, level));
    }

    std::chrono::duration after_benchmark = std::chrono::high_resolution_clock::now().time_since_epoch();
    std::chrono::duration benchmark_duration = after_benchmark - before_benchmark;
    unsigned long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(benchmark_duration).count();

    std::cout << "Calculation for level " << level << " takes on average " << millis / repititions << " milliseconds.\n";
}

void benchmark_engine()
{
    std::cout << "Starting engine benchmark.\n";
    benchmark_engine_level(10, 1000);
    benchmark_engine_level(15, 10);
    benchmark_engine_level(18, 3);
    std::cout << "Engine benchmark finished.\n\n";
}

int main()
{
    std::cout << "- Shashki-Engine benchmark -\n\n";
    benchmark_move_generation();
    benchmark_engine();
    std::cout << "Benchmark finished!\n";
    return 0;
}
