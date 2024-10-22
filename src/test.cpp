#include "vector.tcc"

#include <catch2/catch_test_macros.hpp>

using namespace vector;

TEST_CASE("Constructors work", "[constructors]") {
    std::array arr {1, 2, 3, 4, 5};

    Vector<int> v0;
    REQUIRE(v0.empty());

    Vector v1(10, 1);
    REQUIRE(v1.size() == 10);
    REQUIRE(v1[0] == 1);
    REQUIRE(v1[9] == 1);

    Vector<int> v2(5);
    REQUIRE(v2.size() == 5);
    REQUIRE(v2[0] == 0);
    REQUIRE(v2[4] == 0);

    Vector<int> v3(arr.begin(), arr.end());
    REQUIRE(v3.size() == arr.size());
    REQUIRE(v3[2] == arr[2]);

    Vector v4(v3);
    REQUIRE(v4.size() == v3.size());
    REQUIRE(v4[2] == v3[2]);

    Vector v5(std::move(v4));
    REQUIRE(v5.size() == v3.size());
    REQUIRE(v5[2] == v3[2]);
    REQUIRE(v4.empty());

    Vector v6 {5, 10, 15, 20, 25};
    REQUIRE(v6[2] == 15);
}
