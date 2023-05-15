#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <numeric>
#include <vector>
#include <iostream>

#include <getopt.hpp>

#include "util.hpp"

namespace nine_pieces
{
    static constexpr int SIZE = 3 * 3;
    static constexpr int NUM_ORIENTATIONS = 4;

    using piece_t = std::array<int, 4>;
    using puzzle_t = std::array<piece_t, SIZE>;

    struct solution_t
    {
        int order;
        int rot;
    };

    class puzzle
    {
        puzzle_t pieces_;
        std::array<solution_t, 9> solution_;

    public:
        explicit puzzle(puzzle_t const &pieces)
            : pieces_(pieces)
        {
        }

        inline static bool will_fit(piece_t const &a, int rot_a, int side_a, piece_t const &b, int rot_b, int side_b)
        {
            const auto av = a.at(static_cast<std::size_t>((4 + side_a - rot_a) % 4));
            const auto bv = b.at(static_cast<std::size_t>((4 + side_b - rot_b) % 4));
            return (av + bv) == 0;
        }

        std::array<solution_t, 9> const &solution() const
        {
            return solution_;
        }

        solution_t &solution(int k)
        {
            return solution_[k];
        }

        int order(int i) const
        {
            return solution_.at(static_cast<std::size_t>((SIZE + i) % SIZE)).order;
        }

        int rot(int i) const
        {
            return solution_.at(static_cast<std::size_t>((SIZE + i) % SIZE)).rot;
        }

        bool will_fit(int k, int current_piece_idx, int current_rotation) const
        {
            if (k == 0)
            {
                return true;
            }
            piece_t const &piece0 = pieces_.at(current_piece_idx);
            piece_t const &piece1 = pieces_.at(order(k - 1));
            static const std::array<int, SIZE> side_map = {1, 3, 0, 1, 1, 2, 2, 3, 3};
            int side0 = side_map.at(k);
            int side1 = (4 + side0 - 2) % 4;
            bool fits = will_fit(piece0, current_rotation, side0, piece1, rot(k - 1), side1);
            switch (k)
            {
            case 3:
                return fits & will_fit(pieces_.at(current_piece_idx), current_rotation, 0, pieces_.at(order(0)), rot(0), 2);
            case 5:
                return fits & will_fit(pieces_.at(current_piece_idx), current_rotation, 1, pieces_.at(order(0)), rot(0), 3);
            case 7:
                return fits & will_fit(pieces_.at(current_piece_idx), current_rotation, 2, pieces_.at(order(0)), rot(0), 0);
            case 8:
                return fits & will_fit(pieces_.at(current_piece_idx), current_rotation, 2, pieces_.at(order(1)), rot(1), 0);
            default:
                break;
            }
            return fits;
        }
    };

    class solver
    {
        puzzle pieces_;
        std::vector<std::array<solution_t, 9>> solutions_;
        std::array<int, SIZE + 1> num_calls_at_level;

    public:
        solver(puzzle const &pieces)
            : pieces_(pieces)
        {
        }

        void solve(int k, puzzle const &current_puzzle, std::vector<int> const &available_pieces)
        {
            ++num_calls_at_level[k];
            if (k == SIZE)
            {
                solutions_.push_back(current_puzzle.solution());
                return;
            }
            for (int idx = 0; idx < static_cast<int>(available_pieces.size()); ++idx)
            {
                int next_piece_idx = available_pieces[idx];
                for (int rot = 0; rot < NUM_ORIENTATIONS; ++rot)
                {
                    if (current_puzzle.will_fit(k, next_piece_idx, rot))
                    {
                        puzzle next_puzzle{current_puzzle};
                        next_puzzle.solution(k).order = next_piece_idx;
                        next_puzzle.solution(k).rot = rot;
#if 0
                        std::vector<int> remaining_pieces{available_pieces};
                        remaining_pieces.erase(remaining_pieces.begin() + idx);
#else
                        std::vector<int> remaining_pieces(available_pieces.size() - 1);
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

        std::vector<std::array<solution_t, 9>> solutions() const
        {
            return solutions_;
        }

        void solve()
        {
            std::vector<int> stack{0, 1, 2, 3, 4, 5, 6, 7, 8};
            solutions_.clear();
            num_calls_at_level.fill(0);
            solve(0, pieces_, stack);
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
            << s.at(6).order << ' ' << s.at(7).order << ' ' << s.at(8).order << " | "
            << s.at(6).rot << ' ' << s.at(7).rot << ' ' << s.at(8).rot << '\n'
            << s.at(5).order << ' ' << s.at(0).order << ' ' << s.at(1).order << " | "
            << s.at(5).rot << ' ' << s.at(0).rot << ' ' << s.at(1).rot << '\n'
            << s.at(4).order << ' ' << s.at(3).order << ' ' << s.at(2).order << " | "
            << s.at(4).rot << ' ' << s.at(3).rot << ' ' << s.at(2).rot << "\n\n";
    }
    std::cout << "Total tries: " << s.num_tries() << '\n';
    std::cout << dt.count() << " µs\n";
    return 0;
}