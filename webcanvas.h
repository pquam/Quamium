#pragma once
#include <QWidget>
#include <QTextDocument>
#include <QString>
#include <QPainter>

#include <iostream>

#include "layout.h"
#include "structs/DisplayText.h"

class WebCanvas : public QWidget {
    Q_OBJECT
public:
    explicit WebCanvas(QWidget* parent = nullptr);

    // Set the display list and layout used for rendering
    void setDisplayList(const std::vector<DisplayText>& display_list, const Layout& layout);
    void clear();

protected:
    void paintEvent(QPaintEvent* ev) override;
    void wheelEvent(QWheelEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;

    std::vector<DisplayText> display_list;

private:
    Layout la;
    qreal _scrollY = 0.0;
};
