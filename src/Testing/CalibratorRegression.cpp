#include "../File/Fake.h"
#include "../Util.h"
#include "../ParameterFile/ParameterFile.h"
#include "../Calibrator/Regression.h"
#include <gtest/gtest.h>

namespace {
   class TestCalibratorRegression : public ::testing::Test {
      protected:
         TestCalibratorRegression() {
         }
         virtual ~TestCalibratorRegression() {
         }
         virtual void SetUp() {
         }
         virtual void TearDown() {
         }
   };
   // Constant correction to 0.3
   TEST_F(TestCalibratorRegression, 10x10_0order) {
      FileArome from("testing/files/10x10.nc");
      ParameterFile par("testing/files/regression0order.txt");
      CalibratorRegression cal = CalibratorRegression(&par, Variable::T);

      cal.calibrate(from);
      FieldPtr after = from.getField(Variable::T, 0);
      ASSERT_EQ(10, after->getNumLat());
      ASSERT_EQ(10, after->getNumLon());
      ASSERT_EQ(1,  after->getNumEns());

      EXPECT_FLOAT_EQ(0.3, (*after)(5,2,0));
      EXPECT_FLOAT_EQ(0.3, (*after)(5,9,0));
      EXPECT_FLOAT_EQ(0.3, (*after)(0,9,0));
   }
   TEST_F(TestCalibratorRegression, 10x10_1order) {
      FileArome from("testing/files/10x10.nc");
      ParameterFile par("testing/files/regression1order.txt");
      CalibratorRegression cal = CalibratorRegression(&par, Variable::T);

      cal.calibrate(from);
      FieldPtr after = from.getField(Variable::T, 0);
      ASSERT_EQ(10, after->getNumLat());
      ASSERT_EQ(10, after->getNumLon());
      ASSERT_EQ(1,  after->getNumEns());

      EXPECT_FLOAT_EQ(361.5, (*after)(5,2,0)); // 0.3 + 1.2*301
      EXPECT_FLOAT_EQ(365.1, (*after)(5,9,0));
      EXPECT_FLOAT_EQ(384.3, (*after)(0,9,0));
   }
   TEST_F(TestCalibratorRegression, 10x10_2order) {
      FileArome from("testing/files/10x10.nc");
      ParameterFile par("testing/files/regression2order.txt");
      CalibratorRegression cal = CalibratorRegression(&par, Variable::T);

      cal.calibrate(from);
      FieldPtr after = from.getField(Variable::T, 0);
      ASSERT_EQ(10, after->getNumLat());
      ASSERT_EQ(10, after->getNumLon());
      ASSERT_EQ(1,  after->getNumEns());

      EXPECT_FLOAT_EQ(-72174.08, (*after)(5,2,0)); // -0.3 + 1.02*301 - 0.8*301^2
      EXPECT_FLOAT_EQ(-73623.02, (*after)(5,9,0));
      EXPECT_FLOAT_EQ(-81593.90, (*after)(0,9,0));
   }
   TEST_F(TestCalibratorRegression, missing_parameters) {
      FileArome from("testing/files/10x10.nc");
      ParameterFile par("testing/files/regressionMissing.txt");
      CalibratorRegression cal = CalibratorRegression(&par, Variable::T);

      cal.calibrate(from);
      FieldPtr after = from.getField(Variable::T, 0);
      ASSERT_EQ(10, after->getNumLat());
      ASSERT_EQ(10, after->getNumLon());
      ASSERT_EQ(1,  after->getNumEns());

      EXPECT_FLOAT_EQ(Util::MV, (*after)(5,2,0)); // 0.3 + 1.2*301
      EXPECT_FLOAT_EQ(Util::MV, (*after)(5,9,0));
      EXPECT_FLOAT_EQ(Util::MV, (*after)(0,9,0));
   }
   // Incorrect number of data columns
   TEST_F(TestCalibratorRegression, invalid) {
      ::testing::FLAGS_gtest_death_test_style = "threadsafe";
      Util::setShowError(false);
      ParameterFile par1("testing/files/regressionInvalid1.txt");
      EXPECT_DEATH(CalibratorRegression cal = CalibratorRegression(&par1, Variable::T), ".*");
   }
   TEST_F(TestCalibratorRegression, description) {
      CalibratorRegression::description();
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
