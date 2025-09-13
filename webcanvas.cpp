#include "webcanvas.h"
#include <QPainter>
#include <QWheelEvent>
#include <QResizeEvent>
#include <iostream>

WebCanvas::WebCanvas(QWidget* parent) : QWidget(parent) {
    _doc.setDocumentMargin(24);               // left/right padding
    QFont f; f.setFamily("monospace");        // easy for early debugging
    f.setPointSize(11);
    _doc.setDefaultFont(f);
}

void WebCanvas::setPlainText(const QString& text) {
    _doc.setPlainText(text);
    _doc.setTextWidth(width() - 2 * _doc.documentMargin());
    _scrollY = 0;
    
    // Set the widget size to match the document size so QScrollArea knows the full range
    QSize docSize = _doc.size().toSize();
    setFixedSize(docSize);
    
    updateGeometry();
    update();
}

void WebCanvas::setHtml(const QString& html) {
    std::cout << "WebCanvas::setHtml called with: " << html.toStdString().substr(0, 100) << "..." << std::endl;
    
    // Set HTML content directly
    _doc.setHtml(html);
    _doc.setTextWidth(width() - 2 * _doc.documentMargin());
    _scrollY = 0;
    
    // Set the widget size to match the document size so QScrollArea knows the full range
    QSize docSize = _doc.size().toSize();
    std::cout << "Setting WebCanvas size to: " << docSize.width() << "x" << docSize.height() << std::endl;
    setFixedSize(docSize);
    
    updateGeometry();
    update();
    
    std::cout << "Document size: " << _doc.size().width() << "x" << _doc.size().height() << std::endl;
    std::cout << "Document isEmpty: " << (_doc.isEmpty() ? "YES" : "NO") << std::endl;
    std::cout << "Document toPlainText length: " << _doc.toPlainText().length() << std::endl;
}

void WebCanvas::setRawHtml(const QString& html) {
    std::cout << "=== RAW HTML CONTENT ===" << std::endl;
    std::cout << html.toStdString() << std::endl;
    std::cout << "=== END RAW HTML ===" << std::endl;
    
    // Set the HTML content directly without escaping it
    _doc.setHtml(html);
    _doc.setTextWidth(width() - 2 * _doc.documentMargin());
    _scrollY = 0;
    
    // Set the widget size to match the document size so QScrollArea knows the full range
    QSize docSize = _doc.size().toSize();
    std::cout << "Setting WebCanvas size to: " << docSize.width() << "x" << docSize.height() << std::endl;
    
    // Use setFixedSize instead of resize to ensure the size is respected
    setFixedSize(docSize);
    
    updateGeometry();
    update();
}

void WebCanvas::paintEvent(QPaintEvent* /*ev*/) {
    static int paintCount = 0;
    std::cout << "WebCanvas::paintEvent called #" << ++paintCount << ", size: " << width() << "x" << height() << std::endl;
    
    QPainter p(this);
    
    if (width() <= 0 || height() <= 0) {
        std::cout << "Widget has zero size!" << std::endl;
        return;
    }
    
    // Let Qt handle scrolling - just draw the document normally
    _doc.setTextWidth(width() - 2 * _doc.documentMargin());
    
    std::cout << "Drawing document, text width: " << _doc.textWidth() << ", doc size: " << _doc.size().width() << "x" << _doc.size().height() << std::endl;
    std::cout << "Document has content: " << (_doc.isEmpty() ? "NO" : "YES") << std::endl;
    
    _doc.drawContents(&p);
    
}

void WebCanvas::wheelEvent(QWheelEvent* ev) {
    // Let Qt handle scrolling - just pass the event to the parent
    QWidget::wheelEvent(ev);
}

void WebCanvas::resizeEvent(QResizeEvent* ev) {
    std::cout << "WebCanvas::resizeEvent: " << ev->size().width() << "x" << ev->size().height() << std::endl;
    _doc.setTextWidth(width() - 2 * _doc.documentMargin());
    QWidget::resizeEvent(ev);
}


QSize WebCanvas::sizeHint() const {
    if (_doc.isEmpty()) {
        return QSize(800, 600);
    }
    return _doc.size().toSize();
}

void WebCanvas::clampScroll() {
    const qreal docHeight = _doc.size().height();
    const qreal maxScroll = std::max<qreal>(0.0, docHeight - height());
    if (_scrollY < 0) _scrollY = 0;
    if (_scrollY > maxScroll) _scrollY = maxScroll;
}
