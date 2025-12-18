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
#include <QVBoxLayout>

#include <vector>
#include <string>
#include <fstream>
#include <iostream>


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

    std::vector<Content> tokens;
    Lexer l;
    Layout la;
    std::string body;

    int width = 1600;
    int height = 900;

    void loadDefault();
};
#endif // QUAMIUM_H
