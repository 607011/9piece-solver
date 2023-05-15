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

    struct puzzle
    {
        puzzle_t pieces;
        std::array<solution_t, 9> solution;

        explicit puzzle(puzzle_t const &pieces)
            : pieces(pieces)
        {
        }

        inline static bool is_fit(piece_t const &a, int rot_a, int side_a, piece_t const &b, int rot_b, int side_b)
        {
            const auto av = a.at(static_cast<std::size_t>((4 + side_a - rot_a) % 4));
            const auto bv = b.at(static_cast<std::size_t>((4 + side_b - rot_b) % 4));
            return (av + bv) == 0;
        }

        int order(int i) const
        {
            return solution.at(static_cast<std::size_t>((SIZE + i) % SIZE)).order;
        }

        int rot(int i) const
        {
            return solution.at(static_cast<std::size_t>((SIZE + i) % SIZE)).rot;
        }

        bool is_fit(int k, int used_k, int rot_k) const
        {
            if (k == 0)
            {
                return true;
            }
            auto const &piece_k = pieces.at(used_k);
            auto const &piece_j = pieces.at(order(k - 1));
            static const std::array<int, SIZE> side_map = {1, 3, 0, 1, 1, 2, 2, 3, 3};
            static const std::array<int, SIZE> side_map_opposite = {3, 1, 2, 3, 3, 0, 0, 1, 1};
            int side_k = side_map.at(k);
            int side_j = side_map_opposite.at(k);
            bool fits = is_fit(piece_k, rot_k, side_k, piece_j, rot(k - 1), side_j);
            switch (k)
            {
            case 3:
                return fits & is_fit(pieces.at(used_k), rot_k, 0, pieces.at(order(0)), rot(0), 2);
            case 5:
                return fits & is_fit(pieces.at(used_k), rot_k, 1, pieces.at(order(0)), rot(0), 3);
            case 7:
                return fits & is_fit(pieces.at(used_k), rot_k, 2, pieces.at(order(0)), rot(0), 0);
            case 8:
                return fits & is_fit(pieces.at(used_k), rot_k, 2, pieces.at(order(1)), rot(1), 0);
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

    public:
        std::array<int, SIZE + 1> num_calls_at_level;

        solver(puzzle const &pieces)
            : pieces_(pieces)
        {
        }

        void solve(int k, puzzle const &current_puzzle, std::vector<int> const &stack)
        {
            ++num_calls_at_level[k];
            if (k == SIZE)
            {
                solutions_.push_back(current_puzzle.solution);
                return;
            }
            for (int idx = 0; idx < static_cast<int>(stack.size()); ++idx)
            {
                int next_piece_idx = stack[idx];
                for (int rot = 0; rot < NUM_ORIENTATIONS; ++rot)
                {
                    if (current_puzzle.is_fit(k, next_piece_idx, rot))
                    {
                        puzzle next_puzzle{current_puzzle};
                        next_puzzle.solution[k].order = next_piece_idx;
                        next_puzzle.solution[k].rot = rot;
#if 0
                        std::vector<int> next_stack{stack};
                        next_stack.erase(next_stack.begin() + idx);
#else
                        std::vector<int> next_stack(stack.size() - 1);
                        std::remove_copy_if(stack.begin(), stack.end(), next_stack.begin(), [next_piece_idx](int i)
                                            { return i == next_piece_idx; });
#endif
                        solve(k + 1, next_puzzle, next_stack);
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
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
    if (piece_idx != 9)
    {
        std::cerr << "ERROR: Invalid data." << std::endl;
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