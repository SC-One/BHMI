#ifndef CAMERA_DRIVER_H
#define CAMERA_DRIVER_H
#include <QCamera>
#include <QScopedPointer>

class QCameraViewfinder;
class CameraDriver {
 public:
  explicit CameraDriver();

  void setCameraRenderer(QCameraViewfinder* view);

  bool start();
  bool stop();

 private:
  QScopedPointer<QCamera> _camera;
  //  QCameraViewfinder* cameraView = new QCameraViewfinder (this);
};
#endif  // CAMERA_DRIVER_H
