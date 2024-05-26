#include "preprocessing.h"

preprocessing::preprocessing() {}

bool preprocessing::setMode(QString PreProcessMode) {
    if (this->PreprocessingModes.indexOf(PreProcessMode) > 0) {
        this->PreprocessingMode = PreProcessMode;
        return true;
    }
    return false;
}

cv::Mat preprocessing::preprocess(cv::Mat input) {
    // add preprocessing code here
    return input;
}

QStringList preprocessing::getModes() {
    return this->PreprocessingModes;
}
