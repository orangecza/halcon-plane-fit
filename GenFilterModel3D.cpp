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

#include "GenFilterModel3D.hpp"

using namespace HalconCpp;

// Procedures
// Chapter: 3D Object Model / Creation
// Short Description: This procedure first filters points of which Z value is
// exceeded. Then points is sorted by Z values and stripped by a percent. The
// final output is a 3D model.
void
GenFilterModel3D (HObject ho_Image, HTuple hv_ROICenterRow,
                  HTuple hv_ROICenterColumn, HTuple hv_ROIHalfLength,
                  HTuple hv_ROIHalfWidth, HTuple hv_Phi, HTuple hv_FilterLower,
                  HTuple hv_FilterUpper, HTuple hv_StripThresholdLower, HTuple hv_StripThresholdUpper,
                  HTuple *hv_ObjectModel3D)
{
  // Local iconic variables
  HObject ho_Image1, ho_Image2, ho_Image3, ho_ROI;
  HObject ho_ImageReduced, ho_ImageReduced1, ho_ImageReduced2;

  // Local control variables
  HTuple hv_X, hv_Y, hv_Z, hv_Len, hv_Index, hv_ZSortedIndex;
  HTuple hv_InlierIndex;

  Decompose3 (ho_Image, &ho_Image1, &ho_Image2, &ho_Image3);
  GenRectangle2 (&ho_ROI, hv_ROICenterRow, hv_ROICenterColumn, hv_Phi,
                 hv_ROIHalfLength, hv_ROIHalfWidth);
  ReduceDomain (ho_Image1, ho_ROI, &ho_ImageReduced);
  ReduceDomain (ho_Image2, ho_ROI, &ho_ImageReduced1);
  ReduceDomain (ho_Image3, ho_ROI, &ho_ImageReduced2);
  // Get Tuple and 3D model
  XyzToObjectModel3d (ho_ImageReduced, ho_ImageReduced1, ho_ImageReduced2,
                      &(*hv_ObjectModel3D));
  SelectPointsObjectModel3d ((*hv_ObjectModel3D), "point_coord_z",
                             hv_FilterLower, hv_FilterUpper,
                             &(*hv_ObjectModel3D));
  GetObjectModel3dParams ((*hv_ObjectModel3D), "point_coord_x", &hv_X);
  GetObjectModel3dParams ((*hv_ObjectModel3D), "point_coord_y", &hv_Y);
  GetObjectModel3dParams ((*hv_ObjectModel3D), "point_coord_z", &hv_Z);
  // Sample Points
  hv_Len = (hv_Z.TupleLength ()) - 1;
  hv_Index = HTuple::TupleGenSequence (0, hv_Len, 2);
  hv_X = hv_X.TupleSelect (hv_Index);
  hv_Y = hv_Y.TupleSelect (hv_Index);
  hv_Z = hv_Z.TupleSelect (hv_Index);
  // Strip outliers
  hv_ZSortedIndex = hv_Z.TupleSortIndex ();
  hv_InlierIndex = HTuple::TupleGenSequence (
      ((hv_ZSortedIndex.TupleLength ()) * hv_StripThresholdLower).TupleInt (),
      (((hv_ZSortedIndex.TupleLength ()) * (1 - hv_StripThresholdUpper))
           .TupleInt ())
          - 1,
      1);
  hv_InlierIndex = hv_ZSortedIndex.TupleSelect (hv_InlierIndex);
  hv_X = hv_X.TupleSelect (hv_InlierIndex);
  hv_Y = hv_Y.TupleSelect (hv_InlierIndex);
  hv_Z = hv_Z.TupleSelect (hv_InlierIndex);
  GenObjectModel3dFromPoints (hv_X, hv_Y, hv_Z, &(*hv_ObjectModel3D));
  return;
}
