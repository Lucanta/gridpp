#include "Gradient.h"
#include "../File/File.h"
#include "../Util.h"
#include <math.h>

DownscalerGradient::DownscalerGradient(Variable::Type iVariable) :
      Downscaler(iVariable),
      mSearchRadius(3),
      mMinGradient(-10),
      mMaxGradient(10),
      mConstGradient(Util::MV) {
}

void DownscalerGradient::downscaleCore(const File& iInput, File& iOutput) const {
   int nLat = iOutput.getNumLat();
   int nLon = iOutput.getNumLon();
   int nEns = iOutput.getNumEns();
   int nTime = iInput.getNumTime();

   vec2 ilats  = iInput.getLats();
   vec2 ilons  = iInput.getLons();
   vec2 ielevs = iInput.getElevs();
   vec2 olats  = iOutput.getLats();
   vec2 olons  = iOutput.getLons();
   vec2 oelevs = iOutput.getElevs();

   // Get nearest neighbour
   vec2Int nearestI, nearestJ;
   DownscalerNearestNeighbour::getNearestNeighbourFast(iInput, iOutput, nearestI, nearestJ);

   for(int t = 0; t < nTime; t++) {
      Field& ifield = *iInput.getField(mVariable, t);
      Field& ofield = *iOutput.getField(mVariable, t);

      #pragma omp parallel for
      for(int i = 0; i < nLat; i++) {
         for(int j = 0; j < nLon; j++) {
            int Icenter = nearestI[i][j];
            int Jcenter = nearestJ[i][j];
            assert(Icenter < ielevs.size());
            assert(Jcenter < ielevs[Icenter].size());
            for(int e = 0; e < nEns; e++) {
               float currElev = oelevs[i][j];
               float nearestElev = ielevs[Icenter][Jcenter];
               if(!Util::isValid(currElev) || !Util::isValid(nearestElev)) {
                  // Can't adjust if we don't have an elevation, use nearest neighbour
                  ofield(i,j,e) = ifield(Icenter,Jcenter,e);
               }
               else {
                  float dElev = currElev - nearestElev;
                  float gradient = 0;
                  if(!Util::isValid(mConstGradient)) {
                     // Compute gradient from neighbourhood
                     float meanXY  = 0; // elev*T
                     float meanX   = 0; // elev
                     float meanY   = 0; // T
                     float meanXX  = 0; // elev*elev
                     int   counter = 0;
                     for(int ii = std::max(0, Icenter-mSearchRadius); ii <= std::min(iInput.getNumLat()-1, Icenter+mSearchRadius); ii++) {
                        for(int jj = std::max(0, Jcenter-mSearchRadius); jj <= std::min(iInput.getNumLon()-1, Jcenter+mSearchRadius); jj++) {
                           assert(ii < ielevs.size());
                           assert(jj < ielevs[ii].size());
                           float x = ielevs[ii][jj];
                           float y = ifield(ii,jj,e);
                           if(Util::isValid(x) && Util::isValid(y)) {
                              meanXY += x*y;
                              meanX  += x;
                              meanY  += y;
                              meanXX += x*x;
                              counter++;
                           }
                        }
                     }
                     if(counter > 0 && meanXX != meanX*meanX) {
                        // Estimate lapse rate
                        meanXY /= counter;
                        meanX  /= counter;
                        meanY  /= counter;
                        meanXX /= counter;
                        gradient = (meanXY - meanX*meanY)/(meanXX - meanX*meanX);
                     }
                     // Safety check
                     if(!Util::isValid(gradient))
                        gradient = 0;
                     if(gradient > mMaxGradient)
                        gradient = mMaxGradient;
                     if(gradient < mMinGradient)
                        gradient = mMinGradient;
                  }
                  else {
                     gradient = mConstGradient;
                  }
                  ofield(i,j,e) = ifield(Icenter,Jcenter,e) + dElev * gradient;
               }
            }
         }
      }
   }
}
void DownscalerGradient::setConstantGradient(float iGradient) {
   if(!Util::isValid(iGradient)) {
      std::stringstream ss;
      ss << "DownscalerGradient: constant gradient must be a valid number";
      Util::error(ss.str());
   }
   mConstGradient = iGradient;
}
float DownscalerGradient::getConstantGradient() const {
   return mConstGradient;
}
void DownscalerGradient::setSearchRadius(int iNumPoints) {
   if(!Util::isValid(iNumPoints) || iNumPoints <= 0) {
      std::stringstream ss;
      ss << "DownscalerGradient: search radius must be >= 1";
      Util::error(ss.str());
   }
   mSearchRadius = iNumPoints;
}

int DownscalerGradient::getSearchRadius() const {
   return mSearchRadius;
}

std::string DownscalerGradient::description() {
   std::stringstream ss;
   ss << "   -d gradient                  Adjusts the nearest neighbour based on the elevation difference" << std::endl;
   ss << "                                to the output gridpoint." << std::endl;
   ss << "      constantGradient=undef    Fix gradient to this value. If unspecified, computes the gradient" << std::endl;
   ss << "                                by linear regression of points in a neighbourhood." << std::endl;
   ss << "      searchRadius=3            Compute gradient in a neighbourhood box of points within +- radius" << std::endl;
   ss << "                                in both east-west and north-south direction." << std::endl;
   return ss.str();
}
