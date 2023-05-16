#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "solver.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "\u001b[31;1mERROR: No file name given.\n";
        return EXIT_FAILURE;
    }
    nine_pieces::puzzle_t pieces;
    std::size_t piece_idx = 0;
    std::ifstream fin(argv[1]);
    std::string line;
    while (std::getline(fin, line))
    {
        auto const &pieces_str = util::split(line, ' ');
        if (pieces_str.size() != 4)
        {
            std::cerr << "\u001b[31;1mERROR: each piece must have 4 edges (line " << (piece_idx + 1) << ").\n";
            return EXIT_FAILURE;
        }
        nine_pieces::piece_t piece;
        try
        {
            std::transform(pieces_str.cbegin(), pieces_str.cend(), piece.begin(),
                           [](std::string const &value) -> nine_pieces::value_type
                           {
                               return static_cast<nine_pieces::value_type>(std::stoi(value));
                           });
        }
        catch (std::invalid_argument const &e)
        {
            std::cerr << "\u001b[31;1mERROR: " << e.what() << '\n';
            return EXIT_FAILURE;
        }
        catch (std::out_of_range const &e)
        {
            std::cerr << "\u001b[31;1mERROR: " << e.what() << '\n';
            return EXIT_FAILURE;
        }
        if (std::any_of(piece.begin(), piece.end(), [](int val) {
            return val == 0;
        })) {
            std::cerr << "\u001b[31;1mERROR: Edge must not be 0 (line " << (piece_idx + 1) << ").\n";
            return EXIT_FAILURE;
        }
        pieces[piece_idx++] = piece;
    }
    if (piece_idx < 9)
    {
        std::cerr << "ERROR: Not enough pieces. Must be 9.\n";
        return EXIT_FAILURE;
    }
    if (piece_idx > 9)
    {
        std::cerr << "ERROR: Too many pieces. Must be 9.\n";
        return EXIT_FAILURE;
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
    std::cout << "Total tries: " << s.total_tries() << '\n';
    std::cout << dt.count() << " µs\n";
    return EXIT_SUCCESS;
}