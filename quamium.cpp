#include "quamium.h"
#include <qwindowdefs.h>



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
    
    connect(webCanvas, &WebCanvas::needRelayout, this, [this](int width) {
        auto list = la.layout(width);
        contentSize = QSize(la.getContentWidth(), la.getContentHeight());
        webCanvas->setDisplayList(list, contentSize);
    });

    // Connect the search button's clicked signal to our slot
    connect(ui->searchButton, &QPushButton::clicked, this, &Quamium::onSearchButtonClicked);

    loadDefault();
}

void Quamium::loadDefault() {

        // Open the file using ifstream
    std::string filepath = QCoreApplication::applicationDirPath().toStdString() + "/../defaultpages/aboutblank.html";
    std::fstream file(filepath);

    // confirm file opening
    if (file.is_open()) {


        //string constructor overload 7
        //template <class InputIterator>  string  (InputIterator first, InputIterator last);
        //std::istreambuf_iterator<char>() is a default-constructed iterator that represents “end of stream”.
        std::string body((std::istreambuf_iterator<char>(file)),
                 std::istreambuf_iterator<char>());


        tokens = l.lex(body, tokens);

        la.setContentHeight(height);
        la.setContentWidth(width);
        la.clearMetricsCache();
        la.initialLayout(&tokens, width);

        contentSize = QSize(la.getContentWidth(), la.getContentHeight());

        webCanvas->start(la.getDisplayList(),contentSize);
    }

    file.close();

}

void Quamium::onSearchButtonClicked()
{
    webCanvas->clear();

    Server s = Server();
    s.setInput(ui->searchBar->text().toStdString());
    body = s.getBody(true);

    /* lexer based layout
    tokens = l.lex(body, tokens);

    la.setContentHeight(height);
    la.setContentWidth(width);
    la.clearMetricsCache();
    la.initialLayout(&tokens, width);
    */

    root_node = p.parse(body, htmlTreeHolder);
    p.printTree(root_node, 2);

    nla.setContentHeight(height);
    nla.setContentWidth(width);
    nla.clearMetricsCache();
    nla.initialLayout(root_node, width);
    

    contentSize = QSize(la.getContentWidth(), la.getContentHeight());

    webCanvas->start(la.getDisplayList(),contentSize);

}



Quamium::~Quamium()
{
    
    delete ui;
}