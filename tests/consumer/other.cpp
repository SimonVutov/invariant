#include <invariant/invariant.hpp>
float other() {
    invariant::Matrix<float> p(1, 1, 3.0f);
    return invariant::eval(p, 1);
}
