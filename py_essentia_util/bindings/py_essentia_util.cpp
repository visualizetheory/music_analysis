#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "essentia_util.h"

namespace py = pybind11;

// Wrapper to load and process audio in one go
std::string compute_key_from_file(const std::string& filename) {
    essentiautil::init();
    std::vector<essentia::Real> audio = essentiautil::loadAudio(filename);
    std::string key = essentiautil::computeKey(audio);
    essentiautil::shutdown();
    return key;
}

PYBIND11_MODULE(py_essentia_util, m) {
    m.doc() = "Python bindings for essentiautil";

    m.def("compute_key_from_file", &compute_key_from_file,
          py::arg("filename"),
          "Compute the key of an audio file");

    m.def("compute_key", &essentiautil::computeKey,
          py::arg("audio"),
          "Compute the key from an audio buffer");
}
