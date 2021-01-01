#include "shashki-engine/move-generation.hpp"

#include <functional>

enum class Direction
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct MoveDirection
{
    Direction                                                               horizontal_direction;
    Direction                                                               vertical_direction;
    unsigned long long                                                      normal_wall;
    unsigned long long                                                      attack_wall;
    int                                                                     position_move;
    std::function<unsigned long long(unsigned long long bits)>              bit_operation;
    std::function<bool(shashki::Side, shashki::PieceType, int position)>    promotion_check;
};

const unsigned long long WALL_NORMAL_LEFT = 0b1000000010000000100000001000000010000000100000001000000010000000;
const unsigned long long WALL_NORMAL_RIGHT = 0b0000000100000001000000010000000100000001000000010000000100000001;
const unsigned long long WALL_NORMAL_UP = 0b1111111100000000000000000000000000000000000000000000000000000000;
const unsigned long long WALL_NORMAL_DOWN = 0b0000000000000000000000000000000000000000000000000000000011111111;
const unsigned long long WALL_ATTACK_LEFT = 0b1100000011000000110000001100000011000000110000001100000011000000;
const unsigned long long WALL_ATTACK_RIGHT = 0b0000001100000011000000110000001100000011000000110000001100000011;
const unsigned long long WALL_ATTACK_UP = 0b1111111111111111000000000000000000000000000000000000000000000000;
const unsigned long long WALL_ATTACK_DOWN = 0b0000000000000000000000000000000000000000000000001111111111111111;

const MoveDirection LEFT_UP = {
    Direction::LEFT,
    Direction::UP,
    WALL_NORMAL_LEFT | WALL_NORMAL_UP,
    WALL_ATTACK_LEFT | WALL_ATTACK_UP,
    9,
    [](unsigned long long bits) { return bits << 9; },
    [](shashki::Side side, shashki::PieceType piece_type, int position)
        { return side == shashki::Side::WHITE && piece_type == shashki::PieceType::MAN && position > 55; }
};

const MoveDirection RIGHT_UP = {
    Direction::RIGHT,
    Direction::UP,
    WALL_NORMAL_RIGHT | WALL_NORMAL_UP,
    WALL_ATTACK_RIGHT | WALL_ATTACK_UP,
    7,
    [](unsigned long long bits) { return bits << 7; },
    [](shashki::Side side, shashki::PieceType piece_type, int position)
        { return side == shashki::Side::WHITE && piece_type == shashki::PieceType::MAN && position > 55; }
};

const MoveDirection LEFT_DOWN = {
    Direction::LEFT,
    Direction::DOWN,
    WALL_NORMAL_LEFT | WALL_NORMAL_DOWN,
    WALL_ATTACK_LEFT | WALL_ATTACK_DOWN,
    -7,
    [](unsigned long long bits) { return bits >> 7; },
    [](shashki::Side side, shashki::PieceType piece_type, int position)
        { return side == shashki::Side::BLACK && piece_type == shashki::PieceType::MAN && position < 8; }
};

const MoveDirection RIGHT_DOWN = {
    Direction::RIGHT,
    Direction::DOWN,
    WALL_NORMAL_RIGHT | WALL_NORMAL_DOWN,
    WALL_ATTACK_RIGHT | WALL_ATTACK_DOWN,
    -9,
    [](unsigned long long bits) { return bits >> 9; },
    [](shashki::Side side, shashki::PieceType piece_type, int position)
        { return side == shashki::Side::BLACK && piece_type == shashki::PieceType::MAN && position < 8; }
};

// Declaration of helper functions:

void generate_normal_moves(std::vector<shashki::Move>& moves, const shashki::BitBoard& bit_board, const shashki::Side& side, const shashki::PieceType& piece_type, const MoveDirection& move_direction);
void generate_normal_moves_with_bit_board(std::vector<shashki::Move>& moves, const shashki::BitBoard& bit_board, const shashki::Side& side, const shashki::PieceType& piece_type, const MoveDirection& move_direction, unsigned long long move_bit_board, int move_count);
void generate_attack_moves(std::vector<shashki::Move>& moves, const shashki::BitBoard& bit_board, const shashki::Side& side, const shashki::PieceType& piece_type, const MoveDirection& move_direction);
void generate_attack_moves_for_piece(std::vector<shashki::Move>& moves, const shashki::BitBoard& bit_board, const shashki::Piece& piece, const MoveDirection& move_direction, unsigned long long capture_bit_board);
void move_before_enemy(std::vector<shashki::Move>& moves, const shashki::BitBoard& bit_board, const shashki::Side& side, const shashki::PieceType& piece_type, const MoveDirection& move_direction, unsigned long long capture_bit_board, unsigned long long move_bit_board, int move_count);
void move_after_enemy(std::vector<shashki::Move>& moves, const shashki::BitBoard& bit_board, const shashki::Side& side, const shashki::PieceType& piece_type, const MoveDirection& move_direction, unsigned long long capture_bit_board, unsigned long long move_bit_board, int move_count, int attack_count);
void generate_follow_moves(shashki::Move& move, unsigned long long capture_bit_board);
void generate_follow_move(shashki::Move& move, const MoveDirection& move_direction, unsigned long long capture_bit_board);
void follow_move_before_enemy(shashki::Move& move, const MoveDirection& move_direction, unsigned long long capture_bit_board, unsigned long long move_bit_board, int move_count);
void follow_move_after_enemy(shashki::Move& move, const MoveDirection& move_direction, unsigned long long capture_bit_board, unsigned long long move_bit_board, int move_count, int attack_count);

// Implementation:

std::vector<shashki::Move>* shashki::generate_moves_for_game(const Game& game)
{
    if (game.in_move_combo()) {
        return generate_moves_for_piece(game.get_bit_board(), game.move_combo_piece(), game.capture_bit_board());
    } else {
        return generate_moves_for_side(game.get_bit_board(), game.get_current_turn());
    }
}

std::vector<shashki::Move>* shashki::generate_moves_for_side(const BitBoard& bit_board, const Side& side)
{
    std::vector<Move>* moves = new std::vector<Move>();
    
    generate_attack_moves(*moves, bit_board, side, PieceType::MAN, LEFT_UP);
    generate_attack_moves(*moves, bit_board, side, PieceType::MAN, RIGHT_UP);
    generate_attack_moves(*moves, bit_board, side, PieceType::MAN, LEFT_DOWN);
    generate_attack_moves(*moves, bit_board, side, PieceType::MAN, RIGHT_DOWN);
    generate_attack_moves(*moves, bit_board, side, PieceType::KING, LEFT_UP);
    generate_attack_moves(*moves, bit_board, side, PieceType::KING, RIGHT_UP);
    generate_attack_moves(*moves, bit_board, side, PieceType::KING, LEFT_DOWN);
    generate_attack_moves(*moves, bit_board, side, PieceType::KING, RIGHT_DOWN);
    
    if (moves->empty() && side == Side::WHITE) {
        generate_normal_moves(*moves, bit_board, side, PieceType::MAN, LEFT_UP);
        generate_normal_moves(*moves, bit_board, side, PieceType::MAN, RIGHT_UP);
        generate_normal_moves(*moves, bit_board, side, PieceType::KING, LEFT_UP);
        generate_normal_moves(*moves, bit_board, side, PieceType::KING, RIGHT_UP);
        generate_normal_moves(*moves, bit_board, side, PieceType::KING, LEFT_DOWN);
        generate_normal_moves(*moves, bit_board, side, PieceType::KING, RIGHT_DOWN);
    } else if (moves->empty() && side == Side::BLACK) {
        generate_normal_moves(*moves, bit_board, side, PieceType::MAN, LEFT_DOWN);
        generate_normal_moves(*moves, bit_board, side, PieceType::MAN, RIGHT_DOWN);
        generate_normal_moves(*moves, bit_board, side, PieceType::KING, LEFT_UP);
        generate_normal_moves(*moves, bit_board, side, PieceType::KING, RIGHT_UP);
        generate_normal_moves(*moves, bit_board, side, PieceType::KING, LEFT_DOWN);
        generate_normal_moves(*moves, bit_board, side, PieceType::KING, RIGHT_DOWN);
    }
    
    return moves;
}

std::vector<shashki::Move>* shashki::generate_moves_for_piece(const BitBoard& bit_board, const Piece& piece, unsigned long long capture_bit_board)
{
    std::vector<Move>* moves = new std::vector<Move>();

    generate_attack_moves_for_piece(*moves, bit_board, piece, LEFT_UP, capture_bit_board);
    generate_attack_moves_for_piece(*moves, bit_board, piece, RIGHT_UP, capture_bit_board);
    generate_attack_moves_for_piece(*moves, bit_board, piece, LEFT_DOWN, capture_bit_board);
    generate_attack_moves_for_piece(*moves, bit_board, piece, RIGHT_DOWN, capture_bit_board);

    return moves;
}

void generate_normal_moves(std::vector<shashki::Move>& moves,
                           const shashki::BitBoard& bit_board,
                           const shashki::Side& side,
                           const shashki::PieceType& piece_type,
                           const MoveDirection& move_direction)
{
    unsigned long long move_bit_board = bit_board.pieces_of_side_and_type(side, piece_type);
    generate_normal_moves_with_bit_board(moves, bit_board, side, piece_type, move_direction, move_bit_board, 1);
}

void generate_normal_moves_with_bit_board(std::vector<shashki::Move>& moves,
                                          const shashki::BitBoard& bit_board,
                                          const shashki::Side& side,
                                          const shashki::PieceType& piece_type,
                                          const MoveDirection& move_direction,
                                          unsigned long long move_bit_board,
                                          int move_count)
{
    move_bit_board = move_bit_board & ~move_direction.normal_wall;

    move_bit_board = move_direction.bit_operation(move_bit_board);
    move_bit_board = move_bit_board & ~bit_board.blocking_board();

    if (move_bit_board == 0) {
        return;
    }

    for (int bit = 0; bit < 64; bit++) {
        if (move_bit_board & (1ULL << bit)) {
            moves.push_back(
                shashki::Move(
                    shashki::Piece(side, piece_type, bit - move_count * move_direction.position_move),
                    bit,
                    std::optional<shashki::Piece>(),
                    move_direction.promotion_check(side, piece_type, bit),
                    bit_board));
        }
    }

    if (piece_type == shashki::PieceType::KING) {
        generate_normal_moves_with_bit_board(moves, bit_board, side, piece_type, move_direction, move_bit_board, move_count + 1);
    }
}

void generate_attack_moves(std::vector<shashki::Move>& moves,
                           const shashki::BitBoard& bit_board,
                           const shashki::Side& side,
                           const shashki::PieceType& piece_type,
                           const MoveDirection& move_direction)
{
    unsigned long long move_bit_board = bit_board.pieces_of_side_and_type(side, piece_type);
    move_before_enemy(moves, bit_board, side, piece_type, move_direction, 0ULL, move_bit_board, 1);
}

void generate_attack_moves_for_piece(std::vector<shashki::Move>& moves,
                                     const shashki::BitBoard& bit_board,
                                     const shashki::Piece& piece,
                                     const MoveDirection& move_direction,
                                     unsigned long long capture_bit_board)
{
    unsigned long long move_bit_board = 1ULL << piece.position;
    move_before_enemy(moves, bit_board, piece.side, piece.piece_type, move_direction, capture_bit_board, move_bit_board, 1);
}

void move_before_enemy(std::vector<shashki::Move>& moves,
                       const shashki::BitBoard& bit_board,
                       const shashki::Side& side,
                       const shashki::PieceType& piece_type,
                       const MoveDirection& move_direction,
                       unsigned long long capture_bit_board,
                       unsigned long long move_bit_board,
                       int move_count)
{
    move_bit_board = move_bit_board & ~move_direction.attack_wall;
    move_bit_board = move_direction.bit_operation(move_bit_board);

    move_bit_board = move_bit_board & ~capture_bit_board;

    if (move_bit_board == 0) {
        return;
    }

    unsigned long long attack_bit_board = move_bit_board & bit_board.blocking_board_of_side(shashki::side_opposite(side));
    move_bit_board = move_bit_board & ~bit_board.blocking_board();

    if (piece_type == shashki::PieceType::KING) {
        move_before_enemy(moves,
                          bit_board,
                          side,
                          piece_type,
                          move_direction,
                          capture_bit_board,
                          move_bit_board,
                          move_count + 1);
    }

    move_after_enemy(moves,
                     bit_board,
                     side,
                     piece_type,
                     move_direction,
                     capture_bit_board,
                     attack_bit_board,
                     move_count + 1,
                     1);
}

void move_after_enemy(std::vector<shashki::Move>& moves,
                      const shashki::BitBoard& bit_board,
                      const shashki::Side& side,
                      const shashki::PieceType& piece_type,
                      const MoveDirection& move_direction,
                      unsigned long long capture_bit_board,
                      unsigned long long move_bit_board,
                      int move_count,
                      int attack_count)
{
    move_bit_board = move_bit_board & ~move_direction.normal_wall;
    move_bit_board = move_direction.bit_operation(move_bit_board);
    move_bit_board = move_bit_board & ~bit_board.blocking_board();

    move_bit_board = move_bit_board & ~capture_bit_board;

    if (move_bit_board == 0) {
        return;
    }

    for (int bit = 0; bit < 64; bit++) {
        if (move_bit_board & (1ULL << bit)) {
            moves.push_back(
                shashki::Move(
                    shashki::Piece(side, piece_type, bit - move_count * move_direction.position_move),
                    bit,
                    shashki::Piece(
                        shashki::side_opposite(side),
                        bit_board.piece_type_on_position(bit - attack_count * move_direction.position_move),
                        bit - attack_count * move_direction.position_move),
                    move_direction.promotion_check(side, piece_type, bit),
                    bit_board));

            unsigned long long follow_move_capture_bit_board =
                capture_bit_board | (1ULL << (bit - attack_count * move_direction.position_move));
            generate_follow_moves(moves.back(), follow_move_capture_bit_board);
        }
    }

    if (piece_type == shashki::PieceType::KING) {
        move_after_enemy(moves,
                         bit_board,
                         side,
                         piece_type,
                         move_direction,
                         capture_bit_board,
                         move_bit_board,
                         move_count + 1,
                         attack_count + 1);
    }
}

void generate_follow_moves(shashki::Move& move,
                           unsigned long long capture_bit_board)
{
    generate_follow_move(move, LEFT_UP, capture_bit_board);
    generate_follow_move(move, RIGHT_UP, capture_bit_board);
    generate_follow_move(move, LEFT_DOWN, capture_bit_board);
    generate_follow_move(move, RIGHT_DOWN, capture_bit_board);
}

void generate_follow_move(shashki::Move& move,
                          const MoveDirection& move_direction,
                          unsigned long long capture_bit_board)
{
    unsigned long long move_bit_board = 1ULL << move.get_target_position();
    follow_move_before_enemy(move, move_direction, capture_bit_board, move_bit_board, 1);
}

void follow_move_before_enemy(shashki::Move& move,
                              const MoveDirection& move_direction,
                              unsigned long long capture_bit_board,
                              unsigned long long move_bit_board,
                              int move_count)
{
    move_bit_board = move_bit_board & ~move_direction.attack_wall;
    move_bit_board = move_direction.bit_operation(move_bit_board);

    move_bit_board = move_bit_board & ~capture_bit_board;

    if (move_bit_board == 0) {
        return;
    }

    unsigned long long attack_bit_board =
        move_bit_board & move.get_target_bit_board().blocking_board_of_side(move.get_attacked_piece().value().side);
    move_bit_board = move_bit_board & ~move.get_target_bit_board().blocking_board();

    if (move.get_moving_piece().piece_type == shashki::PieceType::KING || move.is_promotion()) {
        follow_move_before_enemy(move, move_direction, capture_bit_board, move_bit_board, move_count + 1);
    }

    follow_move_after_enemy(move, move_direction, capture_bit_board, attack_bit_board, move_count + 1, 1);
}

void follow_move_after_enemy(shashki::Move& move,
                             const MoveDirection& move_direction,
                             unsigned long long capture_bit_board,
                             unsigned long long move_bit_board,
                             int move_count,
                             int attack_count)
{
    move_bit_board = move_bit_board & ~move_direction.normal_wall;
    move_bit_board = move_direction.bit_operation(move_bit_board);
    move_bit_board = move_bit_board & ~move.get_target_bit_board().blocking_board();

    move_bit_board = move_bit_board & ~capture_bit_board;

    if (move_bit_board == 0) {
        return;
    }

    for (int bit = 0; bit < 64; bit++) {
        if (move_bit_board & (1ULL << bit)) {
            shashki::Move follow_move = shashki::Move(
                    shashki::Piece(
                        move.get_moving_piece().side,
                        move.is_promotion() ? shashki::PieceType::KING : move.get_moving_piece().piece_type,
                        bit - move_count * move_direction.position_move),
                    bit,
                    shashki::Piece(
                        move.get_attacked_piece().value().side,
                        move.get_target_bit_board().piece_type_on_position(bit - attack_count * move_direction.position_move),
                        bit - attack_count * move_direction.position_move),
                    move_direction.promotion_check(
                        move.get_moving_piece().side,
                        move.is_promotion() ? shashki::PieceType::KING : move.get_moving_piece().piece_type,
                        bit),
                    move.get_target_bit_board());

            unsigned long long follow_move_capture_bit_board =
                capture_bit_board | (1ULL << (bit - attack_count * move_direction.position_move));
            generate_follow_moves(follow_move, follow_move_capture_bit_board);

            move.add_follow_move(follow_move);
        }
    }

    if (move.get_moving_piece().piece_type == shashki::PieceType::KING || move.is_promotion()) {
        follow_move_after_enemy(move, move_direction, capture_bit_board, move_bit_board, move_count + 1, attack_count + 1);
    }
}

// TODOs: Check optional values() -> direct access!
// TODOs: Check whether move list allocation necessary (faster on stack)!
// TODOs: Check move semantic for faster code
