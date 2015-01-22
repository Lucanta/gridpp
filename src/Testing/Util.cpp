#include "../Util.h"
#include <algorithm>
#include <math.h>
#include <gtest/gtest.h>

namespace {
   class UtilTest : public ::testing::Test {
      protected:
   };

   TEST_F(UtilTest, isValid) {
      EXPECT_FALSE(Util::isValid(Util::MV));
      EXPECT_FALSE(Util::isValid(1.0/0));
      EXPECT_TRUE(Util::isValid(-15));
      EXPECT_TRUE(Util::isValid(0));
      EXPECT_TRUE(Util::isValid(3.14));
   }
   TEST_F(UtilTest, deg2rad) {
      EXPECT_FLOAT_EQ(2*Util::pi, Util::deg2rad(360));
      EXPECT_FLOAT_EQ(-2*Util::pi, Util::deg2rad(-360));
      EXPECT_FLOAT_EQ(0, Util::deg2rad(0));
      EXPECT_FLOAT_EQ(1.0/3*Util::pi, Util::deg2rad(60));
   }
   TEST_F(UtilTest, rad2deg) {
      EXPECT_FLOAT_EQ(360, Util::rad2deg(2*Util::pi));
      EXPECT_FLOAT_EQ(-360, Util::rad2deg(-2*Util::pi));
      EXPECT_FLOAT_EQ(60, Util::rad2deg(1.0/3*Util::pi));
   }
   TEST_F(UtilTest, getDistance) {
      EXPECT_FLOAT_EQ(0, Util::getDistance(60,10,60,10));
      EXPECT_FLOAT_EQ(20037508, Util::getDistance(90,10,-90,10));
      EXPECT_FLOAT_EQ(20037508, Util::getDistance(0,0,0,180));
      EXPECT_FLOAT_EQ(16879114, Util::getDistance(60.5,5.25,-84.75,-101.75));
   }
   TEST_F(UtilTest, sortFirst) {
      typedef std::pair<int,int> pair;
      std::vector<pair> pairs;
      pairs.push_back(pair(0,2));
      pairs.push_back(pair(1,4));
      pairs.push_back(pair(3,5));
      pairs.push_back(pair(2,3));
      std::sort(pairs.begin(), pairs.end(), Util::sort_pair_first<int,int>());
      EXPECT_EQ(0, pairs[0].first);
      EXPECT_EQ(1, pairs[1].first);
      EXPECT_EQ(2, pairs[2].first);
      EXPECT_EQ(3, pairs[3].first);
      EXPECT_EQ(2, pairs[0].second);
      EXPECT_EQ(4, pairs[1].second);
      EXPECT_EQ(3, pairs[2].second);
      EXPECT_EQ(5, pairs[3].second);
   }
   TEST_F(UtilTest, sortSecond) {
      typedef std::pair<int,int> pair;
      std::vector<pair> pairs;
      pairs.push_back(pair(0,2));
      pairs.push_back(pair(1,4));
      pairs.push_back(pair(3,5));
      pairs.push_back(pair(2,3));
      std::sort(pairs.begin(), pairs.end(), Util::sort_pair_second<int,int>());
      EXPECT_EQ(0, pairs[0].first);
      EXPECT_EQ(2, pairs[1].first);
      EXPECT_EQ(1, pairs[2].first);
      EXPECT_EQ(3, pairs[3].first);
      EXPECT_EQ(2, pairs[0].second);
      EXPECT_EQ(3, pairs[1].second);
      EXPECT_EQ(4, pairs[2].second);
      EXPECT_EQ(5, pairs[3].second);
   }
   TEST_F(UtilTest, calcDate) {
      EXPECT_EQ(20150101, Util::calcDate(20141231, 24));
      EXPECT_EQ(20141231, Util::calcDate(20141231, 0));
      EXPECT_EQ(20141231, Util::calcDate(20141231, 1.5));
      EXPECT_EQ(20150101, Util::calcDate(20141231, 47.9));
      EXPECT_EQ(20141231, Util::calcDate(20150101, -0.1));
      EXPECT_EQ(20141231, Util::calcDate(20150101, -24));
      EXPECT_EQ(20141230, Util::calcDate(20150101, -24.01));
   }
   TEST_F(UtilTest, split) {
      std::string s = "test 1 2 3";
      std::vector<std::string> strings = Util::split(s);
      ASSERT_EQ(4, strings.size());
      EXPECT_EQ("test", strings[0]);
      EXPECT_EQ("1", strings[1]);
      EXPECT_EQ("2", strings[2]);
      EXPECT_EQ("3", strings[3]);
   }
   TEST_F(UtilTest, splitEmpty) {
      std::string s = "";
      std::vector<std::string> strings = Util::split(s);
      EXPECT_EQ(0, strings.size());
   }
   TEST_F(UtilTest, splitEmptyExtraSpace) {
      std::string s = "     ";
      std::vector<std::string> strings = Util::split(s);
      EXPECT_EQ(0, strings.size());
   }
   TEST_F(UtilTest, splitExtraSpace) {
      std::string s = "  test  1 ";
      std::vector<std::string> strings = Util::split(s);
      ASSERT_EQ(2, strings.size());
      EXPECT_EQ("test", strings[0]);
      EXPECT_EQ("1", strings[1]);
   }
   TEST_F(UtilTest, logit) {
      const float p[] = {0.2,0.9};
      const float exp[] = {-1.386294, 2.197225};
      for(int i = 0; i < sizeof(p)/sizeof(float); i++) {
         float ans = Util::logit(p[i]);
         EXPECT_FLOAT_EQ(exp[i], ans);
      }
   }
   TEST_F(UtilTest, invlogit) {
      const float x[] = {-2, 3};
      const float exp[] = {0.1192029, 0.9525741};
      for(int i = 0; i < sizeof(x)/sizeof(float); i++) {
         float ans = Util::invLogit(x[i]);
         EXPECT_FLOAT_EQ(exp[i], ans);
      }
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
