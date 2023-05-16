#include "puzzle.hpp"

namespace nine_pieces
{
    std::size_t puzzle::clamped(Int i, Int size)
    {
        return static_cast<std::size_t>((size + i) % size);
    }

    bool puzzle::will_fit(piece_to_place const &a, piece_to_place const &b)
    {
        Int av = a.piece.at(clamped(a.edge - a.rot, 4));
        Int bv = b.piece.at(clamped(b.edge - b.rot, 4));
        return (av + bv) == 0;
    }

    std::array<placed_piece, SIZE> const &puzzle::solution() const
    {
        return solution_;
    }

    placed_piece &puzzle::solution(int k)
    {
        return solution_[k];
    }

    void puzzle::place_piece_at(int k, placed_piece const &piece)
    {
        solution_[k] = piece;
    }

    Int puzzle::idx(Int i) const
    {
        return solution_.at(clamped(i, SIZE)).idx;
    }

    Int puzzle::rot(Int i) const
    {
        return solution_.at(clamped(i, SIZE)).rot;
    }

    bool puzzle::will_fit(Int k, Int current_piece_idx, Int rot0) const
    {
        if (k == 0)
        {
            return true;
        }
        piece_t const &piece0 = pieces_.at(current_piece_idx);
        piece_t const &piece1 = pieces_.at(idx(k - 1)); // get previous piece
        Int rot1 = rot(k - 1);                          // get rotation of previous piece
        static const std::array<Int, SIZE> Directions = {RIGHT, LEFT, TOP, RIGHT, RIGHT, BOTTOM, BOTTOM, LEFT, LEFT};
        Int edge_a = Directions.at(k);
        Int edge_b = (NUM_EDGES + edge_a - 2) % NUM_EDGES;
        bool fits = will_fit(
            {piece0, rot0, edge_a},
            {piece1, rot1, edge_b});
        switch (k)
        {
        case 3:
            return fits && will_fit(
                               {pieces_.at(current_piece_idx), rot0, TOP},
                               {pieces_.at(idx(0)), rot(0), BOTTOM});
        case 5:
            return fits && will_fit(
                               {pieces_.at(current_piece_idx), rot0, RIGHT},
                               {pieces_.at(idx(0)), rot(0), LEFT});
        case 7:
            return fits && will_fit(
                               {pieces_.at(current_piece_idx), rot0, BOTTOM},
                               {pieces_.at(idx(0)), rot(0), TOP});
        case 8:
            return fits && will_fit(
                               {pieces_.at(current_piece_idx), rot0, BOTTOM},
                               {pieces_.at(idx(1)), rot(1), TOP});
        default:
            break;
        }
        return fits;
    }
}
