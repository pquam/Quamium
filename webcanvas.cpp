#include "webcanvas.h"

WebCanvas::WebCanvas(QWidget* parent) : QWidget(parent) {

    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(true);

}

void WebCanvas::start(const std::vector<DisplayText>& displayList, QSize contentSize) {
    this->display_list = display_list;
    setMinimumSize(contentSize.width(), contentSize.height());
    update();
}

void WebCanvas::setDisplayList(const std::vector<DisplayText>& display_list, QSize contentSize) {
    this->display_list = display_list;
    update();
}

void WebCanvas::setScrollArea(QScrollArea* area) {
    scrollArea = area;
}

int WebCanvas::verticalScrollOffset() const {
    return (scrollArea && scrollArea->verticalScrollBar()) 
        ? scrollArea->verticalScrollBar()->value()
        : 0;
}

int WebCanvas::horizontalScrollOffset() const {
    return (scrollArea && scrollArea->horizontalScrollBar()) 
        ? scrollArea->horizontalScrollBar()->value()
        : 0;
}

void WebCanvas::paintEvent(QPaintEvent* ev) {
    
    QPainter painter(this);
    painter.fillRect(rect(), palette().window());

    const int viewTop = verticalScrollOffset();
    const int viewBottom = viewTop + (scrollArea ? scrollArea->viewport()->height() : height());

    for (const DisplayText& text : display_list) {
        QFontMetrics metrics(text.font);
        const int textTop = text.y - metrics.ascent();
        const int textBottom = text.y + metrics.descent();

        if (textBottom < viewTop || textTop > viewBottom) continue;

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
    emit WebCanvas::needRelayout(int(0.95 * width()));
}

void WebCanvas::clear() {
    this->display_list.clear();
    update();
}
