#include "shashki-engine/common.hpp"

#include <algorithm>
#include <random>

shashki::Side shashki::side_opposite(Side side)
{
    if (side == Side::WHITE) {
        return Side::BLACK;
    } else {
        return Side::WHITE;
    }
}

shashki::PieceType shashki::piece_type_opposite(PieceType piece_type)
{
    if (piece_type == PieceType::MAN) {
        return PieceType::KING;
    } else {
        return PieceType::MAN;
    }
}

shashki::Piece::Piece(Side side, PieceType piece_type, int position)
    : side(side), piece_type(piece_type), position(position) {}

bool shashki::Piece::operator==(const Piece& piece) const
{
    return this->side == piece.side
        && this->piece_type == piece.piece_type
        && this->position == piece.position;
}

shashki::BitBoard::BitBoard()
    : white_men(0b0000000000000000000000000000000000000000101010100101010110101010),
      white_kings(0b0000000000000000000000000000000000000000000000000000000000000000),
      black_men(0b0101010110101010010101010000000000000000000000000000000000000000),
      black_kings(0b0000000000000000000000000000000000000000000000000000000000000000) {}

shashki::BitBoard::BitBoard(unsigned long long white_men,
                            unsigned long long white_kings,
                            unsigned long long black_men,
                            unsigned long long black_kings)
    : white_men(white_men),
      white_kings(white_kings),
      black_men(black_men),
      black_kings(black_kings) {}

bool shashki::BitBoard::operator==(const BitBoard& bit_board) const
{
    return this->white_men == bit_board.white_men
        && this->white_kings == bit_board.white_kings
        && this->black_men == bit_board.black_men
        && this->black_kings == bit_board.black_kings;
}

unsigned long long shashki::BitBoard::pieces_of_side_and_type(Side side, PieceType piece_type) const
{
    if (side == Side::WHITE) {
        if (piece_type == PieceType::MAN) {
            return this->white_men;
        } else {
            return this->white_kings;
        }
    } else {
        if (piece_type == PieceType::MAN) {
            return this->black_men;
        } else {
            return this->black_kings;
        }
    }
}

unsigned long long shashki::BitBoard::blocking_board() const
{
    return this->white_men | this->white_kings | this->black_men | this->black_kings;
}

unsigned long long shashki::BitBoard::blocking_board_of_side(Side side) const
{
    if (side == Side::WHITE) {
        return this->white_men | this->white_kings;
    } else {
        return this->black_men | this->black_kings;
    }
}

shashki::PieceType shashki::BitBoard::piece_type_on_position(int position) const
{
    return (this->white_men | this->black_men) & (1ULL << position) ? PieceType::MAN : PieceType::KING;
}

shashki::Move::Move(Piece moving_piece, int target_position, std::optional<Piece> attacked_piece, bool promotion, BitBoard source_bit_board)
    : moving_piece(moving_piece),
      target_position(target_position),
      attacked_piece(attacked_piece),
      promotion(promotion),
      source_bit_board(source_bit_board),
      target_bit_board(source_bit_board),
      follow_moves(std::forward_list<Move>())
{
    this->target_bit_board.white_men = this->target_bit_board.white_men & ~(1ULL << this->moving_piece.position);
    this->target_bit_board.white_kings = this->target_bit_board.white_kings & ~(1ULL << this->moving_piece.position);
    this->target_bit_board.black_men = this->target_bit_board.black_men & ~(1ULL << this->moving_piece.position);
    this->target_bit_board.black_kings = this->target_bit_board.black_kings & ~(1ULL << this->moving_piece.position);

    if (this->attacked_piece.has_value()) {
        this->target_bit_board.white_men = this->target_bit_board.white_men & ~(1ULL << this->attacked_piece.value().position);
        this->target_bit_board.white_kings = this->target_bit_board.white_kings & ~(1ULL << this->attacked_piece.value().position);
        this->target_bit_board.black_men = this->target_bit_board.black_men & ~(1ULL << this->attacked_piece.value().position);
        this->target_bit_board.black_kings = this->target_bit_board.black_kings & ~(1ULL << this->attacked_piece.value().position);
    }

    if (this->moving_piece.side == Side::WHITE && (this->promotion || this->moving_piece.piece_type == PieceType::KING)) {
        this->target_bit_board.white_kings = this->target_bit_board.white_kings | (1ULL << this->target_position);
    } else if (this->moving_piece.side == Side::WHITE) {
        this->target_bit_board.white_men = this->target_bit_board.white_men | (1ULL << this->target_position);
    } else if (this->moving_piece.side == Side::BLACK && (this->promotion || this->moving_piece.piece_type == PieceType::KING)) {
        this->target_bit_board.black_kings = this->target_bit_board.black_kings | (1ULL << this->target_position);
    } else {
        this->target_bit_board.black_men = this->target_bit_board.black_men | (1ULL << this->target_position);
    }
}

bool shashki::Move::operator==(const Move& move) const
{
    return this->source_bit_board == move.source_bit_board
        && this->target_bit_board == move.target_bit_board;
}

void shashki::Move::add_follow_move(const Move& follow_move)
{
    this->follow_moves.push_front(follow_move);
}

void shashki::Move::clear_follow_moves()
{
    this->follow_moves.clear();
}

bool shashki::Move::compare_follow_moves_to_bit_board(const BitBoard& bit_board) const
{
    if (this->follow_moves.empty()) {
        return this->target_bit_board == bit_board;
    } else {
        return std::any_of(this->follow_moves.begin(), this->follow_moves.end(), [&](const Move& follow_move) {
            return follow_move.compare_follow_moves_to_bit_board(bit_board);
        });
    }
}

void shashki::Move::shrink_follow_moves_to_bit_board(const BitBoard& bit_board)
{
    if (this->follow_moves.empty()) {
        return;
    }

    std::remove_if(this->follow_moves.begin(), this->follow_moves.end(), [&](const Move& follow_move) {
        return !follow_move.compare_follow_moves_to_bit_board(bit_board);
    });

    for (Move& follow_move : this->follow_moves) {
        follow_move.shrink_follow_moves_to_bit_board(bit_board);
    }
}

void shashki::Move::shrink_follow_moves_randomly()
{
    if (this->follow_moves.empty()) {
        return;
    }

    std::forward_list<Move> new_follow_moves;
    std::sample(this->follow_moves.begin(), this->follow_moves.end(), std::front_inserter(new_follow_moves), 1, std::mt19937_64{std::random_device{}()});
    this->follow_moves = new_follow_moves;

    this->follow_moves.front().shrink_follow_moves_randomly();
}

std::string shashki::Move::description() const
{
    std::string description;

    description += (char) 7 - this->moving_piece.position % 8 + 65;
    description += std::to_string(this->moving_piece.position / 8 + 1);
    description += "-";

    if (this->attacked_piece.has_value()) {
        description += (char) 7 - this->attacked_piece.value().position % 8 + 65;
        description += std::to_string(this->attacked_piece.value().position / 8 + 1);
        description += "-";
    }

    description += (char) 7 - this->target_position % 8 + 65;
    description += std::to_string(this->target_position / 8 + 1);

    return description;
}

const shashki::Piece& shashki::Move::get_moving_piece() const
{
    return this->moving_piece;
}

const int& shashki::Move::get_target_position() const
{
    return this->target_position;
}

const std::optional<shashki::Piece>& shashki::Move::get_attacked_piece() const
{
    return this->attacked_piece;
}

const bool& shashki::Move::is_promotion() const
{
    return this->promotion;
}

const shashki::BitBoard& shashki::Move::get_source_bit_board() const
{
    return this->source_bit_board;
}

const shashki::BitBoard& shashki::Move::get_target_bit_board() const
{
    return this->target_bit_board;
}

const std::forward_list<shashki::Move>& shashki::Move::get_follow_moves() const
{
    return this->follow_moves;
}

shashki::Game::Game()
    : bit_board(BitBoard()),
      current_turn(Side::WHITE),
      executed_moves(std::vector<Move>()) {}

bool shashki::Game::operator==(const Game& game) const
{
    return this->bit_board == game.bit_board
        && this->current_turn == game.current_turn
        && this->executed_moves == game.executed_moves;
}

void shashki::Game::execute_move(const Move& move)
{
    this->executed_moves.push_back(move);
    this->executed_moves.back().clear_follow_moves();

    this->bit_board = this->executed_moves.back().get_target_bit_board();

    if (move.get_follow_moves().empty()) {
        this->current_turn = side_opposite(this->current_turn);
    }
}

void shashki::Game::undo_last_move()
{
    if (this->executed_moves.size() < 3) {
        return;
    }

    while (this->current_turn != this->executed_moves.back().get_moving_piece().side) {
        this->executed_moves.pop_back();
    }

    while (this->current_turn == this->executed_moves.back().get_moving_piece().side) {
        this->executed_moves.pop_back();
    }

    this->bit_board = this->executed_moves.back().get_target_bit_board();
}

bool shashki::Game::in_move_combo() const
{
    return !this->executed_moves.empty()
        && this->current_turn == this->executed_moves.back().get_moving_piece().side;
}

shashki::Piece shashki::Game::move_combo_piece() const
{
    const Move& last_move = this->executed_moves.back();
    return Piece(last_move.get_moving_piece().side,
                 last_move.is_promotion() ? PieceType::KING : last_move.get_moving_piece().piece_type,
                 last_move.get_target_position());
}

unsigned long long shashki::Game::capture_bit_board() const
{
    unsigned long long capture_bit_board = 0;

    for (std::vector<Move>::const_reverse_iterator iterator = this->executed_moves.rbegin(); this->current_turn == iterator->get_moving_piece().side; iterator++) {
        capture_bit_board = capture_bit_board | (1ULL << iterator->get_attacked_piece().value().position);
    }

    return capture_bit_board;
}

const shashki::BitBoard& shashki::Game::get_bit_board() const
{
    return this->bit_board;
}

const shashki::Side& shashki::Game::get_current_turn() const
{
    return this->current_turn;
}

const std::vector<shashki::Move>& shashki::Game::get_executed_moves() const
{
    return this->executed_moves;
}
