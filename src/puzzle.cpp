#include "puzzle.hpp"

namespace nine_pieces
{
    bool puzzle::will_edge_fit(piece_to_place const &a, piece_to_place const &b)
    {
        Int av = a.piece.at((a.edge + a.rot) % 4);
        Int bv = b.piece.at((b.edge + b.rot) % 4);
        return (av + bv) == 0;
    }

    std::array<placed_piece, SIZE> const &puzzle::solution() const
    {
        return solution_;
    }

    void puzzle::place_piece_at(int k, placed_piece const &piece)
    {
        solution_[k] = piece;
    }

    Int puzzle::idx(Int i) const
    {
        return solution_.at(i).idx;
    }

    Int puzzle::rot(Int i) const
    {
        return solution_.at(i).rot;
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
        constexpr std::array<Int, SIZE> Directions{
            RIGHT, LEFT, TOP, RIGHT, RIGHT, BOTTOM, BOTTOM, LEFT, LEFT};
        Int edge0 = Directions.at(k);
        Int edge1 = (NUM_EDGES + edge0 - 2) % NUM_EDGES;
        bool fits = will_edge_fit(
            {piece0, rot0, edge0},
            {piece1, rot1, edge1});
        switch (k)
        {
        case 3:
            return fits && will_edge_fit(
                               {pieces_.at(current_piece_idx), rot0, TOP},
                               {pieces_.at(idx(0)), rot(0), BOTTOM});
        case 5:
            return fits && will_edge_fit(
                               {pieces_.at(current_piece_idx), rot0, RIGHT},
                               {pieces_.at(idx(0)), rot(0), LEFT});
        case 7:
            return fits && will_edge_fit(
                               {pieces_.at(current_piece_idx), rot0, BOTTOM},
                               {pieces_.at(idx(0)), rot(0), TOP});
        case 8:
            return fits && will_edge_fit(
                               {pieces_.at(current_piece_idx), rot0, BOTTOM},
                               {pieces_.at(idx(1)), rot(1), TOP});
        default:
            break;
        }
        return fits;
    }
}
