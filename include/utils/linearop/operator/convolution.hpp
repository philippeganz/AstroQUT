///
/// \file include/utils/linearop/operator/convolution.hpp
/// \brief Convolution class header
/// \details Provide a convolution operator
/// \author Philippe Ganz <philippe.ganz@gmail.com> 2017-2018
/// \version 0.3.0
/// \date 2018-02-25
/// \copyright GPL-3.0
///

#ifndef ASTROQUT_UTILS_OPERATOR_CONVOLUTION_HPP
#define ASTROQUT_UTILS_OPERATOR_CONVOLUTION_HPP

#include "utils/linearop/operator.hpp"

#include <complex>

namespace astroqut
{

template <class T>
class Convolution : public Operator<T>
{
public:

    /** Default constructor
     */
    Convolution()
        : Operator<T>(0, 0)
    {}

    /** Full member constructor
     *  \param data Array containing the filter's data. Needs to be already inverted if not symmetrical.
     *  \param height Height of the filter, must be odd
     *  \param width Width of the filter, must be odd
     */
    Convolution(const Matrix<T>& data, size_t height, size_t width) noexcept
        : Operator<T>(data, height, width, false)
    {
        if( height % 2 != 1 || width % 2 != 1 )
        {
            throw std::invalid_argument("The filter size for the convolution needs to be odd in both dimensions.");
        }
    }

    /** Clone function
     *  \return A copy of the current instance
     */
    Convolution* Clone() const override final
    {
        return new Convolution(*this);
    }

    /** Default destructor
     */
    virtual ~Convolution()
    {}

    /** Valid instance test
     *  \return Throws an error message if instance is not valid.
     */
    bool IsValid() const override final
    {
        if( this->height_ != 0 && this->width_ != 0 &&
            !this->data_.IsEmpty() &&
            this->height_ % 2 == 1 && this->width_ % 2 == 1 )
        {
            return true;
        }
        else
        {
            throw std::invalid_argument("Operator dimensions must be non-zero and function shall not be nullptr!");
        }
    }

    Matrix<T> operator*(const Matrix<T>& other) const override final
    {
        if( !IsValid() || !other.IsValid() )
        {
            throw std::invalid_argument("Can not perform a convolution with these Matrices.");
        }

        Matrix<T> result((T) 0, other.Height(), other.Width());

        size_t height_dist_from_center = (this->height_ - 1) / 2;
        size_t width_dist_from_center = (this->width_ - 1) / 2;

        for( size_t row = 0; row < other.Height(); ++row )
        {
            for( size_t col = 0; col < other.Width(); ++col )
            {
                size_t relative_dist_row = row - height_dist_from_center;
                size_t filter_start_row = relative_dist_row < 0 ? -relative_dist_row : 0;
                size_t matrix_start_row = height_dist_from_center - relative_dist_row;
                for(size_t filter_row = filter_start_row, matrix_row = matrix_start_row;
                    filter_row < std::min(other.Height(), row+height_dist_from_center);
                    ++filter_row, ++matrix_row)
                {
                    size_t relative_dist_col = col - width_dist_from_center;
                    size_t filter_start_col = relative_dist_col < 0 ? -relative_dist_col : 0;
                    size_t matrix_start_col = width_dist_from_center - relative_dist_col;
                    for(size_t filter_col = filter_start_col, matrix_col = matrix_start_col;
                        filter_col < std::min(other.Width(), col+width_dist_from_center);
                        ++filter_col, ++matrix_col)
                    {
                        result[row * other.Width + col] += other[matrix_row * other.Width() + matrix_col] * this->data_[filter_row * this->width_ + filter_col];
                    }
                }
            }
        }
    }
};



} // namespace astroqut

#endif // ASTROQUT_UTILS_OPERATOR_CONVOLUTION_HPP
