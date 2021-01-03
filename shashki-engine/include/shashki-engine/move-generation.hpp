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

std::vector<Move> generate_moves_for_game(const Game& game);

std::vector<Move> generate_moves_for_side(const BitBoard& bit_board, const Side& side);

std::vector<Move> generate_moves_for_piece(const BitBoard& bit_board, const Piece& piece, unsigned long long capture_bit_board);

}
