#include <invariant/invariant.hpp>
#include "check.hpp"
#include <limits>
#include <random>
using invariant::Matrix;
float other_translation_unit();
int main() {
    Matrix<double> empty;
    throws<std::out_of_range>([&]{ empty.at(0,0); });
    Matrix<double> a(2,2,1.0);
    const auto& ca = a;
    throws<std::out_of_range>([&]{ ca.at(0,2); });
    throws<std::out_of_range>([&]{ a.at(2,0); });
    throws<std::length_error>([]{ Matrix<double> huge(std::numeric_limits<size_t>::max(),2); });
    throws<std::invalid_argument>([]{ Matrix<double> bad(1,1,static_cast<double**>(nullptr)); });
    double* nullrow = nullptr;
    throws<std::invalid_argument>([&]{ Matrix<double> bad(1,1,&nullrow); });
    throws<std::invalid_argument>([]{ Matrix<double> bad(1,1,2.0,1.0); });
    Matrix<int> random(10,10,2,2);
    CHECK(random.sum() == 200);
    auto copied = a; copied.at(0,0)=5;
    CHECK(a.at(0,0)==1);
    auto moved = std::move(copied);
    CHECK(copied.getRows()==0 && copied.getCols()==0);
    copied = moved; CHECK(copied.at(0,0)==5);
    auto& alias = copied;
    copied = alias; CHECK(copied.at(0,0)==5);
    copied = std::move(alias); CHECK(copied.at(0,0)==5);
    copied = std::move(moved); CHECK(moved.getRows()==0);
    throws<std::invalid_argument>([&]{ a.power(-1); });
    CHECK(ca.power(0).at(0,0)==1);
    for (size_t r : {size_t(0),size_t(1),size_t(3)})
        for (size_t inner : {size_t(0),size_t(2),size_t(5)})
            for (size_t c : {size_t(0),size_t(1),size_t(4)}) {
                Matrix<double> left(r,inner), right(inner,c);
                for (size_t i=0;i<r;++i) for (size_t k=0;k<inner;++k) left.at(i,k)=double(i)-double(k);
                for (size_t k=0;k<inner;++k) for (size_t j=0;j<c;++j) right.at(k,j)=double(k)+double(j);
                auto product = left*right;
                CHECK(product.getRows()==r && product.getCols()==c);
                for (size_t i=0;i<r;++i) for (size_t j=0;j<c;++j) {
                    double reference=0;
                    for (size_t k=0;k<inner;++k) reference+=left.at(i,k)*right.at(k,j);
                    CHECK(product.at(i,j)==reference);
                }
            }
    Matrix<float> fa(1,1,2.0f), fb(1,1,3.0f);
    CHECK(fa.solve(fb).at(0,0)==1.5f);
    CHECK(fa.solve_jacobi(fb).at(0,0)==1.5f);
    Matrix<double> rhs(2,1,1.0);
    throws<std::domain_error>([&]{ a.solve(rhs); });
    Matrix<double> zero(2,2);
    throws<std::domain_error>([&]{ zero.solve(rhs); });
    throws<std::invalid_argument>([&]{ empty.solve(empty); });
    throws<std::invalid_argument>([&]{ a.solve(Matrix<double>(3,1)); });
    a.at(0,0)=0; a.at(0,1)=2; a.at(1,0)=3; a.at(1,1)=4;
    Matrix<double> identity(2,2); identity.at(0,0)=identity.at(1,1)=1;
    CHECK((a*a.solve(identity)).two_norm_euclidian_length_difference(identity)<1e-12);
    for (double scale : {1e-200,1e200}) {
        CHECK((a*scale).solve(rhs*scale).two_norm_euclidian_length_difference(a.solve(rhs))<1e-12);
    }
    std::mt19937 rng(42); std::uniform_real_distribution<double> dist(-1,1);
    for (size_t n=1;n<=12;++n) {
        Matrix<double> A(n,n), expected(n,3);
        for (size_t i=0;i<n;++i) {
            for (size_t j=0;j<n;++j) A.at(i,j)=dist(rng)+(i==j?2*n:0);
            for (size_t j=0;j<3;++j) expected.at(i,j)=dist(rng);
        }
        CHECK(A.solve(A*expected).two_norm_euclidian_length_difference(expected)<1e-12);
    }
    Matrix<double> diagonal(2,2); diagonal.at(0,0)=2; diagonal.at(1,1)=4;
    CHECK(diagonal.solve_jacobi(rhs).two_norm_euclidian_length_difference(diagonal.solve(rhs))<1e-12);
    throws<std::invalid_argument>([&]{ diagonal.solve_jacobi(rhs,0); });
    for (double tolerance : {0.0,-1.0,std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN()})
        throws<std::invalid_argument>([&]{ diagonal.solve_jacobi(rhs,10,tolerance); });
    throws<std::invalid_argument>([&]{ zero.solve_jacobi(rhs); });
    Matrix<double> diverging(2,2,2.0); diverging.at(0,0)=diverging.at(1,1)=1;
    throws<std::runtime_error>([&]{ diverging.solve_jacobi(rhs,20); });
    Matrix<double> dd(2,2,1.0); dd.at(0,0)=dd.at(1,1)=4;
    CHECK((dd*dd.solve_jacobi(rhs)).two_norm_euclidian_length_difference(rhs)<2e-10);
    throws<std::runtime_error>([&]{ dd.solve_jacobi(rhs,1,1e-15); });
    diagonal.at(0,0)=std::numeric_limits<double>::infinity();
    throws<std::invalid_argument>([&]{ diagonal.solve(rhs); });
    throws<std::invalid_argument>([&]{ diagonal.solve_jacobi(rhs); });
    Matrix<double> stochastic(2,2,1.0); stochastic.at(0,1)=stochastic.at(1,1)=0;
    auto original = stochastic;
    throws<std::invalid_argument>([&]{ stochastic.makeColumnStochastic(); });
    CHECK(stochastic.two_norm_euclidian_length_difference(original)==0);
    stochastic.at(0,1)=-1;
    throws<std::invalid_argument>([&]{ stochastic.makeColumnStochastic(); });
    Matrix<double> huge(1,1,1e200), origin(1,1);
    CHECK(std::isfinite(huge.two_norm_euclidian_length_difference(origin)));
    const double coefficients[] = {1,2,3};
    CHECK(invariant::polyval_horner(coefficients,2,2.0)==17);
    CHECK(invariant::polyval_horner2(coefficients,2,2.0)==17);
    CHECK(invariant::polyval_O_n_ln_n_rec(coefficients,2,2.0)==17);
    CHECK(invariant::polyval_horner(coefficients,0,2.0)==1);
    CHECK(invariant::polyval_horner2(coefficients,0,2.0)==1);
    CHECK(invariant::pow_O_ln_n_iter(2.0,-3)==0.125);
    CHECK(invariant::pow_O_ln_n_rec(2.0,-3)==0.125);
    CHECK(invariant::pow_O_ln_n_rec(2,3)==8);
    CHECK(invariant::pow_O_ln_n_iter(2,3)==8);
    CHECK(invariant::pow_O_ln_n_rec(1.0,INT_MIN)==1);
    CHECK(invariant::pow_O_ln_n_iter(1.0,INT_MIN)==1);
    throws<std::domain_error>([]{ invariant::pow_O_ln_n_rec(0.0,-1); });
    throws<std::domain_error>([]{ invariant::pow_O_ln_n_iter(2,-1); });
    throws<std::invalid_argument>([]{ invariant::polyval_horner<double>(nullptr,0,1); });
    CHECK(other_translation_unit()==7);
    Matrix<float> polynomial(1,2,1.0f); CHECK(invariant::eval(polynomial,2)==3);
}
