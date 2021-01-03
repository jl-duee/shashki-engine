#include "shashki-engine/move-generation.hpp"

#include <functional>

/**
 * A simple representation of the four directions important for the
 * move generation in Shashki.
 */
enum class Direction
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

/**
 * The MoveDirection is a structure that includes all the information that is important
 * for a move to be made in a specific direction. As it is only possible to move diagonally
 * in Shashki there is a horizontal_direction and a vertical_direction which together form
 * the diagonal direction. It includes other information as well that are specific to its
 * direction so that moving can be implemented in an abstract way.
 * The normal_wall is a wall that blocks pieces from moving over the edge of the board resulting
 * into weird moves (if that would happen). Depending on the direction the wall is only present in
 * two places (horizontal and vertical).
 * The attack_wall is like the normal_wall but blocks pieces from attacking/jumping over the edge.
 * The position_move is the number places the piece moves diagonally. It is either -7/7/-9/9.
 * In case of an attack/jump it is doubled by the move generation.
 * The bit_operation is a function provided for the MoveDirection to alter a bit representation
 * of pieces into that diagonal direction of the MoveDirection.
 * The promotion_check is a function provided to check whether a piece has landed on a promoting field.
 * The promotion_check is depending on the MoveDirection which is why it is included into this structure.
 */
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

// Definition of the normal/attack walls existing for each direction:

const unsigned long long WALL_NORMAL_LEFT = 0b1000000010000000100000001000000010000000100000001000000010000000;
const unsigned long long WALL_NORMAL_RIGHT = 0b0000000100000001000000010000000100000001000000010000000100000001;
const unsigned long long WALL_NORMAL_UP = 0b1111111100000000000000000000000000000000000000000000000000000000;
const unsigned long long WALL_NORMAL_DOWN = 0b0000000000000000000000000000000000000000000000000000000011111111;
const unsigned long long WALL_ATTACK_LEFT = 0b1100000011000000110000001100000011000000110000001100000011000000;
const unsigned long long WALL_ATTACK_RIGHT = 0b0000001100000011000000110000001100000011000000110000001100000011;
const unsigned long long WALL_ATTACK_UP = 0b1111111111111111000000000000000000000000000000000000000000000000;
const unsigned long long WALL_ATTACK_DOWN = 0b0000000000000000000000000000000000000000000000001111111111111111;

// Definition of the MoveDirections possible in a Shashki game:

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

// Declaration of the helper functions:

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

// Implementation of the library functions:

std::vector<shashki::Move> shashki::generate_moves_for_game(const Game& game)
{
    if (game.in_move_combo()) {
        return generate_moves_for_piece(game.get_bit_board(), game.move_combo_piece(), game.capture_bit_board());
    } else {
        return generate_moves_for_side(game.get_bit_board(), game.get_current_turn());
    }
}

std::vector<shashki::Move> shashki::generate_moves_for_side(const BitBoard& bit_board,
                                                            const Side& side)
{
    std::vector<Move> moves = std::vector<Move>();

    // Generate attack (jump) moves first.
    
    generate_attack_moves(moves, bit_board, side, PieceType::MAN, LEFT_UP);
    generate_attack_moves(moves, bit_board, side, PieceType::MAN, RIGHT_UP);
    generate_attack_moves(moves, bit_board, side, PieceType::MAN, LEFT_DOWN);
    generate_attack_moves(moves, bit_board, side, PieceType::MAN, RIGHT_DOWN);
    generate_attack_moves(moves, bit_board, side, PieceType::KING, LEFT_UP);
    generate_attack_moves(moves, bit_board, side, PieceType::KING, RIGHT_UP);
    generate_attack_moves(moves, bit_board, side, PieceType::KING, LEFT_DOWN);
    generate_attack_moves(moves, bit_board, side, PieceType::KING, RIGHT_DOWN);
    
    // Only if there are no attack moves - generate normal moves
    // as jumping in Shashki is obligatory if it is possible.

    if (moves.empty() && side == Side::WHITE) {
        generate_normal_moves(moves, bit_board, side, PieceType::MAN, LEFT_UP);
        generate_normal_moves(moves, bit_board, side, PieceType::MAN, RIGHT_UP);
        generate_normal_moves(moves, bit_board, side, PieceType::KING, LEFT_UP);
        generate_normal_moves(moves, bit_board, side, PieceType::KING, RIGHT_UP);
        generate_normal_moves(moves, bit_board, side, PieceType::KING, LEFT_DOWN);
        generate_normal_moves(moves, bit_board, side, PieceType::KING, RIGHT_DOWN);
    } else if (moves.empty() && side == Side::BLACK) {
        generate_normal_moves(moves, bit_board, side, PieceType::MAN, LEFT_DOWN);
        generate_normal_moves(moves, bit_board, side, PieceType::MAN, RIGHT_DOWN);
        generate_normal_moves(moves, bit_board, side, PieceType::KING, LEFT_UP);
        generate_normal_moves(moves, bit_board, side, PieceType::KING, RIGHT_UP);
        generate_normal_moves(moves, bit_board, side, PieceType::KING, LEFT_DOWN);
        generate_normal_moves(moves, bit_board, side, PieceType::KING, RIGHT_DOWN);
    }
    
    return moves;
}

std::vector<shashki::Move> shashki::generate_moves_for_piece(const BitBoard& bit_board,
                                                             const Piece& piece,
                                                             unsigned long long capture_bit_board)
{
    std::vector<Move> moves = std::vector<Move>();

    generate_attack_moves_for_piece(moves, bit_board, piece, LEFT_UP, capture_bit_board);
    generate_attack_moves_for_piece(moves, bit_board, piece, RIGHT_UP, capture_bit_board);
    generate_attack_moves_for_piece(moves, bit_board, piece, LEFT_DOWN, capture_bit_board);
    generate_attack_moves_for_piece(moves, bit_board, piece, RIGHT_DOWN, capture_bit_board);

    return moves;
}

// Implementation of the helper functions:

/**
 * Prepares the move_bit_board for the parameters passed and calls another
 * helper function to do the bit operations on it.
 * This is the preparation for normal moves.
 * The move_bit_board is the representation of the pieces that shall be moved
 * at once by a bit operation. That way several moves can be added to the legal
 * moves list by only doing one bit operation plus an iteration over the bits
 * of this move_bit_board after the bit operation.
 */
void generate_normal_moves(std::vector<shashki::Move>& moves,
                           const shashki::BitBoard& bit_board,
                           const shashki::Side& side,
                           const shashki::PieceType& piece_type,
                           const MoveDirection& move_direction)
{
    unsigned long long move_bit_board = bit_board.pieces_of_side_and_type(side, piece_type);
    generate_normal_moves_with_bit_board(moves, bit_board, side, piece_type, move_direction, move_bit_board, 1);
}

/**
 * Adds normal moves to the list of legal moves by executing bit operations
 * on the move_bit_board and then iterating over the bits to generate the moves.
 */
void generate_normal_moves_with_bit_board(std::vector<shashki::Move>& moves,
                                          const shashki::BitBoard& bit_board,
                                          const shashki::Side& side,
                                          const shashki::PieceType& piece_type,
                                          const MoveDirection& move_direction,
                                          unsigned long long move_bit_board,
                                          int move_count)
{
    // Remove all pieces that would move over the edge of the board.
    move_bit_board = move_bit_board & ~move_direction.normal_wall;

    // Execute the bit-operation so the pieces move all together.
    move_bit_board = move_direction.bit_operation(move_bit_board);
    // Remove the pieces that landed on another pieces position after the bit-operation.
    move_bit_board = move_bit_board & ~bit_board.blocking_board();

    // Stop the processing if there are no pieces to process
    // as this function is called recursively (used as recursion break).
    if (move_bit_board == 0) {
        return;
    }

    // Iterate over the still existing bits and add moves to the legal moves list for each bit.
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

    // Kings can move over several positions diagonally in one move
    // so the bit-operation is done recursivly on those pieces with the move_count increased.
    // The move_count is the number of places the pieces moves.
    // Recursion will break if the move_bit_board is 0 (if there are no pieces left to iterate through).
    if (piece_type == shashki::PieceType::KING) {
        generate_normal_moves_with_bit_board(moves, bit_board, side, piece_type, move_direction, move_bit_board, move_count + 1);
    }
}

/**
 * Prepares the move_bit_board for the parameters passed and calls another
 * helper function to do the bit operations on it.
 * This is the preparation for attack moves.
 * The move_bit_board is the representation of the pieces that shall be moved
 * at once by a bit operation. That way several moves can be added to the legal
 * moves list by only doing one bit operation plus an iteration over the bits
 * of this move_bit_board after the bit operation.
 */
void generate_attack_moves(std::vector<shashki::Move>& moves,
                           const shashki::BitBoard& bit_board,
                           const shashki::Side& side,
                           const shashki::PieceType& piece_type,
                           const MoveDirection& move_direction)
{
    unsigned long long move_bit_board = bit_board.pieces_of_side_and_type(side, piece_type);
    move_before_enemy(moves, bit_board, side, piece_type, move_direction, 0ULL, move_bit_board, 1);
}

/**
 * Prepares the move_bit_board for the parameters passed and calls another
 * helper function to do the bit operations on it.
 * This is the preparation for attack moves for only one piece.
 * It is used specifically for the generation of attack/jump moves in a combo situation.
 * The move_bit_board is the representation of the pieces that shall be moved
 * at once by a bit operation. That way several moves can be added to the legal
 * moves list by only doing one bit operation plus an iteration over the bits
 * of this move_bit_board after the bit operation.
 */
void generate_attack_moves_for_piece(std::vector<shashki::Move>& moves,
                                     const shashki::BitBoard& bit_board,
                                     const shashki::Piece& piece,
                                     const MoveDirection& move_direction,
                                     unsigned long long capture_bit_board)
{
    unsigned long long move_bit_board = 1ULL << piece.position;
    move_before_enemy(moves, bit_board, piece.side, piece.piece_type, move_direction, capture_bit_board, move_bit_board, 1);
}

/**
 * This bit-operation function handles all the moving that is done before
 * an opponents piece is encountered. For Man type pieces this is just
 * one bit-operation (from its source position onto the position of the opponents piece;
 * The necessary jump from the attacked piece position to the next field is done in "move_after_enemy()").
 * For King type pieces it can be several bit-operation (position moves) as Kings
 * can move several places before attacking an opponents piece. This is why it calls itself
 * recursively for the King pieces but not for the Man pieces.
 * For the pieces that encounter an attacked enemy, "move_after_enemy()" is called,
 * which creates the actual attack moves at the end.
 */
void move_before_enemy(std::vector<shashki::Move>& moves,
                       const shashki::BitBoard& bit_board,
                       const shashki::Side& side,
                       const shashki::PieceType& piece_type,
                       const MoveDirection& move_direction,
                       unsigned long long capture_bit_board,
                       unsigned long long move_bit_board,
                       int move_count)
{
    // Remove all pieces that would move over the edge of the board.
    // Attack wall is used as jumping moves need at least two position moves.
    move_bit_board = move_bit_board & ~move_direction.attack_wall;
    // Execute the bit-operation so the pieces move all together.
    move_bit_board = move_direction.bit_operation(move_bit_board);

    // Remove pieces that, with the bit-operation, moved over a piece
    // that already has been jumped. The capture_bit_board saves
    // the pieces that has been jumped.
    move_bit_board = move_bit_board & ~capture_bit_board;

    // Stop the processing if there are no pieces to process
    // as this function is called recursively (used as recursion break).
    if (move_bit_board == 0) {
        return;
    }

    // Create an attack_bit_board which represents a move_bit_board with pieces
    // that encountered an opponents piece. This is important information to process
    // the attacks in "move_after_enemy()".
    unsigned long long attack_bit_board = move_bit_board & bit_board.blocking_board_of_side(shashki::side_opposite(side));

    // Remove the pieces that landed on another pieces position after the bit-operation.
    // This move_bit_board is used for further recursion of the king pieces in "move_before_enemy()".
    move_bit_board = move_bit_board & ~bit_board.blocking_board();

    // Recursivly calls itself for King pieces since they can move several positions.
    if (piece_type == shashki::PieceType::KING) {
        move_before_enemy(moves, bit_board, side, piece_type, move_direction, capture_bit_board, move_bit_board, move_count + 1);
    }

    // For all pieces that encountered an opponents piece
    // "move_after_enemy()" is called with the attack_bit_board.
    move_after_enemy(moves, bit_board, side, piece_type, move_direction, capture_bit_board, attack_bit_board, move_count + 1, 1);
}

/**
 * This bit-operation handles all the moving that is done after
 * an opponents piece is encountered. For Man type pieces this is only
 * one bit-operation. For King type pieces it can be several since
 * Kings can move several positions after a jumped piece. Therefore
 * this calls itself recursivly for Kings but not for Men.
 * Another important aspect of this is that following moves need
 * to be created after the attack is done as well (if possible).
 */
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
    // Remove pieces that would move over the edge of the board.
    // It seems to have been checked in "move_before_enemy()" but
    // here it is also important for the Kings that can move several
    // positions.
    move_bit_board = move_bit_board & ~move_direction.normal_wall;
    // Execute the bit-operation so the pieces move all together.
    move_bit_board = move_direction.bit_operation(move_bit_board);
    // Remove pieces that, after the bit-operation, are standing
    // on another piece to prevent jumping of two pieces.
    // It also prevents creating moves with two jumps which is not possible.
    // Double jumps should rather be added in form of follow moves.
    move_bit_board = move_bit_board & ~bit_board.blocking_board();

    // Remove pieces that, with the bit-operation, moved over a piece
    // that already has been jumped. The capture_bit_board saves
    // the pieces that has been jumped.
    move_bit_board = move_bit_board & ~capture_bit_board;

    // Stop the processing if there are no pieces to process
    // as this function is called recursively (used as recursion break).
    if (move_bit_board == 0) {
        return;
    }

    // Iterate over the still existing bits and add moves to the legal moves list for each bit.
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

            // Create a new capture_bit_board for the follow moves.
            // This is important to not alter the capture_bit_board of this function
            // that is needed for further recursive calls of itself.
            unsigned long long follow_move_capture_bit_board =
                capture_bit_board | (1ULL << (bit - attack_count * move_direction.position_move));
            // Generate follow moves for the newly created move by using
            // the new capture_bit_board.
            generate_follow_moves(moves.back(), follow_move_capture_bit_board);
        }
    }

    // Kings can move over several positions diagonally in one move after an attack
    // so the bit-operation is done recursivly on those pieces with the move_count increased.
    // The move_count is the number of places the pieces moves.
    // The attack_count is the number of places the pieces moves after encountering an opponents piece.
    // Recursion will break if the move_bit_board is 0 (if there are no pieces left to iterate through).
    if (piece_type == shashki::PieceType::KING) {
        move_after_enemy(moves, bit_board, side, piece_type, move_direction, capture_bit_board, move_bit_board, move_count + 1, attack_count + 1);
    }
}

/**
 * Generates follow moves for a move with the given capture_bit_board.
 * The capture_bit_board is holding information about the already jumped pieces
 * so it is possible to prevent to jump pieces more than once.
 * This helper function shall only be called after an attack,
 * as this is the only possibility where follow moves occur.
 */
void generate_follow_moves(shashki::Move& move,
                           unsigned long long capture_bit_board)
{
    generate_follow_move(move, LEFT_UP, capture_bit_board);
    generate_follow_move(move, RIGHT_UP, capture_bit_board);
    generate_follow_move(move, LEFT_DOWN, capture_bit_board);
    generate_follow_move(move, RIGHT_DOWN, capture_bit_board);
}

/**
 * Prepares the move_bit_board for the parameters passed and calls another
 * helper function to do the bit operations on it.
 * This is the preparation for attack moves that are also follow moves.
 * The move_bit_board is the representation of the pieces that shall be moved
 * at once by a bit operation. That way several moves can be added to the legal
 * moves list by only doing one bit operation plus an iteration over the bits
 * of this move_bit_board after the bit operation.
 */
void generate_follow_move(shashki::Move& move,
                          const MoveDirection& move_direction,
                          unsigned long long capture_bit_board)
{
    unsigned long long move_bit_board = 1ULL << move.get_target_position();
    follow_move_before_enemy(move, move_direction, capture_bit_board, move_bit_board, 1);
}

/**
 * Same as "move_before_enemy()" except that the promotion
 * during a follow move path is handled.
 */
void follow_move_before_enemy(shashki::Move& move,
                              const MoveDirection& move_direction,
                              unsigned long long capture_bit_board,
                              unsigned long long move_bit_board,
                              int move_count)
{
    // Remove all pieces that would move over the edge of the board.
    // Attack wall is used as jumping moves need at least two position moves.
    move_bit_board = move_bit_board & ~move_direction.attack_wall;
    // Execute the bit-operation so the pieces move all together.
    move_bit_board = move_direction.bit_operation(move_bit_board);

    // Remove pieces that, with the bit-operation, moved over a piece
    // that already has been jumped. The capture_bit_board saves
    // the pieces that has been jumped.
    move_bit_board = move_bit_board & ~capture_bit_board;

    // Stop the processing if there are no pieces to process
    // as this function is called recursively (used as recursion break).
    if (move_bit_board == 0) {
        return;
    }

    // Create an attack_bit_board which represents a move_bit_board with pieces
    // that encountered an opponents piece. This is important information to process
    // the attacks in "follow_move_after_enemy()".
    unsigned long long attack_bit_board =
        move_bit_board & move.get_target_bit_board().blocking_board_of_side(move.get_attacked_piece()->side);

    // Remove the pieces that landed on another pieces position after the bit-operation.
    // This move_bit_board is used for further recursion of the king pieces in "follow_move_before_enemy()".
    move_bit_board = move_bit_board & ~move.get_target_bit_board().blocking_board();

    // Recursivly calls itself for King pieces or promoted pieces since they can move several positions.
    if (move.get_moving_piece().piece_type == shashki::PieceType::KING || move.is_promotion()) {
        follow_move_before_enemy(move, move_direction, capture_bit_board, move_bit_board, move_count + 1);
    }

    // For all pieces that encountered an opponents piece
    // "follow_move_after_enemy()" is called with the attack_bit_board.
    follow_move_after_enemy(move, move_direction, capture_bit_board, attack_bit_board, move_count + 1, 1);
}

/**
 * Same as "move_after_enemy()" except that the promotion
 * during a follow move path is handled.
 */
void follow_move_after_enemy(shashki::Move& move,
                             const MoveDirection& move_direction,
                             unsigned long long capture_bit_board,
                             unsigned long long move_bit_board,
                             int move_count,
                             int attack_count)
{
    // Remove pieces that would move over the edge of the board.
    // It seems to have been checked in "follow_move_before_enemy()" but
    // here it is also important for the Kings that can move several
    // positions.
    move_bit_board = move_bit_board & ~move_direction.normal_wall;
    // Execute the bit-operation so the pieces move all together.
    move_bit_board = move_direction.bit_operation(move_bit_board);
    // Remove pieces that, after the bit-operation, are standing
    // on another piece to prevent jumping of two pieces.
    // It also prevents creating moves with two jumps which is not possible.
    // Double jumps should rather be added in form of follow moves.
    move_bit_board = move_bit_board & ~move.get_target_bit_board().blocking_board();

    // Remove pieces that, with the bit-operation, moved over a piece
    // that already has been jumped. The capture_bit_board saves
    // the pieces that has been jumped.
    move_bit_board = move_bit_board & ~capture_bit_board;

    // Stop the processing if there are no pieces to process
    // as this function is called recursively (used as recursion break).
    if (move_bit_board == 0) {
        return;
    }

    // Iterate over the still existing bits and add moves to the legal moves list for each bit.
    for (int bit = 0; bit < 64; bit++) {
        if (move_bit_board & (1ULL << bit)) {
            shashki::Move follow_move = shashki::Move(
                    shashki::Piece(
                        move.get_moving_piece().side,
                        move.is_promotion() ? shashki::PieceType::KING : move.get_moving_piece().piece_type,
                        bit - move_count * move_direction.position_move),
                    bit,
                    shashki::Piece(
                        move.get_attacked_piece()->side,
                        move.get_target_bit_board().piece_type_on_position(bit - attack_count * move_direction.position_move),
                        bit - attack_count * move_direction.position_move),
                    move_direction.promotion_check(
                        move.get_moving_piece().side,
                        move.is_promotion() ? shashki::PieceType::KING : move.get_moving_piece().piece_type,
                        bit),
                    move.get_target_bit_board());

            // Create a new capture_bit_board for the follow moves.
            // This is important to not alter the capture_bit_board of this function
            // that is needed for further recursive calls of itself.
            unsigned long long follow_move_capture_bit_board =
                capture_bit_board | (1ULL << (bit - attack_count * move_direction.position_move));
            // Generate follow moves for the newly created move by using
            // the new capture_bit_board.
            generate_follow_moves(follow_move, follow_move_capture_bit_board);

            // Adds this new follow move (including its follow moves)
            // to the list of follow move.
            move.add_follow_move(follow_move);
        }
    }

    // Kings can move over several positions diagonally in one move after an attack
    // so the bit-operation is done recursivly on those pieces with the move_count increased.
    // The move_count is the number of places the pieces moves.
    // The attack_count is the number of places the pieces moves after encountering an opponents piece.
    // Recursion will break if the move_bit_board is 0 (if there are no pieces left to iterate through).
    if (move.get_moving_piece().piece_type == shashki::PieceType::KING || move.is_promotion()) {
        follow_move_after_enemy(move, move_direction, capture_bit_board, move_bit_board, move_count + 1, attack_count + 1);
    }
}
