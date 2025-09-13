#include "quamium.h"
#include "./ui_quamium.h"

Quamium::Quamium(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Quamium)
{
    ui->setupUi(this);
}

Quamium::~Quamium()
{
    delete ui;
}
