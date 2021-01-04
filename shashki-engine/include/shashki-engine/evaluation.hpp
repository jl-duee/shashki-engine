/**
 * Project: Shashki-Engine
 * Library: shashki-engine
 * Author:  Jean-Luc Düe
 * Module:  evaluation
 * 
 * This module includes functionality for BitBoard evaluation.
 */

#pragma once

#include "shashki-engine/common.hpp"

namespace shashki
{

/**
 * Evaluates the given BitBoard and returns the evaluation
 * as integer type.
 * A positive number means that White has the advantage.
 * A negative number means that Black has the advantage.
 * "0" means that the both sides are even.
 * If the number is higher (lower for Black) it means that
 * the advantage is bigger.
 */
int evaluate_bit_board(const BitBoard& bit_board);

}
