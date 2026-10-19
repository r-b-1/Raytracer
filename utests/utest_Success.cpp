#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "vec3.h"
#include "ray.h"

using Catch::Matchers::WithinAbs;

TEST_CASE("Vector addition")
{
    Vec3 a(1, 2, 3);
    Vec3 b(4, 5, 6);
    Vec3 c = a + b;
    REQUIRE( c.x() == 5 );
    REQUIRE( c.y() == 7 );
    REQUIRE( c.z() == 9 );
}

TEST_CASE("Vector subtraction")
{
    Vec3 a(1, 2, 3);
    Vec3 b(4, 5, 6);
    Vec3 c = a - b;
    REQUIRE( c.x() == -3 );
    REQUIRE( c.y() == -3 );
    REQUIRE( c.z() == -3 );
}

TEST_CASE("Vector multiplication")
{
    Vec3 a(1, 2, 3);
    Vec3 b(4, 5, 6);
    Vec3 c = a * b;
    REQUIRE( c.x() == 4 );
    REQUIRE( c.y() == 10 );
    REQUIRE( c.z() == 18 );
}

TEST_CASE("Vector division")
{
    Vec3 a(1, 2, 3);
    Vec3 c = a / 2;
    REQUIRE( c.x() == 0.5 );
    REQUIRE( c.y() == 1 );
    REQUIRE( c.z() == 1.5 );
}

TEST_CASE("Vector dot product")
{
    Vec3 a(1, 2, 3);
    Vec3 b(4, 5, 6);
    float c = dot(a,b);
    REQUIRE( c == 32 );
}

// Values below are not exactly representable as floats, so they need a
// tolerance rather than an exact comparison.

TEST_CASE("Vector length")
{
    // 3-4-5 triangle. 25 is exact, and sqrt(25) is exact.
    REQUIRE( Vec3(3, 4, 0).length() == 5.0f );

    // sqrt(3) is irrational, so compare within a tolerance.
    REQUIRE_THAT( Vec3(1, 1, 1).length(), WithinAbs(1.7320508f, 0.0001f) );

    REQUIRE( Vec3().length() == 0.0f );
}

TEST_CASE("Unit vector")
{
    // Axis-aligned input gives an exactly representable result.
    Vec3 x = unit_vector(Vec3(7, 0, 0));
    REQUIRE( x.x() == 1.0f );
    REQUIRE( x.y() == 0.0f );
    REQUIRE( x.z() == 0.0f );

    // 3-4-5 normalized is (0.6, 0.8, 0). Not exact in binary.
    Vec3 u = unit_vector(Vec3(3, 4, 0));
    REQUIRE_THAT( u.x(), WithinAbs(0.6f, 0.0001f) );
    REQUIRE_THAT( u.y(), WithinAbs(0.8f, 0.0001f) );

    // A unit vector always has length 1.
    REQUIRE_THAT( u.length(), WithinAbs(1.0f, 0.0001f) );
}

TEST_CASE("Vector cross product")
{
    Vec3 x(1, 0, 0);
    Vec3 y(0, 1, 0);
    Vec3 z(0, 0, 1);

    // Cross product builds a right-handed basis. Exactly representable.
    Vec3 c = cross(x, y);
    REQUIRE( c.x() == 0.0f );
    REQUIRE( c.y() == 0.0f );
    REQUIRE( c.z() == 1.0f );

    REQUIRE( cross(y, z).x() == 1.0f );
    REQUIRE( cross(z, x).y() == 1.0f );

    // Swapping the arguments reverses the sign.
    REQUIRE( cross(x, y).z() == -cross(y, x).z() );

    // The result is perpendicular to both inputs.
    Vec3 a(1, 2, 3);
    Vec3 b(4, 5, 6);
    REQUIRE_THAT( dot(cross(a, b), a), WithinAbs(0.0f, 0.0001f) );
    REQUIRE_THAT( dot(cross(a, b), b), WithinAbs(0.0f, 0.0001f) );
}

TEST_CASE("Unary negation")
{
    Vec3 a(1, -2, 3);
    Vec3 c = -a;
    REQUIRE( c.x() == -1.0f );
    REQUIRE( c.y() == 2.0f );
    REQUIRE( c.z() == -3.0f );

    // Negating twice gets you back to where you started.
    Vec3 d = -(-a);
    REQUIRE( d.x() == a.x() );
    REQUIRE( d.y() == a.y() );
    REQUIRE( d.z() == a.z() );
}

TEST_CASE("Component-wise multiplication is not the dot product")
{
    Vec3 a(1, 2, 3);
    Vec3 b(4, 5, 6);

    REQUIRE( (a * b).x() == 4.0f );
    REQUIRE( (a * b).y() == 10.0f );
    REQUIRE( (a * b).z() == 18.0f );

    REQUIRE( dot(a, b) == 32.0f );
}


// Ray tests

TEST_CASE("Ray construction and accessors")
{
    Point3 origin(1, 2, 3);
    Vec3 direction(4, 5, 6);
    Ray r(origin, direction);
    
    REQUIRE( r.origin().x() == 1 );
    REQUIRE( r.origin().y() == 2 );
    REQUIRE( r.origin().z() == 3 );
    
    REQUIRE( r.direction().x() == 4 );
    REQUIRE( r.direction().y() == 5 );
    REQUIRE( r.direction().z() == 6 );
}

TEST_CASE("Ray at() function - point along ray")
{
    Point3 origin(0, 0, 0);
    Vec3 direction(1, 0, 0);
    Ray r(origin, direction);
    
    // At t=0, should be at the origin
    Point3 p0 = r.at(0);
    REQUIRE( p0.x() == 0 );
    REQUIRE( p0.y() == 0 );
    REQUIRE( p0.z() == 0 );
    
    // At t=5, should be 5 units along the direction
    Point3 p5 = r.at(5);
    REQUIRE( p5.x() == 5 );
    REQUIRE( p5.y() == 0 );
    REQUIRE( p5.z() == 0 );
}

TEST_CASE("Ray at() with non-origin starting point")
{
    Point3 origin(2, 3, 4);
    Vec3 direction(1, 1, 1);
    Ray r(origin, direction);
    
    // At t=2, should be at origin + 2*direction
    Point3 p = r.at(2);
    REQUIRE( p.x() == 4 );
    REQUIRE( p.y() == 5 );
    REQUIRE( p.z() == 6 );
}

TEST_CASE("Ray at() with negative t")
{
    Point3 origin(5, 5, 5);
    Vec3 direction(1, 0, 0);
    Ray r(origin, direction);
    
    // At negative t, should go backwards along the direction
    Point3 p = r.at(-3);
    REQUIRE( p.x() == 2 );
    REQUIRE( p.y() == 5 );
    REQUIRE( p.z() == 5 );
}

TEST_CASE("Ray default constructor")
{
    Ray r;
    // Default ray should initialize with default Vec3 values (0, 0, 0)
    REQUIRE( r.origin().x() == 0 );
    REQUIRE( r.origin().y() == 0 );
    REQUIRE( r.origin().z() == 0 );
    REQUIRE( r.direction().x() == 0 );
    REQUIRE( r.direction().y() == 0 );
    REQUIRE( r.direction().z() == 0 );
}

