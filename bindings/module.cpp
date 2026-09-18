#include <invariant/invariant.hpp>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;
using invariant::Matrix;

template<class T> using Array = py::array_t<T, py::array::c_style | py::array::forcecast>;
template<class T> Matrix<T> from_array(const Array<T>& array) {
    if (array.ndim() != 2) throw std::invalid_argument("Expected a two-dimensional array.");
    Matrix<T> result(array.shape(0), array.shape(1));
    auto values = array.template unchecked<2>();
    for (py::ssize_t i=0;i<array.shape(0);++i)
        for (py::ssize_t j=0;j<array.shape(1);++j) result.at(i,j)=values(i,j);
    return result;
}
template<class T> py::array_t<T> to_array(const Matrix<T>& matrix) {
    py::array_t<T> result({static_cast<py::ssize_t>(matrix.getRows()), static_cast<py::ssize_t>(matrix.getCols())});
    auto values = result.template mutable_unchecked<2>();
    for (size_t i=0;i<matrix.getRows();++i)
        for (size_t j=0;j<matrix.getCols();++j) values(i,j)=matrix.at(i,j);
    return result;
}
size_t index(py::ssize_t i, size_t length) {
    if (i < 0) i += static_cast<py::ssize_t>(length);
    if (i < 0 || static_cast<size_t>(i) >= length) throw py::index_error("Matrix index out of bounds.");
    return static_cast<size_t>(i);
}
template<class T> void bind_matrix(py::module_& m, const char* name) {
    using M = Matrix<T>;
    py::class_<M>(m,name)
        .def(py::init([](const Array<T>& a){return from_array(a);}), py::arg("array"))
        .def(py::init([](py::ssize_t r, py::ssize_t c, T value){
            if (r<0 || c<0) throw std::invalid_argument("Dimensions must be nonnegative.");
            return M(r,c,value);
        }),py::arg("rows"),py::arg("cols"),py::arg("value")=T{})
        .def_property_readonly("shape",[](const M& a){return py::make_tuple(a.getRows(),a.getCols());})
        .def("numpy",&to_array<T>,"Return an independent NumPy copy.")
        .def("copy",[](const M& a){return M(a);})
        .def("__getitem__",[](const M& a,std::pair<py::ssize_t,py::ssize_t> ij){return a.at(index(ij.first,a.getRows()),index(ij.second,a.getCols()));})
        .def("__setitem__",[](M& a,std::pair<py::ssize_t,py::ssize_t> ij,T value){a.at(index(ij.first,a.getRows()),index(ij.second,a.getCols()))=value;})
        .def("__matmul__",[](const M& a,const M& b){return a*b;},py::is_operator())
        .def("__add__",&M::Add,py::is_operator())
        .def("__sub__",&M::Subtract,py::is_operator())
        .def("__mul__",[](const M& a,T scale){return a*scale;},py::is_operator())
        .def("__rmul__",[](const M& a,T scale){return a*scale;},py::is_operator())
        .def_property_readonly("T",&M::transpose)
        .def("transpose",&M::transpose)
        .def("solve",&M::solve)
        .def("least_squares",&M::least_squares)
        .def("solve_jacobi",&M::solve_jacobi,py::arg("b"),py::arg("max_iterations")=10000,py::arg("tolerance")=1e-10)
        .def("power",&M::power)
        .def("reciprocal",&M::reciprocal)
        .def("make_column_stochastic",&M::makeColumnStochastic)
        .def("sum",&M::sum)
        .def("difference_norm",&M::two_norm_euclidian_length_difference)
        .def("__repr__",[name](const M& a){return std::string(name)+"("+a.dim()+")";});
    const std::string suffix = std::is_same_v<T,float> ? "_f32" : "_f64";
    m.def(("compute"+suffix).c_str(),[](const Array<T>& aa,const Array<T>& bb,const std::string& operation,size_t iterations,double tolerance){
        auto a=from_array(aa), b=from_array(bb);
        M result;
        {
            py::gil_scoped_release release;
            if (operation=="matmul") result=a*b;
            else if (operation=="solve") result=a.solve(b);
            else if (operation=="least_squares") result=a.least_squares(b);
            else if (operation=="jacobi") result=a.solve_jacobi(b,iterations,tolerance);
            else throw std::invalid_argument("Unknown operation.");
        }
        return to_array(result);
    });
}
PYBIND11_MODULE(_core,m) {
    m.attr("__version__")=INVARIANT_VERSION;
    bind_matrix<double>(m,"Matrix");
    bind_matrix<float>(m,"MatrixFloat");
    m.def("polyval",[](const Array<double>& coefficients,double x){
        if (coefficients.ndim()!=1 || coefficients.size()==0) throw std::invalid_argument("Expected nonempty 1D coefficients.");
        double value=0;
        for(py::ssize_t i=coefficients.size();i-- >0;) value=value*x+coefficients.data()[i];
        return value;
    });
}
