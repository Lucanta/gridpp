#ifndef CALIBRATOR_H
#define CALIBRATOR_H
#include <string>
#include <vector>
class File;
class Options;

//! Abstract calibration class
class Calibrator {
   public:
      Calibrator();
      virtual ~Calibrator() {};
      //! \brief Calibrate one or more fields in iFile
      //! @return true if calibration was successful, false otherwise
      bool calibrate(File& iFile) const;

      //! Instantiates a calibrator with name iName
      static Calibrator* getScheme(std::string iName, const Options& iOptions);

      //! \brief Ensure that values in iAfter are in the same order as in iBefore.
      //! If missing values are encountered in iBefore or iAfter, then iAfter is left unchanged.
      //! If the sizes are different, then iAfter is left unchanged.
      static void  shuffle(const std::vector<float>& iBefore, std::vector<float>& iAfter);

      //! Returns the name of this calibrator
      virtual std::string name() const = 0;
   protected:
      virtual bool calibrateCore(File& iFile) const = 0;
   private:
};
#include "Zaga.h"
#include "Cloud.h"
// #include "Wind.h"
#include "Temperature.h"
#include "Accumulate.h"
#include "Neighbourhood.h"
#include "Qc.h"
#include "Qnh.h"
#include "Qq.h"
#include "Phase.h"
#include "Regression.h"
#include "WindDirection.h"
#include "Kriging.h"
#include "Window.h"
#endif
