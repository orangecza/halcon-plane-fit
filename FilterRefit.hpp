#ifndef FILTERREFIT_HPP
#define FILTERREFIT_HPP

#ifndef __APPLE__
#include "HalconCpp.h"
#else
#ifndef HC_LARGE_IMAGES
#include <HALCONCpp/HalconCpp.h>
#else
#include <HALCONCppxl/HalconCpp.h>
#endif
#endif

// Procedure declarations
// Chapter: 3D Object Model / Features
// Short Description: This procedure first fit a plane using all input points.
// Then A percentage of points is stripped which have the nearest and farthest
// distance to the plane. Finally a new plane is fitted by the remaining
// points.
extern void FilterRefit (HalconCpp::HTuple hv_ObjectModel3D, HalconCpp::HTuple hv_NearestFilter,
                         HalconCpp::HTuple hv_FarthestFilter, HalconCpp::HTuple *hv_Plane);

#endif
