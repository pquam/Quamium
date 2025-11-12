#include "webcanvas.h"
#include <iostream>

WebCanvas::WebCanvas(QWidget* parent) : QWidget(parent) {

    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(true);

}

void WebCanvas::setDisplayList() {

    //m_items = items;

    _scrollY = 0;    
    
    update();
}

void WebCanvas::paintEvent(QPaintEvent* /*ev*/) {
    
    QPainter painter(this);
    QFont font = painter.font();
    font.setPixelSize(48);
    painter.setFont(font);

    painter.drawText(30, 30, "Hello World");

}

void WebCanvas::wheelEvent(QWheelEvent* ev) {
    // Let Qt handle scrolling - just pass the event to the parent
    QWidget::wheelEvent(ev);
}

