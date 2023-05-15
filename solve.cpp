#include <algorithm>
#include <array>
#include <cstdlib>
#include <chrono>
#include <fstream>
#include <numeric>
#include <vector>
#include <iostream>

#include <getopt.hpp>

#include "util.hpp"

namespace nine_pieces
{
    using Int = short;

    static constexpr Int SIZE = 3 * 3;
    static constexpr Int NUM_ORIENTATIONS = 4;

    using piece_t = std::array<Int, 4>;
    using puzzle_t = std::array<piece_t, SIZE>;

    struct piece_data
    {
        Int idx;
        Int rot;
    };

    class puzzle
    {
        puzzle_t pieces_;
        std::array<piece_data, 9> solution_;

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

        inline static std::size_t normalized(int i, int size)
        {
            return static_cast<std::size_t>((size + i) % size);
        }

        inline static bool will_fit(piece_t const &a, Int rot_a, Int edge_a, piece_t const &b, Int rot_b, Int edge_b)
        {
            const auto av = a.at(normalized(edge_a - rot_a, 4));
            const auto bv = b.at(normalized(edge_b - rot_b, 4));
            return (av + bv) == 0;
        }

        std::array<piece_data, SIZE> const &solution() const
        {
            return solution_;
        }

        piece_data &solution(int k)
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

        bool will_fit(int k, int current_piece_idx, int current_rotation) const
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
            bool fits = will_fit(piece0, current_rotation, edge_a, piece1, rot1, edge_b);
            switch (k)
            {
            case 3:
                return fits & will_fit(pieces_.at(current_piece_idx), current_rotation, TOP, pieces_.at(idx(0)), rot(0), BOTTOM);
            case 5:
                return fits & will_fit(pieces_.at(current_piece_idx), current_rotation, RIGHT, pieces_.at(idx(0)), rot(0), LEFT);
            case 7:
                return fits & will_fit(pieces_.at(current_piece_idx), current_rotation, BOTTOM, pieces_.at(idx(0)), rot(0), TOP);
            case 8:
                return fits & will_fit(pieces_.at(current_piece_idx), current_rotation, BOTTOM, pieces_.at(idx(1)), rot(1), TOP);
            default:
                break;
            }
            return fits;
        }
    };

    class solver
    {
        puzzle pieces_;
        std::vector<std::array<piece_data, 9>> solutions_;
        std::array<unsigned int, SIZE + 1> num_calls_at_level;

    public:
        solver(puzzle const &pieces)
            : pieces_(pieces)
        {
        }

        void solve(Int k, puzzle const &current_puzzle, std::vector<Int> const &available_pieces)
        {
            ++num_calls_at_level[k];
            if (k == SIZE)
            {
                solutions_.push_back(current_puzzle.solution());
                return;
            }
            for (Int idx = 0; idx < static_cast<Int>(available_pieces.size()); ++idx)
            {
                Int next_piece_idx = available_pieces[idx];
                for (Int rot = 0; rot < NUM_ORIENTATIONS; ++rot)
                {
                    if (current_puzzle.will_fit(k, next_piece_idx, rot))
                    {
                        puzzle next_puzzle{current_puzzle};
                        next_puzzle.solution(k).idx = next_piece_idx;
                        next_puzzle.solution(k).rot = rot;
#if 0
                        std::vector<Int> remaining_pieces{available_pieces};
                        remaining_pieces.erase(remaining_pieces.begin() + idx);
#else
                        std::vector<Int> remaining_pieces(available_pieces.size() - 1);
                        std::remove_copy_if(available_pieces.cbegin(), available_pieces.cend(), remaining_pieces.begin(), [next_piece_idx](int i)
                                            { return i == next_piece_idx; });
#endif
                        solve(k + 1, next_puzzle, remaining_pieces);
                    }
                    if (k == 0)
                    {
                        break;
                    }
                }
            }
        }

        int num_tries() const
        {
            return std::accumulate(num_calls_at_level.cbegin(), num_calls_at_level.cend(), 0);
        }

        std::vector<std::array<piece_data, 9>> solutions() const
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

int main(int argc, char *argv[])
{
    using argparser = argparser::argparser;
    argparser opt(argc, argv);
    nine_pieces::puzzle_t pieces;
    std::size_t piece_idx = 0;
    opt.pos([&piece_idx, &pieces](std::string const &arg)
            {
        std::ifstream fin(arg);
        std::string line;
        while (std::getline(fin, line))
        {
            auto const &pieces_str = util::split(line, ' ');
            if (pieces_str.size() != 4)
            {
                std::cerr << "\u001b[31;1mERROR: each piece must have 4 sides.\n";
                exit(1);
            }
            nine_pieces::piece_t piece;
            std::transform(pieces_str.cbegin(), pieces_str.cend(), piece.begin(),
                [](std::string const &value) -> int {
                    return std::stoi(value);
                });
            pieces[piece_idx++] = piece;
        } });
    try
    {
        opt();
    }
    catch (::argparser::argument_required_exception e)
    {
        std::cerr << "ERROR: " << e.what() << '\n';
    }
    if (piece_idx < 9)
    {
        std::cerr << "ERROR: Not enough pieces. Must be 9.\n";
        exit(1);
    }
    if (piece_idx > 9)
    {
        std::cerr << "ERROR: Too many pieces. Must be 9.\n";
        exit(1);
    }
    namespace chrono = std::chrono;
    auto t0 = chrono::high_resolution_clock().now();
    nine_pieces::puzzle p{pieces};
    nine_pieces::solver s{p};
    s.solve();
    auto t1 = chrono::high_resolution_clock().now();
    auto dt = chrono::duration_cast<chrono::microseconds>(t1 - t0);
    int num_solutions = 0;
    for (auto const &s : s.solutions())
    {
        std::cout
            << "Solution " << (++num_solutions) << " (piece index | rotation):\n"
            << (int)s.at(6).idx << ' ' << (int)s.at(7).idx << ' ' << (int)s.at(8).idx << " | "
            << (int)s.at(6).rot << ' ' << (int)s.at(7).rot << ' ' << (int)s.at(8).rot << '\n'
            << (int)s.at(5).idx << ' ' << (int)s.at(0).idx << ' ' << (int)s.at(1).idx << " | "
            << (int)s.at(5).rot << ' ' << (int)s.at(0).rot << ' ' << (int)s.at(1).rot << '\n'
            << (int)s.at(4).idx << ' ' << (int)s.at(3).idx << ' ' << (int)s.at(2).idx << " | "
            << (int)s.at(4).rot << ' ' << (int)s.at(3).rot << ' ' << (int)s.at(2).rot << "\n\n";
    }
    std::cout << "Total tries: " << s.num_tries() << '\n';
    std::cout << dt.count() << " µs\n";
    return 0;
}