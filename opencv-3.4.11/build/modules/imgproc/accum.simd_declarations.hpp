#define CV_CPU_SIMD_FILENAME "/Users/parkgeonwoo/Documents/document/2022-2/ComputerVision/opencv-3.4.11/modules/imgproc/src/accum.simd.hpp"
#define CV_CPU_DISPATCH_MODE AVX
#include "opencv2/core/private/cv_cpu_include_simd_declarations.hpp"

#define CV_CPU_DISPATCH_MODE AVX2
#include "opencv2/core/private/cv_cpu_include_simd_declarations.hpp"

#define CV_CPU_DISPATCH_MODES_ALL AVX2, AVX, BASELINE

#undef CV_CPU_SIMD_FILENAME
