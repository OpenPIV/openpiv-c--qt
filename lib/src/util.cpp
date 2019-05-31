
#include "util.h"

// std
#include <cmath>
#include <limits>
#include <istream>
#include <numeric>
#include <optional>
#include <sstream>
#include <thread>
#include <typeinfo>
#include <type_traits>
#include <utility>
#include <vector>

// core
#include "exception_builder.h"

namespace openpiv::core {

//
//
thread_local uint8_t entry_exit_logger::indent = 0;

entry_exit_logger::entry_exit_logger( std::ostream& os, const std::string& s )
    : os_( os )
    , s_( s )
    , id_( std::this_thread::get_id() )
{
    ++indent;
    os_ << std::string( indent, '>' ) << " (" << id_ << ") " << s_ << "\n";
}

entry_exit_logger::~entry_exit_logger()
{
    os_ << std::string( indent, '<' ) << " (" << id_ << ") " << s_ << "\n";
    --indent;
}

/// simple RAII for std::istream multi-char peek; allows caller
/// to "peek" at multiple bytes and will restore stream upon destruction:
///
///
peeker::peeker( std::istream& is )
    : is_( is )
{
    pos_ = is.tellg();
    // this will only work if we can read and rewind
    if ( pos_ == -1 || !is.good() )
        exception_builder<std::runtime_error>() << "input stream doesn't support input position indicator";
}

peeker::~peeker()
{
    is_.seekg(pos_);
}


}
