#include <gtest/gtest.h>

#include <locale>

int main(int argc, char **argv) {
  ::setlocale(LC_CTYPE, "");
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
