#ifndef KDTREE_H
#define KDTREE_H
#include <boost/scoped_ptr.hpp>
#include <vector>
#include "Util.h"
#include "File/File.h"
typedef std::vector<std::vector<int> > vec2Int;

class KDTree {
   public:
      KDTree(const vec2& iLats, const vec2& iLons);

      void getNearestNeighbour(const File& iTo, vec2Int& iI, vec2Int& iJ) const;
      // I,J: The indices into the lat/lon grid with the nearest neighbour
      void getNearestNeighbour(float iLat, float iLon, int& iI, int& iJ) const;

   private:
      struct TreeNode {
         bool xsection;
         float lon;
         float lat;
         size_t ipos;
         size_t jpos;

         boost::scoped_ptr<TreeNode> left;
         boost::scoped_ptr<TreeNode> right;

         const TreeNode* parent;

         TreeNode(): xsection(false), left(NULL), right(NULL), parent(NULL) {}
      };

      struct Indexed {
         float lon;
         float lat;
         size_t index1;
         size_t index2;

         Indexed() {}
         Indexed(const float lon_,
                 const float lat_,
                 const size_t index1_,
                 const size_t index2_):
            lon(lon_), lat(lat_), index1(index1_), index2(index2_) {}
      };

      friend bool compareLons (const KDTree::Indexed& l, const KDTree::Indexed& r);
      friend bool compareLats (const KDTree::Indexed& l, const KDTree::Indexed& r);
      typedef boost::scoped_ptr<TreeNode> unode;
      typedef std::vector<Indexed> indexdVec;

      unode mRoot;

      static const TreeNode* nearestNeighbour(const unode& root, const float lon, const float lat);
      static const TreeNode* firstGuess(const unode& root, const float lon, const float lat);
      static void subTree(indexdVec& iLonLat,
                          const size_t from,
                          const size_t to,
                          const bool xsection,
                          const TreeNode* parent,
                          unode& root);

};

#endif
