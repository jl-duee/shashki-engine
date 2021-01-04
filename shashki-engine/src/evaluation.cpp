#include "shashki-engine/evaluation.hpp"

/**
 * The Importance of the Man piece.
 */
const int WEIGHT_MAN = 1;

/**
 * The Importance of the King piece.
 */
const int WEIGHT_KING = 5;

/**
 * Evaluates one part of the BitBoard. A bit_board_part
 * is a combination of Side and PieceType.
 * For example: White Man.
 * It also takes in the weight of this combination
 *  - how important the PieceType to the game is.
 */
int evaluate_bit_board_part(unsigned long long bit_board_part,
                            int evaluation_weight)
{
    int evaluation = 0;

    for (int bit = 0; bit < 64; bit++) {
        if (bit_board_part & (1ULL << bit)) {
            evaluation += evaluation_weight;
        }
    }

    return evaluation;
}

int shashki::evaluate_bit_board(const BitBoard& bit_board)
{
    int evaluation = 0;

    evaluation += evaluate_bit_board_part(bit_board.white_men, WEIGHT_MAN);
    evaluation += evaluate_bit_board_part(bit_board.white_kings, WEIGHT_KING);
    evaluation += evaluate_bit_board_part(bit_board.black_men, -WEIGHT_MAN);
    evaluation += evaluate_bit_board_part(bit_board.black_kings, -WEIGHT_KING);

    return evaluation;
}
