#include <iostream>
#include <vector>
#include "shashki-engine/common.hpp"
#include "shashki-engine/move-generation.hpp"

int main() {
    shashki::Game game = shashki::Game();
    std::vector<shashki::Move> moves = shashki::generate_moves_for_game(game);

    for (const shashki::Move& move : moves) {
        std::cout << move.description() << std::endl;
    }
    
    return 0;
}
