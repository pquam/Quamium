#ifndef QUAMIUM_H
#define QUAMIUM_H

#include "webcanvas.h"
#include "ui_quamium.h"
#include "webcanvas.h"
#include "server.h"
#include "lexer.h"
#include "layout.h"

#include <QMainWindow>
#include <qobject.h>

#include <string>
#include <vector>

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

    int width = 900;
    int height = 600;
};
#endif // QUAMIUM_H
