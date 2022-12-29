#include <CameraDriver.h>

#include <QApplication>
#include <QCameraImageCapture>
//#include <linux/i2c-dev.h>
//#include <linux/i2c.h>
//#include <linux/media.h>
#include <QCamera>
#include <QCameraInfo>
#include <QCameraViewfinder>
#include <QMultimedia>
#include <cassert>

CameraDriver::CameraDriver() {
  const auto cameras = QCameraInfo::availableCameras();
  if (cameras.isEmpty()) {
    qDebug() << "Camera not found! connect camera and restart.";
    assert("Please make camera online!");
    return;
  }
  for (const auto &cameraInfo : cameras) {
    if (QCameraInfo::defaultCamera() == cameraInfo) {
      _camera.reset(new QCamera(cameraInfo));  // last camera
    }
  }
  if (_camera.isNull()) {
    // just use a camera if not found
    _camera.reset(new QCamera(cameras.at(0)));
  }
}

void CameraDriver::setCameraRenderer(QCameraViewfinder *view) {
  if (_camera.isNull()) {
    assert("Please make sure camera is connected!");
    return;
  }
  new QCameraImageCapture(_camera.data());  // extra TODO:remove
  _camera->setViewfinder(view);
  //  view->setMediaObject(_camera.get());
}

bool CameraDriver::start() {
  if (_camera.isNull()) return false;
  _camera->setCaptureMode(QCamera::CaptureViewfinder);
  _camera->start();
  _camera->searchAndLock();
  return true;
}

bool CameraDriver::stop() {
  if (_camera.isNull()) return false;
  //  _camera->setCaptureMode(QCamera::CaptureStillImage);
  _camera->stop();
  return true;
}
