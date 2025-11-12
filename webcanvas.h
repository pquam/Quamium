#pragma once
#include <QWidget>
#include <QTextDocument>
#include <QString>

class WebCanvas : public QWidget {
    Q_OBJECT
public:
    explicit WebCanvas(QWidget* parent = nullptr);

    // Call this with your HTTP response body
    void setDisplayList();

protected:
    void paintEvent(QPaintEvent* ev) override;
    void wheelEvent(QWheelEvent* ev) override;

private:
    qreal _scrollY = 0.0;
};
