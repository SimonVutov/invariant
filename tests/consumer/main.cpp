#include <invariant/invariant.hpp>
float other();
int main() {
    static_assert(INVARIANT_VERSION_MAJOR == 0 && INVARIANT_VERSION_MINOR == 1);
    const invariant::Matrix<double> a(1, 1, 2.0), b(1, 1, 4.0);
    invariant::Matrix<float> p(1, 1, 3.0f);
    return a.solve(b).at(0, 0) == 2 && invariant::eval(p, 1) == other() ? 0 : 1;
}
