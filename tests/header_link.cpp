#include <invariant/Polynomial.hpp>
float other_translation_unit() {
    invariant::Matrix<float> p(1,3,1.0f);
    return invariant::eval(p,2);
}
