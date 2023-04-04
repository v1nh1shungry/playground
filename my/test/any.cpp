#include <any.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vector>

TEST_CASE("empty") {
  my::any a1;
  REQUIRE(a1.has_value() == false);
}

TEST_CASE("basic constructor") {
  my::any a1 = 10;
  REQUIRE(a1.has_value());
  REQUIRE(a1.get<int>() == 10);
}

TEST_CASE("copy constructor") {
  my::any a1 = 10;
  auto a2 = a1;
  REQUIRE(a1.has_value());
  REQUIRE(a1.get<int>() == 10);
  REQUIRE(a2.has_value());
  REQUIRE(a2.get<int>() == 10);
}

TEST_CASE("move constructor") {
  my::any a1 = 10;
  auto a2 = std::move(a1);
  REQUIRE(a1.has_value() == false);
  REQUIRE(a2.has_value());
  REQUIRE(a2.get<int>() == 10);
}

TEST_CASE("non-pod types") {
  my::any a1 = std::vector{0, 1, 2, 3, 4};
  for (int i = 0; i < 5; ++i)
    REQUIRE(a1.get<std::vector<int>>()[i] == i);
  auto &&v = my::any{std::vector{0, 1, 2, 3, 4}}.get<std::vector<int>>();
  for (int i = 0; i < 5; ++i)
    REQUIRE(v[i] == i);
}
