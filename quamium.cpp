#include "quamium.h"


Quamium::Quamium(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Quamium)
{
    ui->setupUi(this);
    
    // Create WebCanvas and attach it to the central layout so it expands with the window
    webCanvas = new WebCanvas;
    webCanvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->gridLayout->addWidget(webCanvas, 0, 0);
    
    std::cout << "WebCanvas created and added to layout" << std::endl;
    
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

    Lexer l;
    Layout la;

    std::string body = s.getBody(true);
    std::cout << body << std::endl;
    std::vector<Content> tokens = l.lex(body);
    std::vector<DisplayText> display_list = la.layout(tokens, width);

    webCanvas->setDisplayList(display_list);

}
