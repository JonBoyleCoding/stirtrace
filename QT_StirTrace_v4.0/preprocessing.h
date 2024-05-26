#ifndef PREPROCESSING_H
#define PREPROCESSING_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <qstring.h>
#include <qstringlist.h>

class preprocessing {
  private:
    QString PreprocessingMode;
    QStringList PreprocessingModes;

  public:
    preprocessing();
    bool setMode(QString PreProcessMode);
    QStringList getModes();
    cv::Mat preprocess(cv::Mat input);
};

#endif // PREPROCESSING_H
