#ifndef __APPLE__
#include "HDevThread.h"
#include "HalconCpp.h"
#else
#ifndef HC_LARGE_IMAGES
#include <HALCONCpp/HDevThread.h>
#include <HALCONCpp/HalconCpp.h>
#else
#include <HALCONCppxl/HDevThread.h>
#include <HALCONCppxl/HalconCpp.h>
#endif
#endif

#include "FilterRefit.hpp"

using namespace HalconCpp;

// Procedures
// Chapter: 3D Object Model / Features
// Short Description: This procedure first fit a plane using all input points.
// Then A percentage of points is stripped which have the nearest and farthest
// distance to the plane. Finally a new plane is fitted by the remaining
// points.
void
FilterRefit (HTuple hv_ObjectModel3D, HTuple hv_NearestFilter,
             HTuple hv_FarthestFilter, HTuple *hv_Plane)
{

  // Local iconic variables

  // Local control variables
  HTuple hv_ObjectModel3DOut, hv_Dist, hv_X, hv_Y;
  HTuple hv_Z, hv_DistSortedIndex, hv_Len, hv_One, hv_Zero;
  HTuple hv_Mask, hv_Index;

  FitPrimitivesObjectModel3d (
      hv_ObjectModel3D,
      (HTuple ("primitive_type").Append ("fitting_algorithm")),
      (HTuple ("plane").Append ("least_squares_tukey")), &hv_ObjectModel3DOut);
  DistanceObjectModel3d (hv_ObjectModel3D, hv_ObjectModel3DOut, HTuple (), 0,
                         HTuple (), HTuple ());
  GetObjectModel3dParams (hv_ObjectModel3D, "&distance", &hv_Dist);
  GetObjectModel3dParams (hv_ObjectModel3D, "point_coord_x", &hv_X);
  GetObjectModel3dParams (hv_ObjectModel3D, "point_coord_y", &hv_Y);
  GetObjectModel3dParams (hv_ObjectModel3D, "point_coord_z", &hv_Z);
  // Filter farthest points and re-fit
  TupleSortIndex (hv_Dist, &hv_DistSortedIndex);
  TupleLength (hv_DistSortedIndex, &hv_Len);
  TupleGenConst (hv_Len * ((1 - hv_NearestFilter) - hv_FarthestFilter), 1,
                 &hv_One);
  if (0 != (int ((hv_NearestFilter * hv_Len) > 0)))
    {
      TupleGenConst (hv_Len * hv_NearestFilter, 0, &hv_Zero);
    }
  else
    {
      hv_Zero = 0;
    }
  hv_Mask.Clear ();
  hv_Mask.Append (hv_Zero);
  hv_Mask.Append (hv_One);
  if (0
      != (int ((hv_Mask.TupleLength ())
               > (hv_DistSortedIndex.TupleLength ()))))
    {
      hv_Mask = hv_Mask.TupleSelectRange (
          0, (hv_DistSortedIndex.TupleLength ()) - 1);
    }
  TupleGenConst (hv_Len - (hv_Mask.TupleLength ()), 0, &hv_Zero);
  hv_Mask = hv_Mask.TupleConcat (hv_Zero);

  TupleSelectMask (hv_DistSortedIndex, hv_Mask, &hv_Index);
  TupleSelect (hv_X, hv_Index, &hv_X);
  TupleSelect (hv_Y, hv_Index, &hv_Y);
  TupleSelect (hv_Z, hv_Index, &hv_Z);
  GenObjectModel3dFromPoints (hv_X, hv_Y, hv_Z, &hv_ObjectModel3D);
  FitPrimitivesObjectModel3d (
      hv_ObjectModel3D,
      (HTuple ("primitive_type").Append ("fitting_algorithm")),
      (HTuple ("plane").Append ("least_squares_tukey")), &(*hv_Plane));
  return;
}
