#include <catch2/catch_test_macros.hpp>
#include "vec3.h"

//     float myCalcOfPi = 3.141f;

//     float eps = 1.0e-3; // a tolerance for how close the answer needs to be
//     float actualAnswer = 3.14159f;
//     REQUIRE_THAT( myCalcOfPi, Catch::Matchers::WithinAbs(actualAnswer, eps));
// }
TEST_CASE( "Vector addition" )
{
    vec3 a(1, 2, 3);
    vec3 b(4, 5, 6);
    vec3 c = a + b;
    REQUIRE( c.x() == 5 );
    REQUIRE( c.y() == 7 );
    REQUIRE( c.z() == 9 );
}

TEST_CASE( "Vector subtraction" )
{
    vec3 a(1, 2, 3);
    vec3 b(4, 5, 6);
    vec3 c = a - b;
    REQUIRE( c.x() == -3 );
    REQUIRE( c.y() == -3 );
    REQUIRE( c.z() == -3 );
}

TEST_CASE( "Vector multiplication" )
{
    vec3 a(1, 2, 3);
    vec3 b(4, 5, 6);
    vec3 c = a * b;
    REQUIRE( c.x() == 4 );
    REQUIRE( c.y() == 10 );
    REQUIRE( c.z() == 18 );
}

TEST_CASE( "Vector division" )
{
    vec3 a(1, 2, 3);
    vec3 c = a / 2;
    REQUIRE( c.x() == 0.5 );
    REQUIRE( c.y() == 1 );
    REQUIRE( c.z() == 1.5 );
}

TEST_CASE( "Vector dot product" )
{
    vec3 a(1, 2, 3);
    vec3 b(4, 5, 6);
    float c = dot(a,b);
    REQUIRE( c == 32 );
}