
#pragma once

// std
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

namespace openpiv::core {

    template <typename T,
              template<typename> class C,
              typename = std::enable_if_t<
                  std::is_same_v<C<T>, std::list<T>> ||
                  std::is_same_v<C<T>, std::vector<T>>>>
    std::string join(const C<T>& container, std::string separator)
    {
        std::stringstream result;
        result << "[";
        const char* ps = nullptr;
        for ( const auto& t : container )
        {
            result << ( ps ? ps : "") << t;
            ps = separator.data();
        }
        result << "]";

        return result.str();
    }

}

namespace std {
    template <typename T,
              template<typename> class C,
              typename = std::enable_if_t<
                  std::is_same_v<C<T>, std::list<T>> ||
                  std::is_same_v<C<T>, std::vector<T>>>>
    ostream& operator<<(ostream& os, const C<T>& ts)
    {
        return os << openpiv::core::join(ts, ", ");
    }
}
