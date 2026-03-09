// Python bindings for OpenPIV C++ using pybind11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <fstream>
#include <vector>
#include <tuple>
#include <memory>

#include "openpiv/core/image.h"
#include "openpiv/core/grid.h"
#include "openpiv/core/image_utils.h"
#include "openpiv/algos/fft.h"
#include "openpiv/algos/pocket_fft.h"
#include "openpiv/loaders/image_loader.h"

namespace py = pybind11;
using namespace openpiv;

/// Cross-correlate two images and return velocity vectors
py::dict cross_correlate(
    const std::string& file1, 
    const std::string& file2,
    int size = 32, 
    double overlap = 0.5,
    const std::string& fft_type = "complex") 
{
    // Load images
    std::ifstream is1(file1, std::ios::binary);
    std::ifstream is2(file2, std::ios::binary);
    
    if (!is1.is_open()) {
        throw std::runtime_error("Cannot open file: " + file1);
    }
    if (!is2.is_open()) {
        throw std::runtime_error("Cannot open file: " + file2);
    }
    
    auto loader = core::image_loader_registry::find(is1);
    if (!loader) {
        throw std::runtime_error("Cannot find image loader for: " + file1);
    }
    
    core::gf_image img1, img2;
    loader->load(is1, img1);
    loader->load(is2, img2);
    
    if (img1.size() != img2.size()) {
        throw std::runtime_error("Image sizes don't match");
    }
    
    // Create grid
    auto ia = core::size{static_cast<unsigned int>(size), static_cast<unsigned int>(size)};
    auto grid = core::generate_cartesian_grid(img1.size(), ia, overlap);
    
    // Select FFT method - create new instance for each call
    using correlator_t = std::function<core::gf_image(const core::gf_image&, const core::gf_image&)>;
    std::unique_ptr<algos::FFT> fft_complex;
    std::unique_ptr<algos::PocketFFT> fft_pocket;
    correlator_t correlator;
    
    try {
        if (fft_type == "complex" || fft_type == "real") {
            fft_complex = std::make_unique<algos::FFT>(ia);
            if (fft_type == "complex") {
                correlator = [ptr = fft_complex.get()](const core::gf_image& a, const core::gf_image& b) {
                    return ptr->cross_correlate(a, b);
                };
            } else {
                correlator = [ptr = fft_complex.get()](const core::gf_image& a, const core::gf_image& b) {
                    return ptr->cross_correlate_real(a, b);
                };
            }
        } else if (fft_type == "pocket" || fft_type == "pocket_real") {
            fft_pocket = std::make_unique<algos::PocketFFT>(ia);
            if (fft_type == "pocket") {
                correlator = [ptr = fft_pocket.get()](const core::gf_image& a, const core::gf_image& b) {
                    return ptr->cross_correlate(a, b);
                };
            } else {
                correlator = [ptr = fft_pocket.get()](const core::gf_image& a, const core::gf_image& b) {
                    return ptr->cross_correlate_real(a, b);
                };
            }
        } else {
            throw std::runtime_error("Unknown FFT type: " + fft_type);
        }
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("FFT initialization failed: ") + e.what());
    }
    
    // Process each interrogation area
    std::vector<std::tuple<double, double, double, double, double>> results;
    results.reserve(grid.size());
    
    for (const auto& rect : grid) {
        auto view1 = core::extract(img1, rect);
        auto view2 = core::extract(img2, rect);
        auto corr = correlator(view1, view2);
        
        // Find peaks
        auto peaks = core::find_peaks(corr, 2, 1);
        if (peaks.size() >= 2 && peaks[1][{1, 1}] > 0) {
            auto loc = core::fit_simple_gaussian(peaks[0]);
            double sn = peaks[0][{1, 1}] / peaks[1][{1, 1}];
            
            auto bl = rect.bottomLeft();
            auto mid = rect.midpoint();
            double u = mid[0] - (bl[0] + loc[0]);
            double v = mid[1] - (bl[1] + loc[1]);
            
            // Convert Y to image coordinates
            double y = img1.height() - mid[1];
            
            results.emplace_back(mid[0], y, u, v, sn);
        }
    }
    
    // Convert to numpy array
    std::vector<ssize_t> shape{static_cast<ssize_t>(results.size()), 5};
    py::array_t<double> arr(shape);
    auto buf = arr.mutable_unchecked<2>();
    
    for (size_t i = 0; i < results.size(); i++) {
        buf(i, 0) = std::get<0>(results[i]);  // x
        buf(i, 1) = std::get<1>(results[i]);  // y
        buf(i, 2) = std::get<2>(results[i]);  // u
        buf(i, 3) = std::get<3>(results[i]);  // v
        buf(i, 4) = std::get<4>(results[i]);  // sn
    }
    
    // Build result dictionary
    py::dict result;
    result["vectors"] = arr;
    result["count"] = static_cast<int>(results.size());
    result["image_size"] = std::vector<int>{static_cast<int>(img1.width()), static_cast<int>(img1.height())};
    result["ia_size"] = size;
    result["overlap"] = overlap;
    
    return result;
}

/// Simple PIV processing that returns structured data
py::dict piv_process(
    const std::string& file1,
    const std::string& file2,
    int size = 32,
    double overlap = 0.5)
{
    auto result = cross_correlate(file1, file2, size, overlap);
    
    // Extract vectors
    auto vectors = result["vectors"].cast<py::array_t<double>>();
    
    // Calculate statistics
    double min_speed = 1e10, max_speed = 0, sum_speed = 0;
    double min_sn = 1e10, max_sn = 0, sum_sn = 0;
    
    auto buf = vectors.unchecked<2>();
    ssize_t n = buf.shape(0);
    
    for (ssize_t i = 0; i < n; i++) {
        double u = buf(i, 2);
        double v = buf(i, 3);
        double speed = std::sqrt(u*u + v*v);
        double sn = buf(i, 4);
        
        min_speed = std::min(min_speed, speed);
        max_speed = std::max(max_speed, speed);
        sum_speed += speed;
        
        min_sn = std::min(min_sn, sn);
        max_sn = std::max(max_sn, sn);
        sum_sn += sn;
    }
    
    py::dict stats;
    if (n > 0) {
        stats["min_speed"] = min_speed;
        stats["max_speed"] = max_speed;
        stats["mean_speed"] = sum_speed / static_cast<double>(n);
        stats["min_sn"] = min_sn;
        stats["max_sn"] = max_sn;
        stats["mean_sn"] = sum_sn / static_cast<double>(n);
    } else {
        // No vectors found: avoid division by zero and return neutral stats
        stats["min_speed"] = 0.0;
        stats["max_speed"] = 0.0;
        stats["mean_speed"] = 0.0;
        stats["min_sn"] = 0.0;
        stats["max_sn"] = 0.0;
        stats["mean_sn"] = 0.0;
    }
    stats["vector_count"] = n;
    
    result["stats"] = stats;
    
    return result;
}

PYBIND11_MODULE(openpiv_cpp, m) {
    m.doc() = "OpenPIV C++ - Fast Particle Image Velocimetry";
    
    m.def("cross_correlate", &cross_correlate,
          py::arg("file1"), py::arg("file2"),
          py::arg("size") = 32,
          py::arg("overlap") = 0.5,
          py::arg("fft_type") = "complex",
          "Cross-correlate two images and return velocity vectors");
    
    m.def("piv_process", &piv_process,
          py::arg("file1"), py::arg("file2"),
          py::arg("size") = 32,
          py::arg("overlap") = 0.5,
          "Process PIV image pair with statistics");
}
