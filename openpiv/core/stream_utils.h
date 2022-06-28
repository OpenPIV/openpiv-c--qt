
#pragma once

// std
#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <vector>

namespace openpiv::core {

    template <typename T,
              typename A,
              template<typename, typename> class C,
              typename = std::enable_if_t<
                  std::is_same_v<C<T, A>, std::list<T, A>> ||
                  std::is_same_v<C<T, A>, std::vector<T, A>>>>
    std::string join(const C<T, A>& container, std::string separator = ", ")
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
              typename A,
              template<typename, typename> class C,
              typename = std::enable_if_t<
                  std::is_same_v<C<T, A>, std::list<T, A>> ||
                  std::is_same_v<C<T, A>, std::vector<T, A>>>>
    ostream& operator<<(ostream& os, const C<T, A>& container)
    {
        return os << openpiv::core::join(container, ", ");
    }
}
