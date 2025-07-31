#include "essentia_util.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " audio.wav\n";
        return 1;
    }

    essentiautil::init();
    std::string filename = argv[1];
    std::vector<essentia::Real> audio = essentiautil::loadAudio(filename);

    std::cout << "Duration: " << essentiautil::computeDuration(audio) << " seconds" << std::endl;

    auto hpcpMatrix = essentiautil::computeHPCP(audio);
    essentiautil::enhance(hpcpMatrix);
    essentiautil::writeCSV("output/hpcp.csv", hpcpMatrix);

    std::string key = essentiautil::computeKey(audio);
    std::cout << "Detected key: " << key << std::endl;

    essentiautil::shutdown();
    return 0;
}
