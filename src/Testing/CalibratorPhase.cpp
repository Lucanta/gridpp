#include "../File/Fake.h"
#include "../Util.h"
#include "../ParameterFile.h"
#include "../Calibrator/Zaga.h"
#include <gtest/gtest.h>

namespace {
   class TestCalibratorPhase : public ::testing::Test {
      protected:
         TestCalibratorPhase() {
         }
         virtual ~TestCalibratorPhase() {
         }
         virtual void SetUp() {
         }
         virtual void TearDown() {
         }
         Parameters getParameters(float a1, float a2) {
            std::vector<float> parValues(8, 0);
            parValues[0] = a1;
            parValues[1] = a2;
            return Parameters (parValues);
         }
         ParameterFile getParameterFile(float a1, float a2) {
            ParameterFile parFile("testing/files/parametersPhase.txt");
            parFile.setParameters(getParameters(a1, a2), 0);
            return parFile;
         }
         CalibratorPhase getCalibrator(ParameterFile* parFile) {
            return CalibratorPhase(parFile);
         }
   };

   TEST_F(TestCalibratorPhase, 10x10) {
      FileArome file("testing/files/10x10.nc");
      ParameterFile parFile = getParameterFile(273.7,274.7);
      CalibratorPhase cal = getCalibrator(&parFile);

      cal.calibrate(file);
      FieldPtr phase    = file.getField(Variable::Phase, 0);
      FieldPtr precip   = file.getField(Variable::Precip, 0);
      FieldPtr temp     = file.getField(Variable::T, 0);
      FieldPtr rh       = file.getField(Variable::RH, 0);
      FieldPtr pressure = file.getField(Variable::P, 0);
      // T      301 K
      // RH     0.95925
      // P      98334 pa
      // Precip 1.38 mm
      EXPECT_FLOAT_EQ(CalibratorPhase::PhaseRain, (*phase)(2,5,0));

      (*temp)(2,5,0) = 270;
      cal.calibrate(file);
      EXPECT_FLOAT_EQ(CalibratorPhase::PhaseSnow, (*phase)(2,5,0));

      (*temp)(2,5,0) = 274;
      cal.calibrate(file);
      EXPECT_FLOAT_EQ(CalibratorPhase::PhaseSleet, (*phase)(2,5,0));

      (*precip)(2,5,0) = 0;
      cal.calibrate(file);
      EXPECT_FLOAT_EQ(CalibratorPhase::PhaseNone, (*phase)(2,5,0));
   }
   TEST_F(TestCalibratorPhase, missingParameters) {
      FileArome file("testing/files/10x10.nc");
      ParameterFile parFile = getParameterFile(Util::MV,1.5);
      CalibratorPhase cal = getCalibrator(&parFile);

      cal.calibrate(file);
      FieldPtr phase = file.getField(Variable::Phase, 0);
      for(int i = 0; i < file.getNumLat(); i++) {
         for(int j = 0; j < file.getNumLon(); j++) {
            for(int e = 0; e < file.getNumEns(); e++) {
               EXPECT_FLOAT_EQ(Util::MV, (*phase)(i,j,e));
            }
         }
      }
   }
   TEST_F(TestCalibratorPhase, getWetbulb) {
      ::testing::FLAGS_gtest_death_test_style = "threadsafe";
      Util::setShowError(false);
                                                                                  // NOAA
      EXPECT_FLOAT_EQ(269.02487, CalibratorPhase::getWetbulb(270, 100000, 0.80)); // 269.03
      EXPECT_FLOAT_EQ(296.13763, CalibratorPhase::getWetbulb(300, 101000, 0.70)); // 295.95
      EXPECT_FLOAT_EQ(269.92218, CalibratorPhase::getWetbulb(270, 100000, 1));    // 270
      EXPECT_FLOAT_EQ(239.83798, CalibratorPhase::getWetbulb(240, 50000, 0.90));  // 239.89
   }
   TEST_F(TestCalibratorPhase, getWetbulbInvalid) {
      EXPECT_FLOAT_EQ(Util::MV, CalibratorPhase::getWetbulb(Util::MV, 30000, 1));
      EXPECT_FLOAT_EQ(Util::MV, CalibratorPhase::getWetbulb(270, Util::MV, 1));
      EXPECT_FLOAT_EQ(Util::MV, CalibratorPhase::getWetbulb(270, 30000, Util::MV));
      EXPECT_FLOAT_EQ(Util::MV,    CalibratorPhase::getWetbulb(270, 100000, 0)); // No humidity
   }
   TEST_F(TestCalibratorPhase, description) {
      CalibratorPhase::description();
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
