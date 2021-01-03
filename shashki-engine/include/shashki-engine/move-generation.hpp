/* 
 * Project: Shashki-Engine
 * Library: shashki-engine
 * Author:  Jean-Luc Düe
 * Module:  move-generation
 * 
 * This module includes functionality for the generation of
 * legal moves by utilizing the common module.
 */

#pragma once

#include <vector>
#include "shashki-engine/common.hpp"

namespace shashki
{

/**
 * Generates legal moves for the given game.
 * This considers the BitBoard situation of the given game
 * and the current side that has the turn. It also considers
 * if the game is in a combo situation - then all possible
 * follow moves (jumps) are returned.
 * The moves are returned including all its possible follow_moves.
 */
std::vector<Move> generate_moves_for_game(const Game& game);

/**
 * Generates legal moves for the given BitBoard and for the given Side.
 * This does not consider a combo situation so it shall not be used in a combo situation
 * because the rule of not jumping over jumped pieces once again will be disregarded.
 */
std::vector<Move> generate_moves_for_side(const BitBoard& bit_board,
                                          const Side& side);

/**
 * Generates legal moves only for a specific piece for the given BitBoard.
 * This function should only be used in combo situations and the information of the
 * already jumped pieces position has to be passed as capture_bit_board.
 * It only provides attack moves for the piece so it would not be correct to use this
 * for non-combo situations.
 */
std::vector<Move> generate_moves_for_piece(const BitBoard& bit_board,
                                           const Piece& piece,
                                           unsigned long long capture_bit_board);

}
