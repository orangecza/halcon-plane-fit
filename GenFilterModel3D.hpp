#ifndef GENFILTERMODEL3D_HPP
#define GENFILTERMODEL3D_HPP

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
// Chapter: 3D Object Model / Creation
// Short Description: This procedure first filters points of which Z value is
// exceeded. Then points is sorted by Z values and stripped by a percent. The
// final output is a 3D model.
extern void GenFilterModel3D (
    HalconCpp::HObject ho_Image, HalconCpp::HTuple hv_ROICenterRow,
    HalconCpp::HTuple hv_ROICenterColumn, HalconCpp::HTuple hv_ROIHalfLength,
    HalconCpp::HTuple hv_ROIHalfWidth, HalconCpp::HTuple hv_Phi,
    HalconCpp::HTuple hv_FilterLower, HalconCpp::HTuple hv_FilterUpper,
    HalconCpp::HTuple hv_StripThresholdLower, HalconCpp::HTuple hv_StripThresholdUpper, HalconCpp::HTuple *__restrict hv_ObjectModel3D);

#endif
