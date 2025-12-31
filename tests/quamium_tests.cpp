#include <QtTest/QtTest>
#include <QGuiApplication>
#include <QFontMetrics>

#include "utils/utils.h"
#include "lexer.h"
#include "layout.h"
#include "server.h"

class UtilsTests : public QObject {
    Q_OBJECT
private slots:
    void split_basic();
    void split_preserves_empty_segments();
};

void UtilsTests::split_basic() {
    auto parts = Utils::split("one two three", ' ');
    QCOMPARE(static_cast<int>(parts.size()), 3);
    QCOMPARE(QString::fromStdString(parts[0]), QString("one"));
    QCOMPARE(QString::fromStdString(parts[1]), QString("two"));
    QCOMPARE(QString::fromStdString(parts[2]), QString("three"));
}

void UtilsTests::split_preserves_empty_segments() {
    auto parts = Utils::split("alpha,,beta,", ',');
    QCOMPARE(static_cast<int>(parts.size()), 4);
    QCOMPARE(QString::fromStdString(parts[0]), QString("alpha"));
    QCOMPARE(QString::fromStdString(parts[1]), QString(""));
    QCOMPARE(QString::fromStdString(parts[2]), QString("beta"));
    QCOMPARE(QString::fromStdString(parts[3]), QString(""));
}

class LexerTests : public QObject {
    Q_OBJECT
private slots:
    void lex_splits_tags_and_text();
};

void LexerTests::lex_splits_tags_and_text() {
    Lexer lexer;
    std::string html = "<body>Hello</body>";
    std::vector<Content> tokens = { Content{true, "stale"} };

    lexer.lex(html, tokens);

    QCOMPARE(static_cast<int>(tokens.size()), 3);
    QVERIFY(tokens[0].isTag);
    QCOMPARE(QString::fromStdString(tokens[0].text), QString("body"));
    QVERIFY(!tokens[1].isTag);
    QCOMPARE(QString::fromStdString(tokens[1].text), QString("Hello"));
    QVERIFY(tokens[2].isTag);
    QCOMPARE(QString::fromStdString(tokens[2].text), QString("/body"));
}

class LayoutTests : public QObject {
    Q_OBJECT
private slots:
    void processes_body_text_into_display_list();
    void applies_inline_styles();
    void wraps_lines_when_width_is_small();
};

void LayoutTests::processes_body_text_into_display_list() {
    Lexer lexer;
    std::string html = "<body>Hello world</body>";
    std::vector<Content> tokens;
    lexer.lex(html, tokens);

    Layout layout;
    layout.initialLayout(&tokens, 400);

    const auto& display = layout.getDisplayList();
    QCOMPARE(static_cast<int>(display.size()), 2);
    QCOMPARE(display[0].text, QString("Hello"));
    QCOMPARE(display[1].text, QString("world"));
    QCOMPARE(display[0].font.pixelSize(), 16);
    QVERIFY(layout.getContentWidth() > 0);
    QVERIFY(layout.getContentHeight() > 0);
}

void LayoutTests::applies_inline_styles() {
    Lexer lexer;
    std::string html = "<body><b>Bold</b> text <i>italic</i></body>";
    std::vector<Content> tokens;
    lexer.lex(html, tokens);

    Layout layout;
    layout.initialLayout(&tokens, 400);
    const auto& display = layout.getDisplayList();

    const DisplayText* boldWord = nullptr;
    const DisplayText* plainWord = nullptr;
    const DisplayText* italicWord = nullptr;

    for (const auto& word : display) {
        if (word.text == "Bold") boldWord = &word;
        else if (word.text == "text") plainWord = &word;
        else if (word.text == "italic") italicWord = &word;
    }

    QVERIFY(boldWord != nullptr);
    QVERIFY(boldWord->font.bold());

    QVERIFY(plainWord != nullptr);
    QVERIFY(!plainWord->font.bold());
    QVERIFY(!plainWord->font.italic());

    QVERIFY(italicWord != nullptr);
    QVERIFY(italicWord->font.italic());
}

void LayoutTests::wraps_lines_when_width_is_small() {
    Lexer lexer;
    std::string html = "<body>first second</body>";
    std::vector<Content> tokens;
    lexer.lex(html, tokens);

    QFont font;
    font.setPixelSize(16);
    QFontMetrics metrics(font);
    const int firstWidth = metrics.horizontalAdvance("first");
    const int pageWidth = firstWidth + 20;

    Layout layout;
    layout.initialLayout(&tokens, pageWidth);
    const auto& display = layout.getDisplayList();

    QCOMPARE(static_cast<int>(display.size()), 2);
    QCOMPARE(display[0].text, QString("first"));
    QCOMPARE(display[1].text, QString("second"));
    QVERIFY(display[0].y < display[1].y);
}

class ServerTests : public QObject {
    Q_OBJECT
private slots:
    void defaults_when_input_is_empty();
    void parses_input_with_port_and_path();
    void parses_host_without_scheme();
};

void ServerTests::defaults_when_input_is_empty() {
    Server server;
    const auto url = server.parseInputToURL("");

    QCOMPARE(QString::fromStdString(url), QString("https://patrick.quam.computer/"));
    QCOMPARE(QString::fromStdString(server.getScheme()), QString("https"));
    QCOMPARE(QString::fromStdString(server.getHost()), QString("patrick.quam.computer"));
    QCOMPARE(QString::fromStdString(server.getPort()), QString("443"));
    QCOMPARE(QString::fromStdString(server.getPath()), QString("/"));
}

void ServerTests::parses_input_with_port_and_path() {
    Server server;
    const auto url = server.parseInputToURL("http://example.com:8080/path/to/page");

    QCOMPARE(QString::fromStdString(url), QString("http://example.com/path/to/page"));
    QCOMPARE(QString::fromStdString(server.getScheme()), QString("http"));
    QCOMPARE(QString::fromStdString(server.getHost()), QString("example.com"));
    QCOMPARE(QString::fromStdString(server.getPort()), QString("8080"));
    QCOMPARE(QString::fromStdString(server.getPath()), QString("/path/to/page"));
}

void ServerTests::parses_host_without_scheme() {
    Server server;
    const auto url = server.parseInputToURL("example.com/foo/bar");

    QCOMPARE(QString::fromStdString(url), QString("https://example.com/foo/bar"));
    QCOMPARE(QString::fromStdString(server.getScheme()), QString("https"));
    QCOMPARE(QString::fromStdString(server.getHost()), QString("example.com"));
    QCOMPARE(QString::fromStdString(server.getPort()), QString("443"));
    QCOMPARE(QString::fromStdString(server.getPath()), QString("/foo/bar"));
}

int main(int argc, char** argv) {
    QGuiApplication app(argc, argv);
    int status = 0;

    {
        UtilsTests tc;
        status |= QTest::qExec(&tc, argc, argv);
    }
    {
        LexerTests tc;
        status |= QTest::qExec(&tc, argc, argv);
    }
    {
        LayoutTests tc;
        status |= QTest::qExec(&tc, argc, argv);
    }
    {
        ServerTests tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    return status;
}

#include "quamium_tests.moc"
