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

    for (const shashki::BitBoard bit_board : test_bit_boards) {
        shashki::generate_moves_for_side(bit_board, shashki::Side::WHITE);
    }

    std::chrono::duration after_benchmark = std::chrono::high_resolution_clock::now().time_since_epoch();
    std::chrono::duration benchmark_duration = after_benchmark - before_benchmark;
    unsigned long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(benchmark_duration).count();

    std::cout << "Move-generation benchmark finished.\n";
    std::cout << "Moves generated for " << MAX_BOARDS_MOVE_GENERATION << " boards.\n";
    std::cout << "Move-generation benchmark took " << millis << " milliseconds.\n\n";
}

int main()
{
    std::cout << "- Shashki-Engine benchmark -\n\n";
    benchmark_move_generation();
    std::cout << "Benchmark finished!\n";
    return 0;
}
