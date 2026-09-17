#include <invariant/invariant.hpp>
#include <iostream>
int main() {
    invariant::Matrix<double> a(2,2), b(2,1);
    a.at(0,0)=4; a.at(0,1)=1;
    a.at(1,0)=1; a.at(1,1)=3;
    b.at(0,0)=1; b.at(1,0)=2;
    const auto x=a.solve(b);
    std::cout << "Solution:\n";
    x.print();
    const double residual=(a*x).two_norm_euclidian_length_difference(b);
    std::cout << "Residual: " << residual << '\n';
    if (residual > 1e-12) return 1;
    try {
        invariant::Matrix<double>(2,2,1.0).solve(b);
        return 1; // This singular system must be rejected.
    } catch (const std::domain_error& error) {
        std::cout << "Expected singular-system error: " << error.what() << '\n';
    }
}
