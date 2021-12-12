#include <QtSql>
#include <QtTest>

#include "../DKV2/helper.h"
#include "../DKV2/helpersql.h"
#include "../DKV2/dkdbhelper.h"
#include "../DKV2/lettersnippets.h"
#include "../DKV2/letter.h"

#include "testhelper.h"
#include "test_lettersnippet.h"


test_letterSnippet::test_letterSnippet(QObject *p) : QObject(p)
{
}

void test_letterSnippet::initTestCase()
{   LOG_CALL;
    createTestDbTemplate();
}
void test_letterSnippet::cleanupTestCase()
{
    cleanupTestDbTemplate();
}

void test_letterSnippet::init()
{   LOG_CALL;
    initTestDbFromTemplate();
}

void test_letterSnippet::cleanup()
{   LOG_CALL;
    cleanupTestDb();
}

void test_letterSnippet::test_write_read_snippet()
{   LOG_CALL;
    const QString expected {qsl("20.Januar 2021")};
    snippet snip(snippetType::date);
    QVERIFY(snip.write(expected));
    QCOMPARE(snip.read ().first, expected);
}

void test_letterSnippet::test_overwrite_snippet()
{   LOG_CALL;
    snippet snip(snippetType::greeting);

    QVERIFY(snip.write(qsl("firstString")));
    const QString expected {qsl("expected")};
    QVERIFY(snip.write(expected));
    QCOMPARE(snip.read().first, expected);
}

void test_letterSnippet::test_many_snippet_read_writes()
{
    const int nbrCreditors =100;
    saveRandomCreditors_q(nbrCreditors);

    const int iter =100;

    {
    dbgTimer timer(qsl("timer_test_many_snippet_read_writes"));
    QVector<snippet> v =randomSnippets (iter);
    int count =0;
    for(const auto& snip : qAsConst(v))
    {
        count++;
        QString text =qsl("test ") + QString::number(count);
        snip.write (text);
        QCOMPARE(text, snip.read().first);
    }
    }
}

void test_letterSnippet::test_snippet_type_dep_read()
{
    saveRandomCreditor ();
    const QString expected {qsl("expected")};
    snippet s1(snippetType::date);
    s1.write (expected);
    snippet s2(snippetType::date);
    QCOMPARE(expected, s2.read().first);

    snippet s3(snippetType::table);
    s3.write(expected);
    snippet s4(snippetType::table, letterType::annInterestInfoL);
    QCOMPARE(expected, s3.read().first);

    snippet s5(snippetType::address, letterType::all, 1);
    s5.write(expected);
    snippet s6(snippetType::address, letterType::annInterestInfoL, 1);
    QCOMPARE(expected, s6.read().first);
}

void test_letterSnippet::test_fallback()
{
    saveRandomCreditor ();
    const QString expected {qsl("expceted")};

    {
        snippet s2(snippetType::address, letterType::annPayoutL, snippet::cId_allKreditors);
        s2.write (expected);
        // address is "allLetters", should fall through to "allKreditors"
        snippet s1(snippetType::address, letterType::annPayoutL, 1);
        QCOMPARE(expected, s1.read ().first);
        QCOMPARE(expected, s2.read ().first);
        // ... and from all letters
        snippet s3(snippetType::address, letterType::all, 1);
        QCOMPARE(expected, s3.read ().first);
    }
    {
        snippet s1(snippetType::date, letterType::annInterestInfoL, 2);
        s1.write(expected);
        QCOMPARE (expected, s1.read().first);
        // date is "all...all..", should fall through to all Kcreditors and from all letters
        snippet s2(snippetType::date, letterType::all, 0);
        QCOMPARE (expected, s2.read().first);
        snippet s3(snippetType::date, letterType::annInfoL, 0);
        QCOMPARE (expected, s2.read().first);
    }
    {
        // "about" is allKreditors, should fall through from any creditor
        snippet s1(snippetType::about, letterType::annPayoutL, 1);
        s1.write (expected);
        snippet s2(snippetType::about, letterType::annPayoutL, 0);
        QCOMPARE( expected, s2.read ().first);
        // faile for other letter
        snippet s3(snippetType::about, letterType::annInfoL, 1);
        QVERIFY (not s3.read().second);
    }
}

void test_letterSnippet::test_writeDefaultSnippets()
{
    int written =writeDefaultSnippets ();
    QCOMPARE (written, rowCount(snippet::tableName));
}

void test_letterSnippet::test_re_writeDefaultSnippetHasNoEffect()
{
    QString res {qsl("notexpected")};
    writeDefaultSnippets ();
    snippet s1(snippetType::greeting, letterType::all, 0);
    s1.wInitDb (res);
    QVERIFY(s1.read ().first not_eq res);

}
