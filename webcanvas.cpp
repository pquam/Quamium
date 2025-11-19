#include "webcanvas.h"

WebCanvas::WebCanvas(QWidget* parent) : QWidget(parent) {

    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(true);

}

void WebCanvas::setDisplayList(const std::vector<DisplayText>& display_list, const Layout& layout) {
    this->la = layout;
    this->display_list = display_list;
    setMinimumSize(layout.getContentWidth(), layout.getContentHeight());
    update();
}

void WebCanvas::paintEvent(QPaintEvent* /*ev*/) {
    
    QPainter painter(this);
    painter.fillRect(rect(), palette().window());

    for (DisplayText text : display_list) {

        painter.setFont(text.font);
        painter.drawText(text.x, text.y, text.text);
    }
}

void WebCanvas::wheelEvent(QWheelEvent* ev) {
    // Let Qt handle scrolling - just pass the event to the parent
    QWidget::wheelEvent(ev);
}

void WebCanvas::resizeEvent(QResizeEvent* ev) {
    QWidget::resizeEvent(ev);
    auto updatedList = this->la.layout(0.95 * WebCanvas::width());
    this->display_list = updatedList;
    setMinimumSize(la.getContentWidth(), la.getContentHeight());
}

void WebCanvas::clear() {
    this->display_list.clear();
    update();
}
