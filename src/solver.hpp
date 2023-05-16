#include <algorithm>
#include <array>
#include <cstdlib>
#include <chrono>
#include <fstream>
#include <numeric>
#include <stdexcept>
#include <vector>
#include <iostream>

#include "util.hpp"

namespace nine_pieces
{
    using Int = int;
    typedef Int value_type;

    static constexpr Int SIZE = 3 * 3;
    static constexpr Int NUM_ORIENTATIONS = 4;

    using piece_t = std::array<Int, 4>;
    using puzzle_t = std::array<piece_t, SIZE>;

    struct placed_piece
    {
        Int idx;
        Int rot;
    };

    struct piece_to_place
    {
        piece_to_place() = delete;
        piece_to_place(piece_to_place const &) = delete;
        piece_to_place(piece_to_place &&) = delete;
        piece_t const &piece;
        Int rot;
        Int edge;
    };

    class puzzle
    {
        puzzle_t pieces_;
        std::array<placed_piece, 9> solution_;

        enum
        {
            TOP = 0,
            RIGHT = 1,
            BOTTOM = 2,
            LEFT = 3
        };

    public:
        explicit puzzle(puzzle_t const &pieces)
            : pieces_(pieces)
        {
        }

        inline static std::size_t normalized(Int i, Int size)
        {
            return static_cast<std::size_t>((size + i) % size);
        }

        inline static bool will_fit(piece_to_place const &a, piece_to_place const &b)
        {
            Int av = a.piece.at(normalized(a.edge - a.rot, 4));
            Int bv = b.piece.at(normalized(b.edge - b.rot, 4));
            return (av + bv) == 0;
        }

        std::array<placed_piece, SIZE> const &solution() const
        {
            return solution_;
        }

        placed_piece &solution(int k)
        {
            return solution_[k];
        }

        Int idx(Int i) const
        {
            return solution_.at(normalized(i, SIZE)).idx;
        }

        Int rot(Int i) const
        {
            return solution_.at(normalized(i, SIZE)).rot;
        }

        bool will_fit(Int k, Int current_piece_idx, Int rot0) const
        {
            if (k == 0)
            {
                return true;
            }
            piece_t const &piece0 = pieces_.at(current_piece_idx);
            piece_t const &piece1 = pieces_.at(idx(k - 1)); // get previous piece
            int rot1 = rot(k - 1);                          // get rotation of previous piece
            static const std::array<Int, SIZE> Directions = {RIGHT, LEFT, TOP, RIGHT, RIGHT, BOTTOM, BOTTOM, LEFT, LEFT};
            Int edge_a = Directions.at(k);
            Int edge_b = (4 + edge_a - 2) % 4;
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
    };

    class solver
    {
        puzzle pieces_;
        std::vector<std::array<placed_piece, 9>> solutions_;
        std::array<unsigned int, SIZE + 1> num_calls_at_level;

    public:
        solver(puzzle const &pieces)
            : pieces_(pieces)
        {
        }

        void solve(Int depth, puzzle const &current_puzzle, std::vector<Int> const &available_pieces)
        {
            ++num_calls_at_level[depth];
            if (depth == SIZE)
            {
                solutions_.push_back(current_puzzle.solution());
                return;
            }
            for (Int idx = 0; idx < static_cast<Int>(available_pieces.size()); ++idx)
            {
                Int next_piece_idx = available_pieces[idx];
                for (Int rot = 0; rot < NUM_ORIENTATIONS; ++rot)
                {
                    if (current_puzzle.will_fit(depth, next_piece_idx, rot))
                    {
                        puzzle next_puzzle{current_puzzle};
                        next_puzzle.solution(depth).idx = next_piece_idx;
                        next_puzzle.solution(depth).rot = rot;
#if 0
                        std::vector<Int> remaining_pieces{available_pieces};
                        remaining_pieces.erase(remaining_pieces.begin() + idx);
#else
                        std::vector<Int> remaining_pieces(available_pieces.size() - 1);
                        std::remove_copy_if(available_pieces.cbegin(), available_pieces.cend(), remaining_pieces.begin(), [next_piece_idx](int i)
                                            { return i == next_piece_idx; });
#endif
                        solve(depth + 1, next_puzzle, remaining_pieces);
                    }
                    if (depth == 0)
                    {
                        break;
                    }
                }
            }
        }

        int total_tries() const
        {
            return std::accumulate(num_calls_at_level.cbegin(), num_calls_at_level.cend(), 0);
        }

        std::vector<std::array<placed_piece, 9>> solutions() const
        {
            return solutions_;
        }

        void solve()
        {
            std::vector<Int> all_pieces{0, 1, 2, 3, 4, 5, 6, 7, 8};
            solutions_.clear();
            num_calls_at_level.fill(0);
            solve(0, pieces_, all_pieces);
        }
    };

}
