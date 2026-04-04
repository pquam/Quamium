#include <QtTest/QtTest>
#include <QGuiApplication>
#include <QFontMetrics>

#include "utils/utils.h"
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
        ServerTests tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    return status;
}

#include "quamium_tests.moc"
