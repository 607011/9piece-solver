#ifndef __PUZZLE_HPP__
#define __PUZZLE_HPP__

#include <array>
#include <cstdlib>

namespace nine_pieces
{
    using Int = short;
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

        static std::size_t clamped(Int i, Int size);
        static bool will_fit(piece_to_place const &a, piece_to_place const &b);
        std::array<placed_piece, SIZE> const &solution() const;
        placed_piece &solution(int k);
        void place_piece_at(int k, placed_piece const &piece);
        Int idx(Int i) const;
        Int rot(Int i) const;
        bool will_fit(Int k, Int current_piece_idx, Int rot0) const;
    };

}

#endif // __PUZZLE_HPP__
