#include <QString>
#include <QTextFormat>

#pragma once
struct FontCache {

    QString word;
    QFont font;
    int height;
    int advance;
};