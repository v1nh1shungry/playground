#include <catch2/catch_test_macros.hpp>
#include <string.hpp>

const char *short_str = "Hello, world!";
const char *long_str =
    "Hello, world!Hello, world!Hello, world!Hello, world!Hello, world!Hello, "
    "world!Hello, world!Hello, world!";
char buffer[] = "Hello, world!";

TEST_CASE("empty string") {
  my::string s1;
  REQUIRE(s1.empty());
  REQUIRE(s1.size() == 0);
  REQUIRE(strcmp(s1.c_str(), "") == 0);
}

TEST_CASE("C style string constructor") {
  my::string s1 = short_str;
  REQUIRE(s1.size() == strlen(short_str));
  REQUIRE(strcmp(s1.c_str(), short_str) == 0);
  my::string s2 = long_str;
  REQUIRE(s2.size() == strlen(long_str));
  REQUIRE(strcmp(s2.c_str(), long_str) == 0);
  my::string s3 = buffer;
  REQUIRE(s3.size() == strlen(buffer));
  REQUIRE(strcmp(s3.c_str(), buffer) == 0);
}

TEST_CASE("copy constructor") {
  my::string s1;
  auto s2 = s1;
  REQUIRE(s1.size() == 0);
  REQUIRE(strcmp(s1.c_str(), "") == 0);
  REQUIRE(s2.size() == 0);
  REQUIRE(strcmp(s2.c_str(), "") == 0);
  my::string s3{short_str};
  auto s4 = s3;
  REQUIRE(s3.size() == strlen(short_str));
  REQUIRE(strcmp(s3.c_str(), short_str) == 0);
  REQUIRE(s4.size() == strlen(short_str));
  REQUIRE(strcmp(s4.c_str(), short_str) == 0);
}

TEST_CASE("move constructor") {
  my::string s1;
  auto s2 = std::move(s1);
  REQUIRE(s1.size() == 0);
  REQUIRE(strcmp(s1.c_str(), "") == 0);
  REQUIRE(s2.size() == 0);
  REQUIRE(strcmp(s2.c_str(), "") == 0);
  my::string s3 = short_str;
  auto s4 = std::move(s3);
  REQUIRE(s3.size() == 0);
  REQUIRE(strcmp(s3.c_str(), "") == 0);
  REQUIRE(s4.size() == strlen(short_str));
  REQUIRE(strcmp(s4.c_str(), short_str) == 0);
}

TEST_CASE("C style string assignment") {
  my::string s1;
  s1 = short_str;
  REQUIRE(s1.size() == strlen(short_str));
  REQUIRE(strcmp(s1.c_str(), short_str) == 0);
  s1 = long_str;
  REQUIRE(s1.size() == strlen(long_str));
  REQUIRE(strcmp(s1.c_str(), long_str) == 0);
}

TEST_CASE("copy assignment") {
  my::string s1 = short_str;
  my::string s2;
  s2 = s1;
  REQUIRE(s1.size() == strlen(short_str));
  REQUIRE(strcmp(s1.c_str(), short_str) == 0);
  REQUIRE(s2.size() == strlen(short_str));
  REQUIRE(strcmp(s2.c_str(), short_str) == 0);
  my::string s3 = long_str;
  s2 = s3;
  REQUIRE(s2.size() == strlen(long_str));
  REQUIRE(strcmp(s2.c_str(), long_str) == 0);
  REQUIRE(s3.size() == strlen(long_str));
  REQUIRE(strcmp(s3.c_str(), long_str) == 0);
}

TEST_CASE("move assignment") {
  my::string s1 = short_str;
  my::string s2;
  s2 = std::move(s1);
  REQUIRE(s1.size() == 0);
  REQUIRE(strcmp(s1.c_str(), "") == 0);
  REQUIRE(s2.size() == strlen(short_str));
  REQUIRE(strcmp(s2.c_str(), short_str) == 0);
  s1 = std::move(s2);
  REQUIRE(s1.size() == strlen(short_str));
  REQUIRE(strcmp(s1.c_str(), short_str) == 0);
  REQUIRE(s2.size() == 0);
  REQUIRE(strcmp(s2.c_str(), "") == 0);
}

TEST_CASE("push_back") {
  my::string s1;
  s1.push_back('a');
  REQUIRE(s1.size() == 1);
  REQUIRE(strcmp(s1.c_str(), "a") == 0);
}

TEST_CASE("pop_back") {
  my::string s1 = "abc";
  s1.pop_back();
  REQUIRE(s1.size() == 2);
  REQUIRE(strcmp(s1.c_str(), "ab") == 0);
}
