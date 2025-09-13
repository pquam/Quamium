#pragma once
#include <QWidget>
#include <QTextDocument>
#include <QString>

class WebCanvas : public QWidget {
    Q_OBJECT
public:
    explicit WebCanvas(QWidget* parent = nullptr);

    // Call this with your HTTP response body
    void setPlainText(const QString& text);
    void setHtml(const QString& html); // optional: if you want basic rich text
    void setRawHtml(const QString& html);


protected:
    void paintEvent(QPaintEvent* ev) override;
    void wheelEvent(QWheelEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;
    QSize sizeHint() const override;

private:
    QTextDocument _doc;  // does layout for us; we still "paint" it
    qreal _scrollY = 0.0;
    void clampScroll();
};
