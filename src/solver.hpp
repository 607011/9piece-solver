#ifndef __SOLVER_HPP__
#define __SOLVER_HPP__

#include <algorithm>
#include <array>
#include <cstdlib>
#include <numeric>
#include <vector>

#include "util.hpp"
#include "puzzle.hpp"

namespace nine_pieces
{
    /**
     * @brief Solver for any 3x3 Scramble Square™️ puzzle.
     * 
     * The solver places the first piece in the center because it doesn't
     * need to be rotated; rotating it will rotate the entire puzzle.
     * The next piece is placed to the right of the first, the following
     * in a clockwise spiral around the center.
     * For every piece all of the four possible orientations are tested.
     * If it fits the solver goes a level deeper and the process
     * repeats with the remaining pieces until all pieces are placed.
     * If it doesn't fit the next available piece is chosen.
     */
    class solver
    {
        puzzle pieces_;
        std::vector<std::array<placed_piece, 9>> solutions_;
        std::array<unsigned int, SIZE + 1> num_calls_at_level_;

    public:
        solver(puzzle const &pieces)
            : pieces_(pieces)
        {
        }

        void solve(Int k, puzzle const &current_puzzle, std::vector<Int> const &available_pieces);
        unsigned int total_tries() const;
        std::array<unsigned int, SIZE + 1> tries_at_level() const;
        std::vector<std::array<placed_piece, SIZE>> solutions() const;
        void solve();
    };

}

#endif // __SOLVER_HPP__
