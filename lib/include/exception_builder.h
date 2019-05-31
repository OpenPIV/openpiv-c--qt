
#pragma once

// std
#include <sstream>

namespace openpiv::core {

    /// wrapper to allow using stringstream to construct an
    /// exception message; throw \ta E on destruction.
    template < typename E >
    class exception_builder
    {
    public:
        exception_builder() = default;
        ~exception_builder() noexcept(false)
        {
            throw E(ss.str());
        }

        template < typename T >
        std::stringstream& operator<<( const T& v )
        {
            ss << v;
            return ss;
        }

        std::stringstream ss;
    };

}
