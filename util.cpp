#include "util.hpp"
#include "GenFilterModel3D.hpp"

#include <cmath>
#include <exception>
#include <utility>
#include <vector>

class plane_fit_exception;
struct roi;
struct roi_filter_para;

const char *
plane_fit_exception::what () const noexcept
{
  return err_string;
}

roi::roi (std::pair<float, float> center, std::pair<float, float> half_len,
          float phi)
    : center (center), half_len (half_len), phi (phi)
{
  check_exception ();
}

bool
roi::check ()
{
  return half_len.first > 0 && half_len.second > 0 && -M_PI / 2 < phi
         && phi <= -M_PI / 2;
}

void
roi::check_exception ()
{
  if (!check ())
    throw plane_fit_exception ("Wrong ROI Parameter");
}

roi_filter_para::roi_filter_para (std::pair<float, float> val_filter,
                                  std::pair<float, float> strip_threshold)
    : val_filter (val_filter), strip_threshold (strip_threshold)
{
  check_exception ();
}

bool
roi_filter_para::check ()
{
  return 0 <= val_filter.first && val_filter.first < 0.5
         && 0 <= val_filter.second && val_filter.second < 0.5
         && 0 <= strip_threshold.first && strip_threshold.first < 0.5
         && 0 <= strip_threshold.second && strip_threshold.second < 0.5;
}
void
roi_filter_para::check_exception ()
{
  if (!check ())
    throw plane_fit_exception ("Wrong ROI Filter Parameter");
}

HalconCpp::HTuple /* ObjectModel3D */
roi_filter_merge (const std::string path, std::vector<roi> roi_list,
                  roi_filter_para para)
{
  HalconCpp::HObject img;
  HalconCpp::ReadImage(&img, path.c_str());
  if (roi_list.empty ())
    return HalconCpp::HObjectModel3D ();
  HalconCpp::HTuple merge;
  HalconCpp::HTuple model;
  for (const auto &i : roi_list)
    {
      GenFilterModel3D (img, i.center.first, i.center.second, i.half_len.first,
                        i.half_len.second, i.phi, para.val_filter.first,
                        para.val_filter.second, para.strip_threshold.first,
                        para.strip_threshold.second, &model);
      merge.TupleConcat (model);
    }
  HalconCpp::UnionObjectModel3d (merge, "points_surface", &model);
  return model;
}
