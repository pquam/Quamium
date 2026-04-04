#ifndef QUAMIUM_H
#define QUAMIUM_H


#include "structs/content.h"
#include "structs/block.h"
#include "webcanvas.h"
#include "ui_quamium.h"
#include "server.h"

#include "htmlparser.h"
#include "nodelayout.h"

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

    Content* root_node = nullptr;
    std::vector<Content*> htmlTreeHolder = {root_node};

    Block* document = nullptr;
    std::vector<Block*> blockTreeHolder = {document};
    HtmlParser p;
    NodeLayout nla;

    std::string body;
    QSize contentSize;

    int width = 1600;
    int height = 900;

    void loadDefault();
};
#endif // QUAMIUM_H
