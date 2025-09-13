#ifndef QUAMIUM_H
#define QUAMIUM_H

#include "webcanvas.h"

#include <QMainWindow>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui {
class Quamium;
}
QT_END_NAMESPACE

class Quamium : public QMainWindow
{
    Q_OBJECT

public:
    Quamium(QWidget *parent = nullptr);
    ~Quamium();

private slots:
    void onSearchButtonClicked();

private:
    Ui::Quamium *ui;
    WebCanvas* webCanvas;

    void drawBody(std::string);
};
#endif // QUAMIUM_H
