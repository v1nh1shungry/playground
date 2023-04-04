#include <catch2/catch_test_macros.hpp>
#include <numeric>
#include <shared_ptr.hpp>
#include <string>
#include <vector>

TEST_CASE("nullptr") {
  my::shared_ptr<int> p1;
  REQUIRE(p1.use_count() == 0);
  REQUIRE(p1.get() == nullptr);
  my::shared_ptr<int> p2{nullptr};
  REQUIRE(p2.use_count() == 0);
  REQUIRE(p2.get() == nullptr);
}

TEST_CASE("basic") {
  my::shared_ptr p1{new int{10}};
  REQUIRE(p1.use_count() == 1);
  REQUIRE(*p1 == 10);
}

TEST_CASE("copy constructor") {
  my::shared_ptr<int> p1;
  auto p2 = p1;
  REQUIRE(p1.use_count() == 0);
  REQUIRE(p1.get() == nullptr);
  REQUIRE(p2.use_count() == 0);
  REQUIRE(p2.get() == nullptr);
  my::shared_ptr p3 = new int{10};
  auto p4 = p3;
  REQUIRE(p3.use_count() == 2);
  REQUIRE(*p3 == 10);
  REQUIRE(p4.use_count() == 2);
  REQUIRE(*p4 == 10);
}

TEST_CASE("move constructor") {
  my::shared_ptr<int> p1 = nullptr;
  auto p2 = std::move(p1);
  REQUIRE(p1.use_count() == 0);
  REQUIRE(p1.get() == nullptr);
  REQUIRE(p2.use_count() == 0);
  REQUIRE(p2.get() == nullptr);
  my::shared_ptr p3 = new int{10};
  auto p4{std::move(p3)};
  REQUIRE(p3.use_count() == 0);
  REQUIRE(p3.get() == nullptr);
  REQUIRE(p4.use_count() == 1);
  REQUIRE(*p4 == 10);
}

TEST_CASE("copy assignment") {
  my::shared_ptr<int> p1;
  my::shared_ptr p2 = new int{10};
  p1 = p2;
  REQUIRE(p1.use_count() == 2);
  REQUIRE(*p1 == 10);
  REQUIRE(p2.use_count() == 2);
  REQUIRE(*p2 == 10);
  my::shared_ptr<int> p3 = nullptr;
  p2 = p3;
  REQUIRE(p1.use_count() == 1);
  REQUIRE(*p1 == 10);
  REQUIRE(p2.use_count() == 0);
  REQUIRE(p2.get() == nullptr);
  REQUIRE(p3.use_count() == 0);
  REQUIRE(p3.get() == nullptr);
}

TEST_CASE("move assignment") {
  my::shared_ptr p1 = new int{10};
  my::shared_ptr<int> p2;
  p2 = std::move(p1);
  REQUIRE(p1.use_count() == 0);
  REQUIRE(p1.get() == nullptr);
  REQUIRE(p2.use_count() == 1);
  REQUIRE(*p2 == 10);
  my::shared_ptr<int> p3;
  p2 = std::move(p3);
  REQUIRE(p2.use_count() == 0);
  REQUIRE(p2.get() == nullptr);
  REQUIRE(p3.use_count() == 0);
  REQUIRE(p3.get() == nullptr);
}

TEST_CASE("reset") {
  my::shared_ptr<int> p1;
  p1.reset();
  REQUIRE(p1.use_count() == 0);
  REQUIRE(p1.get() == nullptr);
  my::shared_ptr p2 = new int{10};
  p2.reset();
  REQUIRE(p2.use_count() == 0);
  REQUIRE(p2.get() == nullptr);
  p1.reset(new int{10});
  REQUIRE(p1.use_count() == 1);
  REQUIRE(*p1 == 10);
  my::shared_ptr p3 = new int{10};
  p3.reset(new int{0});
  REQUIRE(p3.use_count() == 1);
  REQUIRE(*p3 == 0);
}

TEST_CASE("non-pod types") {
  my::shared_ptr p1 = new std::vector<std::string>;
  for (int i = 0; i < 10; ++i)
    p1->emplace_back("Hello");
  REQUIRE(std::accumulate(
      p1->begin(), p1->end(), true,
      [](bool lhs, const auto &rhs) { return lhs && (rhs == "Hello"); }));
}
