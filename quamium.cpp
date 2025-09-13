#include "quamium.h"
#include "ui_quamium.h"
#include "server.h"

#include <iostream>
#include <string>

Quamium::Quamium(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Quamium)
{
    ui->setupUi(this);
    
    // Connect the search button's clicked signal to our slot
    connect(ui->searchButton, &QPushButton::clicked, this, &Quamium::onSearchButtonClicked);
}

Quamium::~Quamium()
{
    delete ui;
}

void Quamium::onSearchButtonClicked()
{
    Server s = Server();
    s.setInput(ui->searchBar->text().toStdString());
    std::cout << s.getBody(true) << std::endl;
}
