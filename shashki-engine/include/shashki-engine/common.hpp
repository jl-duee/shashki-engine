/* 
 * Project: Shashki-Engine
 * Library: shashki-engine
 * Author:  Jean-Luc Düe
 * Module:  common
 * 
 * This module includes all common data types for the Shashki-Engine project.
 */

#pragma once

#include <optional>
#include <forward_list>
#include <string>
#include <vector>

namespace shashki
{

enum class Side
{
    WHITE,
    BLACK
};

Side side_opposite(Side side);

enum class PieceType
{
    MAN,
    KING
};

PieceType piece_type_opposite(PieceType piece_type);

struct Piece
{
    Side        side;
    PieceType   piece_type;
    int         position;

    Piece(Side side, PieceType piece_type, int position);

    bool operator == (const Piece& piece) const;
};

struct BitBoard
{
    unsigned long long white_men;
    unsigned long long white_kings;
    unsigned long long black_men;
    unsigned long long black_kings;

    BitBoard();
    BitBoard(unsigned long long white_men,
             unsigned long long white_kings,
             unsigned long long black_men,
             unsigned long long black_kings);

    bool operator == (const BitBoard& bit_board) const;

    unsigned long long pieces_of_side_and_type(Side side, PieceType piece_type) const;
    unsigned long long blocking_board() const;
    unsigned long long blocking_board_of_side(Side side) const;
    PieceType piece_type_on_position(int position) const;
};

class Move
{
    private:

    Piece                   moving_piece;
    int                     target_position;
    std::optional<Piece>    attacked_piece;
    bool                    promotion;
    BitBoard                source_bit_board;
    BitBoard                target_bit_board;
    std::forward_list<Move> follow_moves;

    public:

    Move(Piece moving_piece, int target_position, std::optional<Piece> attacked_piece, bool promotion, BitBoard source_bit_board);
    
    bool operator == (const Move& move) const;

    void add_follow_move(const Move& follow_move);
    void clear_follow_moves();
    bool compare_follow_moves_to_bit_board(const BitBoard& bit_board) const;
    void shrink_follow_moves_to_bit_board(const BitBoard& bit_board);
    void shrink_follow_moves_randomly();
    std::string description() const;

    const Piece& get_moving_piece() const;
    const int& get_target_position() const;
    const std::optional<Piece>& get_attacked_piece() const;
    const bool& is_promotion() const;
    const BitBoard& get_source_bit_board() const;
    const BitBoard& get_target_bit_board() const;
    const std::forward_list<Move>& get_follow_moves() const;
};

class Game
{
    private:

    BitBoard            bit_board;
    Side                current_turn;
    std::vector<Move>   executed_moves;

    public:

    Game();

    bool operator == (const Game& game) const;

    void execute_move(const Move& move);
    void undo_last_move();
    bool in_move_combo() const;
    Piece move_combo_piece() const;
    unsigned long long capture_bit_board() const;

    const BitBoard& get_bit_board() const;
    const Side& get_current_turn() const;
    const std::vector<Move>& get_executed_moves() const;
};

}
