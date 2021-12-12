#ifndef LETTER_H
#define LETTER_H

#include "lettersnippets.h"
#include "booking.h"


enum class letterType {
    all =snippet::allLetters,
    annPayoutL        =int(interestModel::payout)   +1,
    annReinvestL      =int(interestModel::reinvest) +1,
    annInterestInfoL  =int(interestModel::fixed)    +1,
    annInfoL          =int(interestModel::zero)     +1,
    maxValue
};
int intFromLetterType( letterType lt);
letterType letterTypeFromInt(int lt);

struct letter
{
    letter(booking b);
    QVector<QString> snippets;
private:
    bool loadSnippets();
    bool initVars();
    std::pair<QString, bool> applyVariables(QString text);

    letterType lt;
    const contract cont;
    const creditor cred;
    QMap<QString, QString> variables;
};



/// init for new db
int writeDefaultSnippets(QSqlDatabase db =QSqlDatabase::database ());


#endif // LETTER_H
