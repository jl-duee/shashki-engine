/**
 * Project: Shashki-Engine
 * Library: shashki-engine
 * Author:  Jean-Luc Düe
 * Module:  engine
 * 
 * This module includes the engine functionality.
 */

#pragma once

#include "shashki-engine/common.hpp"

namespace shashki
{

/**
 * Returns the best move for the given game calculated
 * by the engine with the given depth of the engine tree.
 */
Move best_move(const Game& game, int depth);

/**
 * Returns a random move for the given game.
 */
Move random_move(const Game& game);

}
