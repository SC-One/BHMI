#ifndef BHMI_H
#define BHMI_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class BHMI; }
QT_END_NAMESPACE

class BHMI : public QMainWindow
{
    Q_OBJECT

public:
    BHMI(QWidget *parent = nullptr);
    ~BHMI();

private:
    Ui::BHMI *ui;
};
#endif // BHMI_H
