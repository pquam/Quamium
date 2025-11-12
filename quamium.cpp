#include "quamium.h"
#include "ui_quamium.h"
#include "webcanvas.h"
#include "server.h"
#include "lexer.h"
#include <iostream>
#include <qobject.h>
#include <string>



Quamium::Quamium(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Quamium)
{
    ui->setupUi(this);
    
    // Create WebCanvas and set it as the widget for the scroll area
    webCanvas = new WebCanvas();
    ui->webCanvas->setWidget(webCanvas);
    ui->webCanvas->setWidgetResizable(false);  // Disable auto-resizing so we can control the size
    ui->webCanvas->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->webCanvas->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    webCanvas->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);  // Fixed size policy
    webCanvas->setMinimumSize(800, 600);
    
    std::cout << "WebCanvas created and added to scroll area" << std::endl;
    
    // Connect the search button's clicked signal to our slot
    connect(ui->searchButton, &QPushButton::clicked, this, &Quamium::onSearchButtonClicked);
    onSearchButtonClicked();
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

    Lexer l;

    l.lex(s.getBody(true));

}

void Quamium::drawBody(std::string body) {

    std::cout << "in drawBody!" << std::endl;
    std::cout << "Body length: " << body.length() << std::endl;
    std::cout << "Body content (first 200 chars): " << body.substr(0, 200) << std::endl;
    
    if (body.empty()) {
        std::cout << "Body is empty!" << std::endl;
    } else {
        // Use raw HTML method for debugging
    }
}
