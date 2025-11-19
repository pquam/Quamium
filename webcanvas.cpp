#include "webcanvas.h"
#include <iostream>

WebCanvas::WebCanvas(QWidget* parent) : QWidget(parent) {

    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(true);

}

void WebCanvas::setDisplayList(std::vector<DisplayText> display_list) {


    this->display_list = display_list;    
    
    update();
}

void WebCanvas::paintEvent(QPaintEvent* /*ev*/) {
    
    QPainter painter(this);

    for (DisplayText text : display_list) {

        painter.setFont(text.font);
        painter.drawText(text.x, text.y, text.text);
    }
}

void WebCanvas::wheelEvent(QWheelEvent* ev) {
    // Let Qt handle scrolling - just pass the event to the parent
    QWidget::wheelEvent(ev);
}

