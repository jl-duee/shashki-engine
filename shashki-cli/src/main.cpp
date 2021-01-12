#include <iostream>
#include <limits>
#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include "shashki-engine/common.hpp"
#include "shashki-engine/move-generation.hpp"
#include "shashki-engine/engine.hpp"

shashki::Game   game;
shashki::Side   player_side;
int             engine_level;

enum class Command
{
    UNKNOWN,
    HELP,
    STATUS,
    SETUP,
    HINT,
    BESTHINT,
    MOVE,
    QUIT
};

const std::unordered_map<std::string, Command> COMMANDS = {
    {"UNKNOWN", Command::UNKNOWN},
    {"HELP", Command::HELP},
    {"STATUS", Command::STATUS},
    {"SETUP", Command::SETUP},
    {"HINT", Command::HINT},
    {"BESTHINT", Command::BESTHINT},
    {"MOVE", Command::MOVE},
    {"QUIT", Command::QUIT}
};

Command input_to_command(std::string input)
{
    try {
        return COMMANDS.at(input);
    } catch (std::out_of_range error) {
        return Command::UNKNOWN;
    }
}

const std::string BOARD_HORIZONTAL_LINE = "    :-------:-------:-------:-------:-------:-------:-------:-------:\n";
const std::string BOARD_HORIZONTAL_PLACE = "    :       :       :       :       :       :       :       :       :\n";
const std::string BOARD_HORIZONTAL_DESCRIPTION = "        A       B       C       D       E       F       G       H    \n\n";

void print_piece(int row_index, int column_index)
{
    int position = ((row_index - 1) * 8) + (8 - column_index);

    if ((1ULL << position) & game.get_bit_board().pieces_of_side_and_type(shashki::Side::WHITE, shashki::PieceType::MAN)) {
        std::cout << "o";
    } else if ((1ULL << position) & game.get_bit_board().pieces_of_side_and_type(shashki::Side::WHITE, shashki::PieceType::KING)) {
        std::cout << "Ø";
    } else if ((1ULL << position) & game.get_bit_board().pieces_of_side_and_type(shashki::Side::BLACK, shashki::PieceType::MAN)) {
        std::cout << "+";
    } else if ((1ULL << position) & game.get_bit_board().pieces_of_side_and_type(shashki::Side::BLACK, shashki::PieceType::KING)) {
        std::cout << "#";
    } else {
        std::cout << " ";
    }
}

void print_board_row(int row_index)
{
    std::cout << BOARD_HORIZONTAL_LINE;
    std::cout << BOARD_HORIZONTAL_PLACE;

    std::cout << "  " << row_index << " ";
    for (int column_index = 1; column_index <= 8; column_index++) {
        std::cout << ":   ";
        print_piece(row_index, column_index);
        std::cout << "   ";
    }
    std::cout << ":\n";

    std::cout << BOARD_HORIZONTAL_PLACE;
}

void print_board()
{
    for (int row_index = 8; row_index >= 1; row_index--) {
        print_board_row(row_index);
    }

    std::cout << BOARD_HORIZONTAL_LINE;
    std::cout << BOARD_HORIZONTAL_DESCRIPTION;
}

void print_status()
{
    std::cout << "\nYou are playing as: " << (player_side == shashki::Side::WHITE ? "White" : "Black") << "\n";
    std::cout << "You are playing against engine level: " << engine_level << "\n";
    std::cout << "\nBoard:\n\n";
    print_board();
}

void execute_engine_move_path(const shashki::Move& move)
{
    game.execute_move(move);

    if (!move.get_follow_moves().empty()) {
        execute_engine_move_path(move.get_follow_moves().front());
    }
}

void make_engine_move()
{
    std::cout << "The engine is thinking about the next move...\n";

    shashki::Move best_move = shashki::best_move(game, engine_level);
    execute_engine_move_path(best_move);

    print_status();
}

void setup_player_side()
{
    bool valid_input = false;

    while (!valid_input) {
        std::cout << "Please choose the color you want to play (White/Black): ";
        
        std::string color = std::string();
        std::cin >> color;
        std::transform(color.begin(), color.end(), color.begin(), ::toupper);

        if (color == "WHITE") {
            player_side = shashki::Side::WHITE;
            valid_input = true;
        } else if (color == "BLACK") {
            player_side = shashki::Side::BLACK;
            valid_input = true;
        } else {
            std::cout << "Invalid input!\n";
        }
    }
}

void setup_engine_level()
{
    bool valid_input = false;

    while (!valid_input) {
        std::cout << "Please choose the engine level (1-15): ";
        
        std::cin >> engine_level;
            
        if (std::cin.good() && engine_level >= 1 && engine_level <= 15) {
            valid_input = true;
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input!\n";
        }
    }
}

void setup_game()
{
    std::cout << "\nWelcome to a new game of Shashki!\n\n";
    
    setup_player_side();
    setup_engine_level();

    game = shashki::Game();

    std::cout << "\nSetup finished, the game can start!\n";
    print_status();

    if (game.get_current_turn() != player_side) {
        make_engine_move();
    }
}

void print_help()
{
    std::cout << "\nHELP - prints the commands possible to use.\n";
    std::cout << "STATUS - prints the current situation in the game with the board.\n";
    std::cout << "SETUP - initializes a new game.\n";
    std::cout << "HINT - shows the moves that are possible for you.\n";
    std::cout << "BESTHINT - shows the best move you can do.\n";
    std::cout << "MOVE:XX-XX - executes the move XX-XX (XX-XX-XX for a jump).\n";
    std::cout << "QUIT - exits the shashki-cli application.\n\n";
}

void show_hint()
{
    std::vector<shashki::Move> possible_moves = shashki::generate_moves_for_game(game);

    std::cout << "\n";
    for (const shashki::Move& move : possible_moves) {
        std::cout << move.description() << "\n";
    }
    std::cout << "\n";
}

void show_best_hint()
{
    shashki::Move best_move = shashki::best_move(game, 15);
    std::cout << "\n" << best_move.description() << "\n\n";
}

void make_move(std::string input)
{
    std::string move_description = input.substr(input.find(":") + 1, std::string::npos);
    std::vector<shashki::Move> possible_moves = shashki::generate_moves_for_game(game);

    std::vector<shashki::Move>::iterator iterator = std::find_if(possible_moves.begin(), possible_moves.end(), [&](const shashki::Move& move) {
        return move.description() == move_description;
    });

    if (iterator == possible_moves.end()) {
        std::cout << "\nInvalid move!\n\n";
        return;
    }

    game.execute_move(*iterator);
    print_status();
    
    if (game.get_current_turn() != player_side) {
        make_engine_move();
    }
}

void game_loop()
{
    bool running = true;

    while (running) {
        std::cout << "Please enter the next command (type \"HELP\" for help): ";

        std::string input = std::string();
        std::cin >> input;
        std::transform(input.begin(), input.end(), input.begin(), ::toupper);

        if (input.find("MOVE") == 0) {
            make_move(input);
            continue;
        }

        Command command = input_to_command(input);

        switch (command) {
            case Command::UNKNOWN:
                std::cout << "\nInvalid input!\n\n";
                break;
            case Command::HELP:
                print_help();
                break;
            case Command::STATUS:
                print_status();
                break;
            case Command::SETUP:
                setup_game();
                break;
            case Command::HINT:
                show_hint();
                break;
            case Command::BESTHINT:
                show_best_hint();
                break;
            case Command::QUIT:
                running = false;
                break;
        }
    }

    std::cout << "\nExiting shashki-cli application...\n";
}

int main()
{
    setup_game();
    game_loop();

    return 0;
}
