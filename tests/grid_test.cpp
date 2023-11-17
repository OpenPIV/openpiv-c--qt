
// catch
#include <catch2/catch_test_macros.hpp>

// std
#include <algorithm>
#include <cmath>

// to be tested
#include "core/grid.h"

using namespace openpiv::core;

namespace std {
    template <typename T>
    std::ostream& operator<<(std::ostream& os, const std::vector<T>& ts)
    {
        char separator[] = ", ";
        const char* ps = nullptr;
        for ( const auto& t : ts )
        {
            os << ( ps ? ps : "") << t;
            ps = separator;
        }

        return os;
    }
}


TEST_CASE("grid_test - cartesian grid")
{
    // utils
    using point_t = rect::point_t;
    auto sort_grid = []( auto& grid )
                     {
                         // sort by length of vector and by angle if necessary
                         std::sort( std::begin(grid), std::end(grid),
                                    [](const rect& lhs, const rect& rhs)
                                    {
                                        const auto& lhs_tl = lhs.topLeft();
                                        const auto& rhs_tl = rhs.topLeft();
                                        auto p = [](const auto& _p){ return _p[0]*_p[0] + _p[1]*_p[1]; };
                                        auto a = [](const auto& _p){ return atan2(_p[1], _p[0]); };
                                        if ( p(lhs_tl) == p(rhs_tl) )
                                            return a(lhs_tl) < a(rhs_tl);

                                        return p(lhs_tl) < p(rhs_tl);
                                    } );

                         return grid;
                     };

    size interrogation{ 32, 32 };
    std::vector<rect> expected;

    SECTION("margin")
    {
        auto generated = generate_cartesian_grid( {100, 50}, interrogation, 0.5 );

        for ( const auto& tl :
                  { point_t(2, 1),  point_t(18, 1),  point_t(34, 1),  point_t(50, 1),  point_t(66, 1),
                    point_t(2, 17), point_t(18, 17), point_t(34, 17), point_t(50, 17), point_t(66, 17) } )
            expected.emplace_back( rect(tl, interrogation) );

        INFO( "generated: " << sort_grid(generated) );
        INFO( "expected:  " << sort_grid(expected) );

        CHECK( generated.size() == expected.size() );
        CHECK( (sort_grid( generated ) == sort_grid( expected )) );
    }

    SECTION("exact 1x1")
    {
        auto generated = generate_cartesian_grid( {32, 32}, interrogation, 0.5 );
        expected.emplace_back( rect({0, 0}, interrogation) );

        INFO( "generated: " << sort_grid(generated) );
        INFO( "expected:  " << sort_grid(expected) );

        CHECK( generated.size() == expected.size() );
        CHECK( (sort_grid( generated ) == sort_grid( expected )) );
    }

    SECTION("exact 3x3")
    {
        auto generated = generate_cartesian_grid( {64, 64}, interrogation, 0.5 );

        for ( const auto& tl :
                  { point_t(0, 0),   point_t(16, 0),  point_t(32, 0),
                    point_t(0, 16),  point_t(16, 16), point_t(32, 16),
                    point_t(0, 32),  point_t(16, 32), point_t(32, 32) } )
            expected.emplace_back( rect(tl, interrogation) );

        INFO( "generated: " << sort_grid(generated) );
        INFO( "expected:  " << sort_grid(expected) );

        CHECK( generated.size() == expected.size() );
        CHECK( (sort_grid( generated ) == sort_grid( expected )) );
    }

    SECTION("exact 5x5")
    {
        auto generated = generate_cartesian_grid( {64, 64}, interrogation, 0.25 );

        for ( const auto& tl :
                  { point_t(0, 0),   point_t(8, 0),   point_t(16, 0),   point_t(24, 0),  point_t(32, 0),
                    point_t(0, 8),   point_t(8, 8),   point_t(16, 8),   point_t(24, 8),  point_t(32, 8),
                    point_t(0, 16),  point_t(8, 16),  point_t(16, 16),  point_t(24, 16), point_t(32, 16),
                    point_t(0, 24),  point_t(8, 24),  point_t(16, 24),  point_t(24, 24), point_t(32, 24),
                    point_t(0, 32),  point_t(8, 32),  point_t(16, 32),  point_t(24, 32), point_t(32, 32) } )
            expected.emplace_back( rect(tl, interrogation) );

        INFO( "generated: " << sort_grid(generated) );
        INFO( "expected:  " << sort_grid(expected) );

        CHECK( generated.size() == expected.size() );
        CHECK( (sort_grid( generated ) == sort_grid( expected )) );
    }

    SECTION("3x3, 1 margin")
    {
        auto generated = generate_cartesian_grid( {65, 65}, interrogation, 0.5 );

        for ( const auto& tl :
                  { point_t(0, 0),   point_t(16, 0),  point_t(32, 0),
                    point_t(0, 16),  point_t(16, 16), point_t(32, 16),
                    point_t(0, 32),  point_t(16, 32), point_t(32, 32) } )
            expected.emplace_back( rect(tl, interrogation) );

        INFO( "generated: " << sort_grid(generated) );
        INFO( "expected:  " << sort_grid(expected) );

        CHECK( generated.size() == expected.size() );
        CHECK( (sort_grid( generated ) == sort_grid( expected )) );
    }

    SECTION("3x3, 2 margin")
    {
        auto generated = generate_cartesian_grid( {66, 66}, interrogation, 0.5 );

        for ( const auto& tl :
                  { point_t(1, 1),   point_t(17, 1),  point_t(33, 1),
                    point_t(1, 17),  point_t(17, 17), point_t(33, 17),
                    point_t(1, 33),  point_t(17, 33), point_t(33, 33) } )
            expected.emplace_back( rect(tl, interrogation) );

        INFO( "generated: " << sort_grid(generated) );
        INFO( "expected:  " << sort_grid(expected) );

        CHECK( generated.size() == expected.size() );
        CHECK( (sort_grid( generated ) == sort_grid( expected )) );
    }

    SECTION("exact 3x3, 50% pixel offset")
    {
        auto generated = generate_cartesian_grid( {64, 64}, interrogation, {16, 16} );

        for ( const auto& tl :
                  { point_t(0, 0),   point_t(16, 0),  point_t(32, 0),
                    point_t(0, 16),  point_t(16, 16), point_t(32, 16),
                    point_t(0, 32),  point_t(16, 32), point_t(32, 32) } )
            expected.emplace_back( rect(tl, interrogation) );

        INFO( "generated: " << sort_grid(generated) );
        INFO( "expected:  " << sort_grid(expected) );

        CHECK( generated.size() == expected.size() );
        CHECK( (sort_grid( generated ) == sort_grid( expected )) );
    }

    SECTION("exact 3x5, 50/25% pixel offset")
    {
        auto generated = generate_cartesian_grid( {64, 64}, interrogation, {16, 8} );

        for ( const auto& tl :
                  { point_t(0, 0),   point_t(16, 0),  point_t(32, 0),
                    point_t(0, 8),   point_t(16, 8),  point_t(32, 8),
                    point_t(0, 16),  point_t(16, 16), point_t(32, 16),
                    point_t(0, 24),  point_t(16, 24), point_t(32, 24),
                    point_t(0, 32),  point_t(16, 32), point_t(32, 32) } )
            expected.emplace_back( rect(tl, interrogation) );

        INFO( "generated: " << sort_grid(generated) );
        INFO( "expected:  " << sort_grid(expected) );

        CHECK( generated.size() == expected.size() );
        CHECK( (sort_grid( generated ) == sort_grid( expected )) );
    }

    SECTION("exact 3x7, 50/50% pixel offset, non-square interrogation")
    {
        auto interrogation = size(32, 16);
        auto generated = generate_cartesian_grid(
            {64, 64}, // image size
            interrogation, // interrogation size
            {16, 8}   // offset
            );

        for ( const auto& tl :
                  { point_t(0, 0),   point_t(16, 0),  point_t(32, 0),
                    point_t(0, 8),   point_t(16, 8),  point_t(32, 8),
                    point_t(0, 16),  point_t(16, 16), point_t(32, 16),
                    point_t(0, 24),  point_t(16, 24), point_t(32, 24),
                    point_t(0, 32),  point_t(16, 32), point_t(32, 32),
                    point_t(0, 40),  point_t(16, 40), point_t(32, 40),
                    point_t(0, 48),  point_t(16, 48), point_t(32, 48)
                  } )
            expected.emplace_back( rect(tl, interrogation) );

        INFO( "generated: " << sort_grid(generated) );
        INFO( "expected:  " << sort_grid(expected) );

        CHECK( generated.size() == expected.size() );
        CHECK( (sort_grid( generated ) == sort_grid( expected )) );
    }

}
