#include <algorithm>
#include <array>
#include <cstdlib>
#include <numeric>
#include <vector>

#include "util.hpp"

namespace nine_pieces
{
    using Int = int;
    typedef Int value_type;

    /// @brief The puzzle is a square matrix with 3 by 3 pieces.
    static constexpr Int SIZE = 3 * 3;
    /// @brief There are four possible orientations for each piece.
    static constexpr Int NUM_ORIENTATIONS = 4;
    /// @brief Each piece has four edges.
    static constexpr Int NUM_EDGES = 4;

    using piece_t = std::array<Int, NUM_EDGES>;
    using puzzle_t = std::array<piece_t, SIZE>;

    /// @brief The data of a successfully placed piece.
    struct placed_piece
    {
        /// @brief The piece's index into the array with information about every piece.
        Int idx;
        /// @brief The correct orientation of the piece.
        Int rot;
    };

    /**
     * @brief When the solver tries to place a piece this data is needed to check if the piece will fit.
     * @see `puzzle::will_fit()`
     */
    struct piece_to_place
    {
        piece_to_place() = delete;
        piece_to_place(piece_to_place const &) = delete;
        piece_to_place(piece_to_place &&) = delete;
        piece_to_place &operator=(piece_to_place const &) = delete;
        /// @brief The edge data of the original piece
        piece_t const &piece;
        /// @brief Will it fit with this orientation?
        Int rot;
        /// @brief Will this edge fit?
        Int edge;
    };

    /**
     * @brief This class represents a 3x3 Scramble Square™️ puzzle.
     */
    class puzzle
    {
        /**
         * @brief An array with the 9 pieces of the puzzle.
         * 
         * This array represents the raw puzzle pieces. Neither the
         * order nor the correct orientation of the puzzle is known.
         */
        puzzle_t pieces_;

        /**
         * @brief This array will contain the solution of the puzzle if one could be found.
         * 
         * For each piece in the original array `pieces_` this array
         * contains its index and correct orientation.
         */
        std::array<placed_piece, 9> solution_;

        /**
         * @brief Names and values for the four edges of each piece.
         * 
         */
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

        /// @brief C++ cannot handle negative indexes into arrays. This 
        /// @param i 
        /// @param size 
        /// @return true is piece will fit, false otherwise 
        inline static std::size_t clamped(Int i, Int size)
        {
            return static_cast<std::size_t>((size + i) % size);
        }

        inline static bool will_fit(piece_to_place const &a, piece_to_place const &b)
        {
            Int av = a.piece.at(clamped(a.edge - a.rot, 4));
            Int bv = b.piece.at(clamped(b.edge - b.rot, 4));
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
            return solution_.at(clamped(i, SIZE)).idx;
        }

        Int rot(Int i) const
        {
            return solution_.at(clamped(i, SIZE)).rot;
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
    };


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

        void solve(Int k, puzzle const &current_puzzle, std::vector<Int> const &available_pieces)
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
                        next_puzzle.solution(k).idx = next_piece_idx;
                        next_puzzle.solution(k).rot = rot;
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

        unsigned int total_tries() const
        {
            return std::accumulate(num_calls_at_level_.cbegin(), num_calls_at_level_.cend(), 0);
        }

        std::array<unsigned int, SIZE + 1> tries_at_level() const
        {
            return num_calls_at_level_;
        }

        std::vector<std::array<placed_piece, SIZE>> solutions() const
        {
            return solutions_;
        }

        void solve()
        {
            std::vector<Int> all_pieces{0, 1, 2, 3, 4, 5, 6, 7, 8};
            solutions_.clear();
            num_calls_at_level_.fill(0);
            solve(0, pieces_, all_pieces);
        }
    };

}
