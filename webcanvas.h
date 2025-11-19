#pragma once
#include <QWidget>
#include <QTextDocument>
#include <QString>
#include <QPainter>
#include "structs/DisplayText.h"

class WebCanvas : public QWidget {
    Q_OBJECT
public:
    explicit WebCanvas(QWidget* parent = nullptr);

    // Call this with your HTTP response body
    void setDisplayList(std::vector<DisplayText> display_list);

protected:
    void paintEvent(QPaintEvent* ev) override;
    void wheelEvent(QWheelEvent* ev) override;

    std::vector<DisplayText> display_list;

private:
    qreal _scrollY = 0.0;
};
