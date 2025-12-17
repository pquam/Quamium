#pragma once
#include <QWidget>
#include <QTextDocument>
#include <QString>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollArea>
#include <QScrollBar>

#include <iostream>

#include "layout.h"
#include "structs/DisplayText.h"

class WebCanvas : public QWidget {
    Q_OBJECT
public:
    explicit WebCanvas(QWidget* parent = nullptr);

    // Set the display list and layout used for rendering
    void start(const Layout& layout);
    void setDisplayList(const std::vector<DisplayText>& display_list, const Layout& layout);
    void clear();
    void setScrollArea(QScrollArea* area);
    int verticalScrollOffset() const;
    int horizontalScrollOffset() const;

protected:
    void paintEvent(QPaintEvent* ev) override;
    void wheelEvent(QWheelEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;

    std::vector<DisplayText> display_list;

private:
    Layout la;
    QScrollArea* scrollArea = nullptr;
};
