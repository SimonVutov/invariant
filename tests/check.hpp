#pragma once
#include <stdexcept>
#include <string>
#define CHECK(...) do { if (!(__VA_ARGS__)) throw std::runtime_error(std::string("Check failed: ") + #__VA_ARGS__ + " at line " + std::to_string(__LINE__)); } while(false)
template<class E, class F> void throws(F f) {
    bool caught = false;
    try { f(); } catch (const E&) { caught = true; }
    CHECK(caught);
}
