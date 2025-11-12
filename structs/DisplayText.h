#include <QString>

struct DisplayText {

    int x;
    int y;
    bool italics = false;
    bool bold = false;
    QString text;
    QFont font;
    QColor color;
};