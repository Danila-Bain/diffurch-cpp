#pragma once

#include "vec.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <vector>

namespace diffurch {

auto hist(const std::vector<double> &x, size_t bins_n = 100,
          bool normalize = false) {

  double x_min = *std::min_element(x.begin(), x.end());
  double x_max = *std::max_element(x.begin(), x.end());

  std::vector<double> bins = linspace(x_min, x_max, bins_n + 1);

  std::vector<double> bin_centers(bins_n);
  std::transform(bins.begin() + 1, bins.end(), bins.begin(),
                 bin_centers.begin(),
                 [](double a, double b) { return 0.5 * (a + b); });

  std::vector<double> bin_values(bins_n, 0.);

  for (const auto &e : x) {
    auto ptr = std::lower_bound(bins.begin(), bins.end(), e);
    auto index = std::distance(bins.begin(), ptr);
    bin_values[index - (e != x_min)]++;
  }

  if (normalize) {
    double factor = double(bins_n) / double(x.size());
    std::transform(bin_values.begin(), bin_values.end(), bin_values.begin(),
                   [factor](double x_) { return x_ * factor; });
  }

  return std::make_tuple(bin_centers, bin_values);
}

auto hist2(const std::vector<double> &x, const std::vector<double> &y,
           size_t bins_n = 100, bool normalize = false) {

  assert(x.size() == y.size() &&
         "in hist2, arguments x and y have to have the same size");
  double x_min = *std::min_element(x.begin(), x.end());
  double x_max = *std::max_element(x.begin(), x.end());

  double y_min = *std::min_element(y.begin(), y.end());
  double y_max = *std::max_element(y.begin(), y.end());

  std::vector<double> x_bins = linspace(x_min, x_max, bins_n + 1);
  std::vector<double> y_bins = linspace(y_min, y_max, bins_n + 1);

  std::vector<double> x_bin_centers(bins_n);
  std::vector<double> y_bin_centers(bins_n);

  std::transform(x_bins.begin() + 1, x_bins.end(), x_bins.begin(),
                 x_bin_centers.begin(),
                 [](double a, double b) { return 0.5 * (a + b); });
  std::transform(y_bins.begin() + 1, y_bins.end(), y_bins.begin(),
                 y_bin_centers.begin(),
                 [](double a, double b) { return 0.5 * (a + b); });

  std::vector<std::vector<double>> bin_values(
      bins_n, std::vector<double>(bins_n, 0)); // not best

  for (size_t i = 0; i < x.size(); i++) {
    auto x_ptr = std::lower_bound(x_bins.begin(), x_bins.end(), x[i]);
    auto x_index = std::distance(x_bins.begin(), x_ptr);
    auto y_ptr = std::lower_bound(y_bins.begin(), y_bins.end(), y[i]);
    auto y_index = std::distance(y_bins.begin(), y_ptr);
    bin_values[x_index - (x[i] != x_min)][y_index - (y[i] != y_min)]++;
  }

  if (normalize) {
    double factor = double(bins_n * bins_n) / double(x.size());
    for (auto &e1 : bin_values) {
      for (auto &e2 : e1) {
        e2 *= factor;
      }
    }
  }

  return std::make_tuple(x_bin_centers, y_bin_centers, bin_values);
}

} // namespace diffurch
