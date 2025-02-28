#include "../diffurch.hpp"
#include "../src/util/postprocessing.hpp"
#include "../src/util/print.hpp"
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <iterator>
#include <matplot/util/common.h>
#include <tuple>

#include <matplot/matplot.h>

using namespace diffurch;

int main(int, char *[]) {

  equation::Lorenz eq;

  auto sol = eq.solution(0, 10000, ConstantStepsize(0.01), std::make_tuple());
  auto [t, x, y, z] = sol;

  /*using namespace matplot;*/
  /*plot(t, x)->line_width(2.).color("red");*/
  /*hold(on);*/
  /*plot(t, y)->line_width(2.).color("blue");*/
  /*plot(t, z)->line_width(2.).color("green");*/
  /*show();*/
  {
    std::ofstream fout("../lorenz.ts.dat");

    fout << "# Lorenz Equation time series" << "\n";
    fout << "# sigma = " << eq.sigma << "\n";
    fout << "# rho = " << eq.sigma << "\n";
    fout << "# beta = " << eq.sigma << "\n";
    fout << "# method: rk98" << "\n";

    fout << "t x y z" << "\n";

    std::apply(
        [&](const auto &...v) {
          for (int i = 0; i < t.size(); i++) {
            ((fout << v[i] << " "), ...);
            fout << "\n";
          }
        },
        sol);
  }

  auto [x_bins, y_bins, hist_values] =
      hist2(matplot::transform(x, y, std::plus{}), z, 1000);

  { // task: save image raw data, then convert it to a bitmap (with appropriate
    // colormap), then draw it in pgfplots (with correct bounds)
    std::ofstream fout("../lorenz.hist.dat");
    std::cout << "histrogram output started" << std::endl;
    fout << "# Lorenz attractor 2d histogram" << "\n";
    fout << "# sigma = " << eq.sigma << "\n";
    fout << "# rho = " << eq.rho << "\n";
    fout << "# beta = " << eq.beta << "\n";
    fout << "# method: rk98" << "\n";

    fout << "x+y z c" << "\n";
    for (size_t i = 0; i < x_bins.size(); i++) {

      for (size_t j = 0; j < y_bins.size(); j++) {
        fout << x_bins[i] << " ";
        fout << y_bins[j] << " ";
        fout << hist_values[i][j] << "\n";
      }
      fout << "\n";
    }
  }

  {
    std::ofstream fout("../lorenz.params.dat");
    fout << "sigma = " << eq.sigma << ",\n";
    fout << "rho = " << eq.rho << ",\n";
    fout << "beta = " << eq.beta << ",\n";
    fout << "xmin = " << x_bins[0] << ",\n";
    fout << "xmax = " << x_bins[x_bins.size() - 1] << ",\n";
    fout << "ymin = " << y_bins[0] << ",\n";
    fout << "ymax = " << y_bins[y_bins.size() - 1] << ",\n";
  }
  { // task: save image raw data, then convert it to a bitmap (with appropriate
    // colormap), then draw it in pgfplots (with correct bounds)
    std::ofstream fout("../lorenz.hist.matrix.dat");
    std::cout << "histrogram output started" << std::endl;
    fout << "# Lorenz attractor 2d histogram" << "\n";
    fout << "# sigma = " << eq.sigma << "\n";
    fout << "# rho = " << eq.rho << "\n";
    fout << "# beta = " << eq.beta << "\n";
    fout << "# method: rk98" << "\n";

    fout << x_bins.size() << " ";
    for (size_t i = 0; i < x_bins.size(); i++) {
      fout << x_bins[i] << " ";
    }
    fout << "\n";
    for (size_t j = 0; j < y_bins.size(); j++) {
      fout << y_bins[j] << " ";
      for (size_t i = 0; i < x_bins.size(); i++) {
        fout << hist_values[i][j] << " ";
      }
      fout << "\n";
    }
  }

  return 0;
}
