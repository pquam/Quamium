#include "quamium.h"



Quamium::Quamium(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Quamium)
{
    ui->setupUi(this);
    resize(width, height);

    // Add padding around the search controls
    ui->horizontalLayout->setContentsMargins(5, 0, 5, 5);
    ui->horizontalLayout->setSpacing(2);

    // Ensure the central widget uses a vertical layout so children resize with the window
    auto *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(5, 5, 5, 0);
    mainLayout->setSpacing(0);
    ui->centralwidget->setLayout(mainLayout);
    mainLayout->addWidget(ui->horizontalLayoutWidget);
    mainLayout->addWidget(ui->webCanvas);
    
    // Create WebCanvas and set it inside the scroll area so it fills the available space
    webCanvas = new WebCanvas;
    webCanvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->webCanvas->setWidget(webCanvas);
    ui->webCanvas->setWidgetResizable(true);
    webCanvas->setScrollArea(ui->webCanvas);
    
    std::cout << "WebCanvas created and attached to central layout" << std::endl;
    
    // Connect the search button's clicked signal to our slot
    connect(ui->searchButton, &QPushButton::clicked, this, &Quamium::onSearchButtonClicked);
}

Quamium::~Quamium()
{
    
    delete ui;
}

void Quamium::onSearchButtonClicked()
{
    webCanvas->clear();
    Server s = Server();
    s.setInput(ui->searchBar->text().toStdString());

    Lexer l;
    Layout la;
    la.setContentHeight(height);
    la.setContentWidth(width);

    std::string body = s.getBody(true);
    std::vector<Content> tokens = l.lex(body);
    std::vector<DisplayText> display_list = la.layout(tokens, width);

    webCanvas->setDisplayList(display_list, la);

}
