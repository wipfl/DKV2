#ifndef LETTERSNIPPETS_H
#define LETTERSNIPPETS_H

#include "helper.h"
#include "contract.h"

enum class letterType;

enum class snippetKind {
    allLettersAllKreditors, // like Datum: same for all letters and all creditors
    allKreditors,           // like Betreff: different in each letter, same for all creditors
    allLetters,             // like Anrede: same for all letters, different for each creditor
    individual,     // like "Text 1": different for each letter, might differ for each creditor
    maxValue
};
snippetKind snippetKindFromInt( int i);

#define SNIPPETS \
    X(address,  "address",  snippetKind::allLetters)              \
    X(date,     "date",     snippetKind::allLettersAllKreditors) \
    X(about,    "about",    snippetKind::allKreditors)            \
    X(greeting, "greeting", snippetKind::allLettersAllKreditors)  \
    X(text1,    "text1",    snippetKind::individual)               \
    X(table,    "table",    snippetKind::allKreditors) \
    X(text2,    "text2",    snippetKind::individual)             \
    X(salut,    "salut",    snippetKind::allLetters)               \
    X(foot,     "foot",     snippetKind::allLettersAllKreditors)   \
    X(maxValue, "n/a",      snippetKind::allLettersAllKreditors) \

#define X(a, b, c) a,
enum class snippetType {
    SNIPPETS
};
#undef X

snippetType snippetTypeFromInt(int i);

#define X(a,b,c) qsl(b),
const QStringList snippetNames {
    SNIPPETS
};
#undef X

#define X(a,b,c) {snippetType::a, c},
const QMap<snippetType, snippetKind> snippet_type {
    SNIPPETS
};
#undef X

struct snippet
{
    snippet(snippetType ls, letterType lType =(letterType)allLetters, qlonglong creditor =0);

    QString name()     const { return snippetNames[int(ls)]; };
    snippetKind type() const { return snippet_type[ls]; };

    std::pair<QString, bool> read(QSqlDatabase db=QSqlDatabase::database ()) const;
    bool write(const QString& t, QSqlDatabase db=QSqlDatabase::database ()) const;
    bool wInitDb (const QString text, QSqlDatabase db=QSqlDatabase::database ()) const;

    static const QString tableName, fnSnippet, fnLetter, fnCreditor, fnText;
    static const dbtable& getTableDef();
    static const QVector<QString> getIndexes();
    static const qlonglong cId_allKreditors =0;
    static const int allLetters =0;
private:
    static std::pair<QString, bool> read  (const snippetType sId, const letterType lId, const qlonglong kId, QSqlDatabase db=QSqlDatabase::database ());
    static bool    write (const QString text, const snippetType sId, const letterType lId, const qlonglong kId, QSqlDatabase db=QSqlDatabase::database ());
    const snippetType ls;
    letterType lType;
    qlonglong cId;
};


// for testing
QVector<snippet> randomSnippets(int count);


#endif // LETTERSNIPPETS_H
