#include <invariant/invariant.hpp>
#include "check.hpp"
using invariant::Matrix;
int main() {
    Matrix<double> a(8,3), expected(3,2);
    for(size_t i=0;i<8;++i) {
        const double x=double(i)/7;
        a.at(i,0)=1; a.at(i,1)=x; a.at(i,2)=x*x;
    }
    for(size_t i=0;i<3;++i) for(size_t j=0;j<2;++j) expected.at(i,j)=double(i+j+1);
    auto b=a*expected;
    CHECK(a.least_squares(b).two_norm_euclidian_length_difference(expected)<1e-12);
    b.at(0,0)+=0.1;
    auto solution=a.least_squares(b);
    auto residual=(a*solution).Subtract(b);
    CHECK((a.transpose()*residual).two_norm_euclidian_length_difference(Matrix<double>(3,2))<1e-12);
    for(double scale:{1e-100,1e100})
        CHECK((a*scale).least_squares(b*scale).two_norm_euclidian_length_difference(solution)<1e-12);
    throws<std::domain_error>([]{Matrix<double>(3,2,1.0).least_squares(Matrix<double>(3,1));});
    throws<std::invalid_argument>([]{Matrix<double>(2,3).least_squares(Matrix<double>(2,1));});
    throws<std::invalid_argument>([]{Matrix<double>(2,1).least_squares(Matrix<double>(3,1));});
    throws<std::invalid_argument>([]{Matrix<double>().least_squares(Matrix<double>());});
    Matrix<float> f(3,1,1.0f), y(3,1); y.at(0,0)=1;y.at(1,0)=2;y.at(2,0)=3;
    CHECK(std::abs(f.least_squares(y).at(0,0)-2)<1e-6);
}
