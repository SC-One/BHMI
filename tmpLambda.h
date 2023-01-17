#ifndef TMP_LAMBDA_H_FILE
#define TMP_LAMBDA_H_FILE

#include <BHMI.h>

#include <QByteArray>
#include <QDebug>
#include <QStringList>

struct Holder {
  QByteArray wholeData;

  Holder(BHMI* bhmi) : _bhmi(bhmi) {}
  void doThat(QByteArray const& ba) {
    Structures::DataOverSerial data;
    //    float newBucketTemp;
    //    float cameraOnTemp;

    if (ba.indexOf(";") != -1) {
      wholeData += ba.mid(0, ba.indexOf(";") + 1);
      {
        auto listInt = ba.split(',');
        if (auto x = listInt.last().indexOf(";"); x != -1) {
          listInt.last().remove(x, 1);
        }

        data.s1 = listInt[0].toInt();
        data.s2 = listInt[1].toInt();
        data.rawPump = listInt[2].toInt();
        data.newBucket = listInt[3].toInt();
        data.cameraOn = listInt[4].toInt();
        _bhmi->setLastData(data);
      }
      wholeData = ba.mid(ba.indexOf(";"));
    } else {
      wholeData += ba;
    }
    //    streammer >> data.s1 >> data.s2 >> data.rawPump >> newBucketTemp >>
    //        cameraOnTemp;
    //    data.newBucket = newBucketTemp;
    //    data.cameraOn = cameraOnTemp;
    //    qDebug() << ba;
  }
  BHMI* _bhmi;
};

#endif  // TMP_LAMBDA_H_FILE
