#ifndef UTIL_HPP
#define UTIL_HPP

#include <exception>
#include <utility>
#include <vector>

#ifndef __APPLE__
#include "HalconCpp.h"
#else
#ifndef HC_LARGE_IMAGES
#include <HALCONCpp/HalconCpp.h>
#else
#include <HALCONCppxl/HalconCpp.h>
#endif
#endif

class plane_fit_exception : public std::exception
{
public:
  plane_fit_exception () = delete;
  plane_fit_exception (const char *err_string) : err_string (err_string) {}

  const char *what () const noexcept override;

private:
  const char *err_string;
};

struct roi
{
  std::pair<float, float> center;
  std::pair<float, float> half_len;
  float phi;

  roi () = delete;
  roi (std::pair<float, float> center, std::pair<float, float> half_len,
       float phi);

  bool check ();
  void check_exception ();
};

struct roi_filter_para
{
  std::pair<float, float> val_filter;
  std::pair<float, float> strip_threshold;

  roi_filter_para () = delete;
  roi_filter_para (std::pair<float, float> val_filter,
                   std::pair<float, float> strip_threshold);

  bool check ();
  void check_exception ();
};

HalconCpp::HTuple /* ObjectModel3D */
roi_filter_merge (HalconCpp::HObject &img, std::vector<roi> roi_list,
                  roi_filter_para para);

#endif
