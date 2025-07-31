#include "essentia_util.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>

namespace essentiautil {

void init() {
    essentia::init();
}

void shutdown() {
    essentia::shutdown();
}

std::vector<essentia::Real> loadAudio(const std::string& filename){
    using namespace essentia::standard;

    AlgorithmFactory& factory = AlgorithmFactory::instance();
    Algorithm* loader = factory.create("MonoLoader",
        "filename", filename,
        "sampleRate", 44100);

    std::vector<essentia::Real> audio;
    loader->output("audio").set(audio);
    loader->compute();
    delete loader;
    return audio;
}

essentia::Real computeDuration(const std::vector<essentia::Real>& audio){
    return static_cast<essentia::Real>(audio.size()) / 44100.0f;
}

std::vector<std::vector<essentia::Real>> computeHPCP(const std::vector<essentia::Real>& audio){
    using namespace essentia::standard;

    AlgorithmFactory& factory = AlgorithmFactory::instance();

    Algorithm* frameCutter = factory.create("FrameCutter",
        "frameSize", 2048,
        "hopSize", 512,
        "startFromZero", true);
    std::vector<essentia::Real> frame;
    frameCutter->input("signal").set(audio);
    frameCutter->output("frame").set(frame);

    Algorithm* window = factory.create("Windowing", "type", "hann");
    std::vector<essentia::Real> windowedFrame;
    window->input("frame").set(frame);
    window->output("frame").set(windowedFrame);

    Algorithm* spectrum = factory.create("Spectrum");
    std::vector<essentia::Real> spectrumFrame;
    spectrum->input("frame").set(windowedFrame);
    spectrum->output("spectrum").set(spectrumFrame);

    Algorithm* peaks = factory.create("SpectralPeaks",
        "maxPeaks", 100,
        "orderBy", "magnitude",
        "magnitudeThreshold", 0.0001);
    std::vector<essentia::Real> frequencies, magnitudes;
    peaks->input("spectrum").set(spectrumFrame);
    peaks->output("frequencies").set(frequencies);
    peaks->output("magnitudes").set(magnitudes);

    Algorithm* hpcp = factory.create("HPCP",
        "size", 12,
        "referenceFrequency", 440.0,
        "bandPreset", false,
        "minFrequency", 100.0,
        "maxFrequency", 5000.0,
        "nonLinear", false,
        "normalized", "none",
        "windowSize", 1.0);
    std::vector<essentia::Real> hpcpFrame;
    hpcp->input("frequencies").set(frequencies);
    hpcp->input("magnitudes").set(magnitudes);
    hpcp->output("hpcp").set(hpcpFrame);

    std::vector<std::vector<essentia::Real>> hpcpMatrix;
    while (true) {
        frameCutter->compute();
        if (frame.empty()) break;
        window->compute();
        spectrum->compute();
        peaks->compute();
        hpcp->compute();
        std::rotate(hpcpFrame.begin(), hpcpFrame.begin() + 3, hpcpFrame.end());
        hpcpMatrix.push_back(hpcpFrame);
    }

    delete frameCutter;
    delete window;
    delete spectrum;
    delete peaks;
    delete hpcp;

    size_t frames = hpcpMatrix.size();
    size_t bins = hpcpMatrix[0].size();
    std::vector<std::vector<essentia::Real>> transposed(bins, std::vector<essentia::Real>(frames));
    for (size_t i = 0; i < bins; i++)
        for (size_t j = 0; j < frames; j++)
            transposed[i][j] = hpcpMatrix[j][i];

    return transposed;
}

void enhance(std::vector<std::vector<essentia::Real>>& matrix){
    using essentia::Real;

    float max_val = -std::numeric_limits<Real>::infinity();
    for (const auto& row : matrix)
        for (float val : row)
            max_val = std::max(max_val, val);
    if (max_val > 0.0f) {
        for (auto& row : matrix)
            for (Real& val : row)
                val /= max_val;
    }

    for (auto& row : matrix)
        for (Real& val : row)
            val = std::log(1000.0f * val + 1.0f);

    Real global_threshold = 0.2f * std::log(1000.0f * 1.0f + 1.0f);
    for (auto& row : matrix)
        for (Real& val : row)
            if (val < global_threshold)
                val = 0.0f;

    int rows = matrix.size(), cols = matrix[0].size();
    std::vector<Real> col_max(cols, 0.0f);
    for (int j = 0; j < cols; ++j)
        for (int i = 0; i < rows; ++i)
            col_max[j] = std::max(col_max[j], matrix[i][j]);

    Real column_threshold = 0.2f * std::log(1000.0f * 1.0f + 1.0f);
    for (int j = 0; j < cols; ++j) {
        if (col_max[j] < column_threshold)
            for (int i = 0; i < rows; ++i)
                matrix[i][j] = 0.0f;
        else
            for (int i = 0; i < rows; ++i)
                matrix[i][j] /= col_max[j];
    }

    int filter_width = 32, half = filter_width / 2;
    std::vector<std::vector<Real>> smoothed = matrix;
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j) {
            std::vector<Real> window;
            for (int k = -half; k <= half; ++k) {
                int idx = j + k;
                if (idx >= 0 && idx < cols)
                    window.push_back(matrix[i][idx]);
            }
            std::nth_element(window.begin(), window.begin() + window.size() / 2, window.end());
            smoothed[i][j] = window[window.size() / 2];
        }
    matrix = smoothed;
}

std::string computeKey(const std::vector<essentia::Real>& audio){
    using namespace essentia::standard;

    AlgorithmFactory& factory = AlgorithmFactory::instance();
    Algorithm* keyExtractor = factory.create("KeyExtractor");

    std::string key, scale;
    essentia::Real strength;

    keyExtractor->input("audio").set(audio);
    keyExtractor->output("key").set(key);
    keyExtractor->output("scale").set(scale);
    keyExtractor->output("strength").set(strength);
    keyExtractor->compute();
    delete keyExtractor;

    return key + " " + scale;
}

void writeCSV(const std::string& filename, const std::vector<std::vector<essentia::Real>>& matrix) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error writing to file: " << filename << std::endl;
        return;
    }
    for (const auto& row : matrix) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];
            if (i < row.size() - 1) file << ",";
        }
        file << "\n";
    }
}

} // namespace keydetection
