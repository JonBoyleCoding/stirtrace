#include "ddrawwriter.h"
#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>

DDRawWriter::DDRawWriter() {}

void DDRawWriter::writeRawData(cv::Mat img, QString filename, double scale, double time) {
    QString intensityFileName = filename + ".bin";
    if (scale > 0) {
        QString metadataFileName = filename + ".dd+";
        QStringList fileparts = filename.split("/");
        QString baseFileName = fileparts.at(fileparts.length() - 1);
        QFile metadataFile(metadataFileName);
        if (metadataFile.open(QFile::ReadWrite)) {
            QTextStream metadataStream(&metadataFile);
            metadataStream << "[general]\r" << endl;
            metadataStream << "Width=" << QString::number(img.cols) << "\r" << endl;
            metadataStream << "Height=" << QString::number(img.rows) << "\r" << endl;
            metadataStream << "AreaWidth=" << QString::number((double)img.cols / scale * 1000000.0, 'f') << "\r"
                           << endl;
            metadataStream << "AreaHeight=" << QString::number((double)img.rows / scale * 1000000.0, 'f') << "\r"
                           << endl;
            metadataStream << "ScaleX=" << QString::number(scale * 1000000000.0, 'f') << "\r" << endl;
            metadataStream << "ScaleY=" << QString::number(scale * 1000000000.0, 'f') << "\r" << endl;
            metadataStream << "StartTime=" << QString::number(time, 'f') << "\r" << endl;
            metadataStream << "ZLengthPerDigitF=1"
                           << "\r" << endl;
            metadataStream << "[buffers]"
                           << "\r" << endl;
            metadataStream << "intensity=" << baseFileName << "_intensity.bin"
                           << "\r" << endl;
            metadataStream << "topography=" << baseFileName << "_intensity.bin"
                           << "\r" << endl;
            metadataStream.flush();
            metadataFile.close();
        }
        intensityFileName = filename + "_intensity.bin";
    }
    img.convertTo(img, CV_32FC1);
    QFile intensityFile(intensityFileName);
    if (intensityFile.open(QFile::ReadWrite)) {
        QDataStream intensityStream(&intensityFile);
        intensityStream.setByteOrder(QDataStream::LittleEndian);
        intensityStream << (qint32)img.cols;
        intensityStream << (qint32)img.rows;
        intensityStream.writeRawData((const char *)img.data, img.rows * img.cols * 4);
        intensityFile.close();
    }
}
