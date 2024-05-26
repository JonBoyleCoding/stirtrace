#ifndef IBINARYFILEWRITER_H
#define IBINARYFILEWRITER_H
#include <opencv2/core/core.hpp>
#include <qstring.h>

class IBinaryFileWriter {
  public:
    IBinaryFileWriter() {}
    virtual void writeRawData(cv::Mat img, QString filename, double scale, double time) {}

  private:
};

#endif // IBINARYFILEWRITER_H
