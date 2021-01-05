/**
 * Project: Shashki-Engine
 * Library: shashki-engine
 * Author:  Jean-Luc Düe
 * Module:  common
 * 
 * This module includes all common data types for the game logic of Shashki.
 */

#pragma once

#include <optional>
#include <vector>
#include <string>

namespace shashki
{

/**
 * The Side enum differentiates between Black and White,
 * which can be the two possible sides to play in a Shashki game.
 */
enum class Side
{
    WHITE,
    BLACK
};

/**
 * Takes in a Side as argument and returns the opposite.
 * It returns Black when the argument is White and vise versa.
 */
Side side_opposite(Side side);

/**
 * Defines the two different types of a piece in Shashki.
 * There is a man (a simple piece) and the king, which is
 * a much stroger piece and is gathered be riching the opponents
 * base line.
 */
enum class PieceType
{
    MAN,
    KING
};

/**
 * Takes a PieceType as argumenet and returns the opposite.
 * It returns Man if the argument is King and vise versa.
 */
PieceType piece_type_opposite(PieceType piece_type);

/**
 * A Piece is a representation of a single piece in the game.
 * It holds data for the side of the piece (Black/White),
 * the type of the piece (Man/King) and its position on the board.
 * The position of the piece can be in the range of 0-63 (inclusive).
 * 
 * Board positions visually explained:
 * 
 * 63 62 61 60 59 58 57 56
 * 55 54 53 52 51 50 49 48
 * 47 46 45 44 43 42 41 40
 * 39 38 37 36 35 34 33 32
 * 31 30 29 28 27 26 25 24
 * 23 22 21 20 19 18 17 16
 * 15 14 13 12 11 10 09 08
 * 07 06 05 04 03 02 01 00
 */
struct Piece
{
    Side        side;
    PieceType   piece_type;
    int         position;

    /**
     * Constructs a piece based on the side, type and position.
     */
    Piece(Side side,
          PieceType piece_type,
          int position);

    /**
     * Compares a piece to another piece. All information needs to be
     * identic to return true for this comparison.
     */
    bool operator == (const Piece& piece) const;
};

/**
 * A BitBoard is the representation of the board in Shashki
 * using a BitBoard technique. Information is not stored in a
 * [8][8] two-dimensional array and also not in a single dimension [64]
 * array. Rather it is stored in the bits of a 64-bit integer type.
 * For each side and piece-type combination exists one 64-bit integer
 * member to save the information. A 1-bit represents that there is a
 * unit of the side/type combination on that place while a 0-bit represents
 * that there is nothing of that side/type combination. There are 4 combinations:
 * white_men, white_kings, black_men and black_kings. Logically if for example
 * the 10th bit of white_men is a 1-bit there cannot be a 1-bit in white_kings, black_men
 * or black_kings on the 10th place as there can be only one piece in one place
 * at the same time in a board constellation. Bits are represented in revers order.
 * 
 * Board bit positions visually explained:
 * 
 * 63 62 61 60 59 58 57 56
 * 55 54 53 52 51 50 49 48
 * 47 46 45 44 43 42 41 40
 * 39 38 37 36 35 34 33 32
 * 31 30 29 28 27 26 25 24
 * 23 22 21 20 19 18 17 16
 * 15 14 13 12 11 10 09 08
 * 07 06 05 04 03 02 01 00
 */
struct BitBoard
{
    unsigned long long white_men;
    unsigned long long white_kings;
    unsigned long long black_men;
    unsigned long long black_kings;

    /**
     * Constructs a BitBoard with the start constellation in Shashki.
     */
    BitBoard();

    /**
     * Constructs a BitBoard with the provided information as constellation.
     * 64-bit integers can be passed in for the 4 side/type combinations.
     */
    BitBoard(unsigned long long white_men,
             unsigned long long white_kings,
             unsigned long long black_men,
             unsigned long long black_kings);

    /**
     * Compares a BitBoard to another BitBoard.
     * All 4 side/type combinations must be identic
     * in order to return true for this comparison.
     */
    bool operator == (const BitBoard& bit_board) const;

    /**
     * Returns one side/type combination, so one 64-bit integer
     * of the given side and piece type.
     */
    unsigned long long pieces_of_side_and_type(Side side,
                                               PieceType piece_type) const;

    /**
     * Returns a 64-bit integer representing a board including all
     * side/type combination into one integer. This can be useful
     * to determine what might block movement of one piece. Blocking
     * occurs to all piece types so the differentiation is not necessary
     * hence all the combinations combined into one 64-bit integer.
     */
    unsigned long long blocking_board() const;

    /**
     * Returns a 64-bit integer representing a board including the
     * combinations of one side (White/Black) into one integer.
     * For example the input "White" will give you a logical OR combination
     * of white_men and white_kings. This can be useful to determine
     * blocking pieces of only one specific side. It is used specially
     * for determining pieces that can be jumped, since it is only possible
     * to jump pieces of the opponent.
     */
    unsigned long long blocking_board_of_side(Side side) const;

    /**
     * Determines the piece type that is on a specific bit position
     * of the BitBoard.
     */
    PieceType piece_type_on_position(int position) const;
};

/**
 * Move is the representation of a move in Shashki possibly containing
 * several moves to follow in situations where multiple pieces can be jumped.
 * It contains the moving_piece which is the piece that makes the move, it contains
 * all the important information of one piece. The type of the piece is the type that
 * is present before the move is executed. So in case of a promotion the type is still "Man"
 * and not King (although the target_bit_board represents it as King already).
 * The target_position is the bit position to what the moving_piece moves to.
 * attacked_piece is optional and can represent a piece that is jumped during this move.
 * promotion is a boolean that shows whether the moving_piece gets promoted to a King or not.
 * source_bit_board holds the bord representation before the move is executed.
 * target_bit_board holds the bord representation after the move is executed.
 * follow_moves is a list of possible moves that can be made after this move.
 * This occurs in situations where another jump can be made when a jump has been made
 * with this move. There can be one or more moves in this lists depending on how many
 * jump paths are possible to follow. If this list is empty there is either no further jumping possible
 * or this move was no jump/attack move at all in which case there never are any following moves.
 */
class Move
{
    private:

    Piece                   moving_piece;
    int                     target_position;
    std::optional<Piece>    attacked_piece;
    bool                    promotion;
    BitBoard                source_bit_board;
    BitBoard                target_bit_board;
    std::vector<Move>       follow_moves;

    public:

    /**
     * Constructs a move with the given information.
     * The target_bit_board is calculated automatically based
     * on the information given. Following moves have to be
     * added later with another function.
     */
    Move(Piece moving_piece,
         int target_position,
         std::optional<Piece> attacked_piece,
         bool promotion,
         BitBoard source_bit_board);
    
    /**
     * Compares two moves by simply comparing their
     * source_bit_board and target_bit_board.
     */
    bool operator == (const Move& move) const;

    /**
     * Adds a following move to this move.
     */
    void add_follow_move(const Move& follow_move);

    /**
     * Clears the follow_moves lists. The list is left empty.
     */
    void clear_follow_moves();

    /**
     * Returns true if the BitBoard given as input is the same as
     * the target_bit_board of this move or the target_bit_board
     * of any of the follow_moves of this move. Basically it returns
     * true if the given BitBoard could be reached with this move or any
     * of the following move paths of this move.
     */
    bool compare_follow_moves_to_bit_board(const BitBoard& bit_board) const;

    /**
     * Shrinks all the possible move paths (all the possible jumps) to
     * only the path that will reach the given BitBoard as target_bit_board.
     * This will result into a move with only one following move in the
     * follow_moves list. This following move will also only have one or no moves
     * in the follow_moves list and so on. (Recursively executed on follow_moves)
     */
    void shrink_follow_moves_to_bit_board(const BitBoard& bit_board);

    /**
     * Shrinks all the possible move paths to one randomly chosen one.
     * This will result into a move with only one following move in the
     * follow_moves list. This following move will also only have one or no moves
     * in the follow_moves list and so on. (Recursively executed on follow_moves)
     */
    void shrink_follow_moves_randomly();

    /**
     * Returns a description of the move (excluding the follow_moves).
     * A normal move might look like this: A3-B4
     * A jump move might look like this: A3-B4-C5
     */
    std::string description() const;

    // Getters:

    const Piece& get_moving_piece() const;
    const int& get_target_position() const;
    const std::optional<Piece>& get_attacked_piece() const;
    const bool& is_promotion() const;
    const BitBoard& get_source_bit_board() const;
    const BitBoard& get_target_bit_board() const;
    const std::vector<Move>& get_follow_moves() const;
};

/**
 * A Game represents all the important information of a Shashki game.
 * Its current board situation is stored in bit_board.
 * The color of the player with the current turn is stored in current_turn.
 * All the moves that has been executed on the game to result into the current bit_board
 * are stored in executed_moves.
 */
class Game
{
    private:

    BitBoard            bit_board;
    Side                current_turn;
    std::vector<Move>   executed_moves;

    public:

    /**
     * Constructs a Game in its initial state following the Shashki rules.
     */
    Game();

    /**
     * Compares a game to another game by comparing
     * current BitBoard situation, the current turn
     * and the list of executed moves.
     */
    bool operator == (const Game& game) const;

    /**
     * Executes a move on a game. The move is copied into
     * the list of executed moves and the current_turn and
     * the current BitBoard situations are altered accordingly.
     * The following moves of the executed move are stripped out (if they exist)
     * as it is not known which path of following moves the player chooses
     * to go if there are any. It can put the game into a combo situation
     * described in the "in_move_combo()" function. The following moves (jumps)
     * need to be executed one by one onto the game.
     */
    void execute_move(const Move& move);

    /**
     * Reverts the current situation by deleting the last moves
     * of both players (White/Black) from the executed moves list.
     * The current turn and the current BitBoard situation are
     * altered accordingly. If this revert contains a move combo
     * all the jumps are reverted.
     */
    void undo_last_move();

    /**
     * Returns true if in the current game situation the player
     * with the current turn is in a combo situation. That means
     * that the player has done a jump but can do at least one more jump.
     * It will return false if the player with the current turn
     * cannot jump any further.
     */
    bool in_move_combo() const;

    /**
     * In a combo situation this function returns the piece that
     * has done the last jump and is able to do at least one more jump.
     * This function shall not be called if the check with in_move_combo()
     * returns false or if in_move_combo() has not been checked at all.
     * The behaviour of this is undefined.
     */
    Piece move_combo_piece() const;

    /**
     * In a combo situation this function returns the capture_bit_board
     * of that situation.
     * The capture_bit_board is a single 64-bit integer type that represents
     * all the pieces that has been jumped over in the current combo move.
     * This is useful in situations where it needs to be guaranteed that no
     * piece is jumped twice (which is not allowed in Shashki).
     * This function shall not be called in the check with in_move_combo()
     * returns false or if in_move_combo() has not been checked at all.
     * The behaviour of this is undefined.
     */
    unsigned long long capture_bit_board() const;

    // Getters:

    const BitBoard& get_bit_board() const;
    const Side& get_current_turn() const;
    const std::vector<Move>& get_executed_moves() const;
};

}
