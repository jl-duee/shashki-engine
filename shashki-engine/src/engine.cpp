#include "shashki-engine/engine.hpp"

#include <forward_list>
#include <vector>
#include <random>
#include "shashki-engine/move-generation.hpp"
#include "shashki-engine/evaluation.hpp"

/**
 * An EngineNode is a single node of the engine tree.
 * It represents a Board constellation in a game of Shashki.
 * Therefore each EngineNode has its own BitBoard to represent
 * this constellation. The EngineNode also includes a list of children
 * nodes that are also EngineNodes which represent all the possible Board
 * constellations that are the outcome of all the possible moves
 * (including following moves / move combos) that can be made from the
 * parents node board constellation.
 * The goal is to build an engine tree which is a hirarchy of EngineNodes
 * to a given level (depth). The first level is only one single node called
 * the start_node in this implementation. It is the current situation of a game.
 * When this tree is built it can be traversed with the minimax algorythm
 * to evaluate the best board constellations and with that to find the best
 * move to execute next.
 */
struct EngineNode
{
    shashki::BitBoard               bit_board;
    std::forward_list<EngineNode>   child_nodes;

    EngineNode(shashki::BitBoard bit_board)
        : bit_board(bit_board),
          child_nodes(std::forward_list<EngineNode>()) {}
};

/**
 * The EngineResult is a construct that holds an evaluation value
 * of a BitBoard. It is used as return value for the minimax algorythm so it
 * can be used for recursiv comparison between the levels of the engine tree.
 * The EngineResult also contains the ancestor_move_bit_board which is the
 * resulting BitBoard of the first set of moves that derive from the start_node
 * (first node and current board situation of the game). The minimax algorythm
 * will return the best EngineResult for the current player in turn with the best evaluation
 * value for the future (with the given depth) and the ancestor_move_bit_board
 * which is the BitBoard that shall be reached with the best move to execute next.
 */
struct EngineResult
{
    int                 evaluation_value;
    shashki::BitBoard*  ancestor_move_bit_board;

    EngineResult(int evaluation_value,
                 shashki::BitBoard* ancestor_move_bit_board)
        : evaluation_value(evaluation_value),
          ancestor_move_bit_board(ancestor_move_bit_board) {}
};

/**
 * This function converts a move with all its possible combos
 * into child nodes of the engine tree. Each ending of a combo move path
 * will result into a BitBoard that is used for creating one child.
 * 
 * Example:
 * If there is a move where it is possible to jump in two different directions
 * and for both directions there is another following jump possible in two different
 * directions then there are four different BitBoard outcomes for this move combo.
 * This means four new child nodes are created for the given parent node (engine_node).
 */
void convert_move_combo_to_child_nodes(EngineNode& engine_node,
                                       const shashki::Move& move)
{
    if (move.get_follow_moves().empty()) {
        engine_node.child_nodes.push_front(EngineNode(move.get_target_bit_board()));
    } else {
        for (const shashki::Move& follow_move : move.get_follow_moves()) {
            convert_move_combo_to_child_nodes(engine_node, follow_move);
        }
    }
}

/**
 * This function does two important things.
 * 1. It creates child nodes for the given engine_node (parent).
 * 2. It iterates through the child nodes with a minimax algorythm including
 *    alpha- and beta- pruning recursivly to find the best result of the childs.
 * 
 * These two functionalities are together in one function to prevent
 * building unnecessary tree hirarchy that is pruned away by alpha and beta pruning
 * of the minimax algorythm. So instead of building every possible outcome, only the ones
 * with significant results are build recursivly. (Saves memory and processing time)
 * 
 * At the end an EngineResult is returned that represents the best outcome for the given
 * depth including the BitBoard that shall be reached with the best move that should be
 * executed next.
 */
EngineResult build_and_evaluate(EngineNode& engine_node,
                                shashki::Side side,
                                int depth,
                                int alpha = -100,
                                int beta = 100,
                                shashki::BitBoard* ancestor_move_bit_board = NULL)
{
    // If the given depth is reached, return the evaluation of this depth.
    if (depth == 0) {
        return EngineResult(shashki::evaluate_bit_board(engine_node.bit_board), ancestor_move_bit_board);
    }

    // Create possible moves for the current engine_node that is traversed.
    std::vector<shashki::Move> possible_moves = shashki::generate_moves_for_side(engine_node.bit_board, side);

    // If there are no moves possible, return the evaluation of this depth.
    if (possible_moves.empty()) {
        return EngineResult(shashki::evaluate_bit_board(engine_node.bit_board), ancestor_move_bit_board);
    }

    // Convert the move combos into child nodes and attach them to the current engine_node.
    for (const shashki::Move& move : possible_moves) {
        convert_move_combo_to_child_nodes(engine_node, move);
    }

    // The minimax evaluation with alpha- and beta- pruning follows.
    if (side == shashki::Side::WHITE) {
        EngineResult maximum = EngineResult(-100, NULL);

        for (EngineNode& child_node : engine_node.child_nodes) {
            EngineResult evaluation = build_and_evaluate(child_node, shashki::Side::BLACK, depth - 1, alpha, beta,
                                                         ancestor_move_bit_board == NULL ? &child_node.bit_board : ancestor_move_bit_board);
            
            if (evaluation.evaluation_value > maximum.evaluation_value) {
                maximum = evaluation;
            }

            if (evaluation.evaluation_value > alpha) {
                alpha = evaluation.evaluation_value;
            }

            if (beta <= alpha) {
                break;
            }
        }

        return maximum;
    } else {
        EngineResult minimum = EngineResult(100, NULL);

        for (EngineNode& child_node : engine_node.child_nodes) {
            EngineResult evaluation = build_and_evaluate(child_node, shashki::Side::WHITE, depth - 1, alpha, beta,
                                                         ancestor_move_bit_board == NULL ? &child_node.bit_board : ancestor_move_bit_board);
            
            if (evaluation.evaluation_value < minimum.evaluation_value) {
                minimum = evaluation;
            }

            if (evaluation.evaluation_value < beta) {
                beta = evaluation.evaluation_value;
            }

            if (beta <= alpha) {
                break;
            }
        }

        return minimum;
    }
}

shashki::Move shashki::best_move(const Game& game,
                                 int depth)
{
    // Create the start node from the current game situation.
    EngineNode start_node = EngineNode(game.get_bit_board());
    // Build the engine tree and evaluate to get the best EngineResult.
    EngineResult engine_result = build_and_evaluate(start_node, game.get_current_turn(), depth);

    // Generate the possible moves for the current game situation.
    std::vector<Move> possible_moves = generate_moves_for_game(game);

    // Traverse the possible moves and find the one that matches the EngineResult.
    // Shrink the move combo (if it exists with several paths) to the path that is
    // necessary to use to get to the BitBoard constellation of the EngineResult.
    for (const Move& move : possible_moves) {
        if (move.compare_follow_moves_to_bit_board(*engine_result.ancestor_move_bit_board)) {
            Move best_move = move;
            best_move.shrink_follow_moves_to_bit_board(*engine_result.ancestor_move_bit_board);
            return best_move;
        }
    }

    // If no matching move could be found, return a random possible move.
    return random_move(game);
}

shashki::Move shashki::random_move(const Game& game)
{
    std::vector<Move> possible_moves = generate_moves_for_game(game);
    
    std::mt19937_64 random_number_generator = std::mt19937_64(std::random_device{}());
    std::uniform_int_distribution<int> random_distribution = std::uniform_int_distribution<int>(0, possible_moves.size() - 1);

    Move random_move = possible_moves[random_distribution(random_number_generator)];
    random_move.shrink_follow_moves_randomly();
    
    return random_move;
}
