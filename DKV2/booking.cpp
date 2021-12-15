#include <QtMath>

#include "dkdbviews.h"
#include "contract.h"
#include "booking.h"

/* static */ const dbtable& booking::getTableDef()
{
    static dbtable bookingsTable(qsl("Buchungen"));
    if( 0 == bookingsTable.Fields().size()) {
        bookingsTable.append(dbfield(qsl("id"),          QVariant::LongLong).setPrimaryKey().setAutoInc());
        bookingsTable.append(dbfield(qsl("VertragsId"),  QVariant::LongLong).setNotNull());
        bookingsTable.append(dbForeignKey(bookingsTable[qsl("VertragsId")], dkdbstructur[contract::tnContracts][contract::fnId], qsl("ON DELETE RESTRICT")));
        bookingsTable.append(dbfield(qsl("Datum"),       QVariant::Date).setDefault(EndOfTheFuckingWorld_str).setNotNull());
        bookingsTable.append(dbfield(qsl("BuchungsArt"), QVariant::Int).setNotNull()); // deposit, interestDeposit, outpayment, interestPayment
        bookingsTable.append(dbfield(qsl("Betrag"),      QVariant::Int).setNotNull()); // in cent
        bookingsTable.append(dbfield(qsl("Zeitstempel"),  QVariant::DateTime).setDefaultNow());
    }
    return bookingsTable;
}
/* static */ const dbtable& booking::getTableDef_deletedBookings()
{
    static dbtable deletedBookings(qsl("exBuchungen"));
    if( 0 == deletedBookings.Fields().size()) {
        deletedBookings.append(dbfield(qsl("id"), QVariant::LongLong).setPrimaryKey());
        for( int i =1 /* not 0 */ ; i < getTableDef().Fields().count(); i++) {
            deletedBookings.append(getTableDef().Fields()[i]);
        }
        deletedBookings.append(dbForeignKey(deletedBookings[qsl("VertragsId")],
                               dkdbstructur[contract::tnExContracts][contract::fnId], qsl("ON DELETE RESTRICT")));
    }
    return deletedBookings;
}
/* static */ QString booking::displayString(const Type t)
{
    switch(t)
    {
    case booking::Type::deposit : return qsl("Einzahlung");
    case booking::Type::payout : return qsl("Auszahlung");
    case booking::Type::reInvestInterest: return qsl("Zinsanrechnung");
    case booking::Type::annualInterestDeposit: return qsl("Jahreszins");
    case booking::Type::setInterestActive: return qsl("Aktivierung d. Zinszahlung");
    default:
        QString error{qsl("FEHLER: ungültiger Buchungstyp")};
        Q_ASSERT(false);
        return error;
    }
}
/* static */ QString booking::typeName(booking::Type t)
{
    switch(t) {
    case booking::Type::non: return qsl("invalid booking");
    case booking::Type::deposit: return qsl("deposit");
    case booking::Type::payout: return qsl("payout");
    case booking::Type::reInvestInterest: return qsl("reinvest interest");
    case booking::Type::annualInterestDeposit: return qsl("annual interest");
    case booking::Type::setInterestActive: return qsl("interest activation");
    default: return qsl("ERROR");
    }
}

/* static */ bool booking::doBooking( booking::Type t, const qlonglong contractId, QDate date, const double amount)
{   LOG_CALL_W(typeName(t));
    TableDataInserter tdi(booking::getTableDef());
    tdi.setValue(qsl("VertragsId"), contractId);
    tdi.setValue(qsl("BuchungsArt"), static_cast<int>(t));
    tdi.setValue(qsl("Betrag"), ctFromEuro(amount));
    tdi.setValue(qsl("Datum"), date);
//    "Zeitstempel" will be created by default
    if( -1 not_eq tdi.WriteData()) {
        qInfo() << "successful booking: " << typeName(t) << " contract#: " << contractId << " Amount: " << ctFromEuro(amount) << " date: " << date;
        return true;
    }
    qCritical() << "booking failed for contract#: " << contractId << " Amount: " << ctFromEuro(amount) << " date: " << date;;
    return false;
}
/* static */ bool booking::bookDeposit(const qlonglong contractId, QDate date, const double amount)
{
    Q_ASSERT( amount > 0.);
    return doBooking( Type::deposit, contractId, date, amount);
}
/* static */ bool booking::bookPayout(const qlonglong contractId, QDate date, const double amount)
{
    // contract has to check that a payout is possible
    return doBooking(Type::payout, contractId, date, -1*qFabs(amount));
}
/* static */ bool booking::bookReInvestInterest(const qlonglong contractId, QDate date, const double amount)
{
    Q_ASSERT( amount >= 0.);
    return doBooking(Type::reInvestInterest, contractId, date, amount);
}
/* static */ bool booking::bookAnnualInterestDeposit(const qlonglong contractId, QDate date, const double amount)
{
    Q_ASSERT( amount >= 0.);
    return doBooking(Type::annualInterestDeposit, contractId, date, amount);
}

/* static */ bool booking::bookInterestActive(const qlonglong contractId, QDate date)
{
    return doBooking(Type::setInterestActive, contractId, date, 0.);
}

/* static */ QDate bookings::dateOfnextSettlement()
{   LOG_CALL;
    QDate ret =executeSingleValueSql(qsl("SELECT date FROM (%1)").arg(sqlNextAnnualSettlement)).toDate();
    qInfo() << "Date of next Settlement was found as " << ret;
    return ret;
}

/* static */ int bookings::count(qlonglong cid)
{   LOG_CALL;
    return executeSingleValueSql (qsl("SELECT COUNT(*) FROM Buchungen WHERE VertragsId = %1 ").arg(cid)).toInt ();
}

booking::booking(qlonglong bid)
{
    QSqlRecord rec =executeSingleRecordSql (booking::getTableDef ().Fields (), qsl("id = %1").arg(QString::number(bid)));
    if( rec.isEmpty ()){
        qDebug() << "no matching booking";
        return;
    }
    contractId =rec.value (qsl("VertragsId")).toLongLong ();
    type =bookingTypeFromInt(rec.value(qsl("BuchungsArt")).toInt ());
    date =rec.value (qsl("Datum")).toDate ();
    amount =rec.value(qsl("Betrag")).toDouble();
}
