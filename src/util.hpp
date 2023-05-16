#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace util
{

    std::vector<std::string> split(const std::string &str, char delim);

    template <typename InputIteratorT, typename SeparatorT>
    std::string join(InputIteratorT input, SeparatorT separator)
    {
        std::ostringstream result;
        auto i = std::begin(input);
        if (i != std::end(input))
        {
            result << *i++;
        }
        while (i != std::end(input))
        {
            result << separator << *i++;
        }
        return result.str();
    }
}

#endif // __UTIL___UTIL_HPP__PP__
