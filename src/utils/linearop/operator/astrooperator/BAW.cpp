///
/// \file src/utils/linearop/operator/astrooperator/BAW.cpp
/// \brief Forward astro transform
/// \author Philippe Ganz <philippe.ganz@gmail.com> 2017-2018
/// \version 0.4.0
/// \date 2018-06-02
/// \copyright GPL-3.0
///

#include "utils/linearop/operator/astrooperator.hpp"

namespace astroqut
{

Matrix<long double> AstroOperator::BAW(const Matrix<long double> source,
                                       bool apply_wavelet,
                                       bool apply_spline,
                                       bool ps ) const
{
    Matrix<long double> normalized_source = source / standardise_;

    // split the normalized source into wavelet, spline and ps components
    Matrix<long double> source_wavelet(&normalized_source[0], pic_size_, 1);
    Matrix<long double> source_spline(&normalized_source[pic_size_], pic_size_, 1);
    Matrix<long double> source_ps(&normalized_source[2*pic_size_], pic_size_, pic_size_);

    // W * xw
    Matrix<long double> result_wavelet;
    if( apply_wavelet )
        result_wavelet = wavelet_ * source_wavelet;

    // W * xs
    Matrix<long double> result_spline;
    if( apply_spline )
        result_spline = spline_ * source_spline;

    // A * (Wxw + Wxs)
    Matrix<long double> result = abel_ * (result_wavelet + result_spline);
    result.Height(pic_size_);
    result.Width(pic_size_);

    // AWx + ps
    if( ps )
        result += source_ps;

    // B(AWx + ps)
    result = blur_ * result;
    result.Height(pic_size_*pic_size_);
    result.Width(1);

    // E' .* B(AWx + ps)
    result = result & sensitivity_;

    // release pointers
    source_wavelet.Data(nullptr);
    source_spline.Data(nullptr);
    source_ps.Data(nullptr);

    return result;
}

} // namespace astroqut
