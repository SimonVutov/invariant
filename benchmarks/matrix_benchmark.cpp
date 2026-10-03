#include <invariant/invariant.hpp>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <random>
#include <vector>

using invariant::Matrix;
volatile double checksum = 0;
template<class F> double median_ms(F operation) {
    checksum = operation().sum(); // Warm up; materialize and consume every result.
    std::vector<double> times;
    for (int repeat = 0; repeat < 7; ++repeat) {
        const auto start = std::chrono::steady_clock::now();
        auto result = operation();
        const auto end = std::chrono::steady_clock::now();
        checksum = result.sum(); // Exclude the checksum from the timing.
        times.push_back(std::chrono::duration<double, std::milli>(end-start).count());
    }
    std::sort(times.begin(),times.end());
    return times[times.size()/2];
}
Matrix<double> random_matrix(size_t r, size_t c, std::mt19937& rng) {
    Matrix<double> result(r,c);
    std::uniform_real_distribution<double> dist(-1,1);
    for (size_t i=0;i<r;++i) for (size_t j=0;j<c;++j) result.at(i,j)=dist(rng);
    return result;
}
int main() {
    std::mt19937 rng(42);
    std::cout << "operation,rows,inner,cols,median_ms,relative_error\n" << std::setprecision(9);
    for (size_t n : {32,128,256}) {
        const auto a=random_matrix(n,n,rng), b=random_matrix(n,n,rng);
        Matrix<double> reference(n,n);
        for (size_t i=0;i<n;++i) for (size_t j=0;j<n;++j) {
            long double sum=0;
            for (size_t k=0;k<n;++k) sum+=static_cast<long double>(a.at(i,k))*b.at(k,j);
            reference.at(i,j)=static_cast<double>(sum);
        }
        const double error=(a*b).two_norm_euclidian_length_difference(reference) /
            reference.two_norm_euclidian_length_difference(Matrix<double>(n,n));
        if (!std::isfinite(error) || error>1e-12) return 1;
        std::cout << "matmul," << n << ',' << n << ',' << n << ',' << median_ms([&]{return a*b;}) << ',' << error << '\n';
    }
    const auto weights=random_matrix(128,784,rng), inputs=random_matrix(784,32,rng);
    std::cout << "dense_batch_matmul,128,784,32," << median_ms([&]{return weights*inputs;}) << ",\n";
    auto a=random_matrix(128,128,rng);
    for(size_t i=0;i<128;++i) a.at(i,i)+=256;
    const auto expected=random_matrix(128,4,rng), b=a*expected;
    const double error=a.solve(b).two_norm_euclidian_length_difference(expected) /
        expected.two_norm_euclidian_length_difference(Matrix<double>(128,4));
    if (!std::isfinite(error) || error>1e-12) return 1;
    std::cout << "solve,128,128,4," << median_ms([&]{return a.solve(b);}) << ',' << error << '\n';
}
