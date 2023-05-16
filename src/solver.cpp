#include "solver.hpp"

namespace nine_pieces
{

    void solver::solve(Int k, puzzle const &current_puzzle, std::vector<Int> const &available_pieces)
    {
        ++num_calls_at_level_[k];
        if (k == SIZE)
        {
            // YAY! We're at the deepest level, i.e. we found a solution.
            solutions_.push_back(current_puzzle.solution());
            return;
        }
        for (Int idx = 0; idx < static_cast<Int>(available_pieces.size()); ++idx)
        {
            Int const next_piece_idx = available_pieces[idx];
            for (Int rot = 0; rot < NUM_ORIENTATIONS; ++rot)
            {
                if (current_puzzle.will_fit(k, next_piece_idx, rot))
                {
                    puzzle next_puzzle{current_puzzle};
                    next_puzzle.place_piece_at(k, {next_piece_idx, rot});
                    std::vector<Int> remaining_pieces{available_pieces};
                    remaining_pieces.erase(remaining_pieces.begin() + idx);
                    solve(k + 1, next_puzzle, remaining_pieces);
                }
                if (k == 0)
                {
                    break;
                }
            }
        }
    }

    unsigned int solver::total_tries() const
    {
        return std::accumulate(num_calls_at_level_.cbegin(), num_calls_at_level_.cend(), 0);
    }

    std::array<unsigned int, SIZE + 1> solver::tries_at_level() const
    {
        return num_calls_at_level_;
    }

    std::vector<std::array<placed_piece, SIZE>> solver::solutions() const
    {
        return solutions_;
    }

    void solver::solve()
    {
        std::vector<Int> all_pieces{0, 1, 2, 3, 4, 5, 6, 7, 8};
        solutions_.clear();
        num_calls_at_level_.fill(0);
        solve(0, pieces_, all_pieces);
    }

}
