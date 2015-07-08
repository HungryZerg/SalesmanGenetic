#include <QString>
#include <QtTest>
#include <QCoreApplication>

class Test : public QObject
{
    Q_OBJECT

public:
    Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
};

Test::Test()
{
}

void Test::initTestCase()
{
}

void Test::cleanupTestCase()
{
}

void Test::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_MAIN(Test)

#include "tst_test.moc"
