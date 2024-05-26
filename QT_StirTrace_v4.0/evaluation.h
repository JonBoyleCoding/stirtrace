#ifndef EVALUATION_H
#define EVALUATION_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <qstring.h>
#include <qstringlist.h>
#include <vector>

class evaluation {
  private:
    QString EvaluationMode;
    QStringList EvaluationModes;
    cv::Mat subtractLeastSquarePlane(cv::Mat input);
    std::vector<double> benford(cv::Mat input);
    std::vector<double> morphologicFeatures(cv::Mat mat, double scale);
    std::vector<double> nearestNeighborDistances(std::vector<cv::Vec3f> circles, double maxdist);
    std::vector<double> circleAnalyzer(std::vector<cv::Vec3f> circles, cv::Mat input);
    double crystalDetect(cv::Mat mat, int Blocksize, double thresh);

  public:
    evaluation();
    bool setEvalMode(QString EvalMode);
    QStringList getModes();
    void evaluate(cv::Mat input, double scale, QString outputName);
};

#endif // EVALUATION_H
