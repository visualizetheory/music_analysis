#pragma once

#include <string>
#include <vector>
#include <essentia/essentia.h>
#include <essentia/algorithmfactory.h>

namespace essentiautil {

    // Use fully qualified types from Essentia
    using Real = essentia::Real;

    void init();               // Must be called before any algorithm
    void shutdown();           // Call when you're done

    std::vector<Real> loadAudio(const std::string& filename);
    Real computeDuration(const std::vector<Real>& audio);
    std::vector<std::vector<Real>> computeHPCP(const std::vector<Real>& audio);
    void enhance(std::vector<std::vector<Real>>& matrix);
    std::string computeKey(const std::vector<Real>& audio);
    void writeCSV(const std::string& filename, const std::vector<std::vector<Real>>& matrix);

}
