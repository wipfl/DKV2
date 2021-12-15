#include "pch.h"
#include "helpersql.h"
#include "letter.h"

int intFromLetterType( letterType lt)
{
    return (int) lt;
}

letterType letterTypeFromInt(int lt)
{
    if( lt < intFromLetterType(letterType::maxValue) && lt >=0) {
        return (letterType) lt;
    }
    Q_ASSERT("invalid letterType");
    return letterType::maxValue;
}

letter::letter(booking b)
    : cont(contract(b.contractId))
    , cred(creditor(cont.creditorId ()))
{
    qDebug() << "letter from Contract " << cont.id()
             << " creditor " << cred.id ();
    if( b.type == booking::Type::annualInterestDeposit) {
        switch(cont.iModel ())
        {
        case interestModel::payout:
        case interestModel::reinvest:
        case interestModel::fixed:
        case interestModel::zero:
            lt =letterTypeFromInt ((int)cont.iModel () +1);
            break;
        case interestModel::maxId:
        default:
//        case interestModel::allIModels:
            qCritical() << "can not build invalid letter type";
        }
    }
    loadSnippets ();
}

std::pair<QString, bool> letter::applyVariables(QString )
{
    return {QString(), false};
}

bool letter::loadSnippets()
{   LOG_CALL;
    for(snippetType type =snippetTypeFromInt(0); type < snippetType::maxValue; type =snippetTypeFromInt(int(type) +1)) {
        qDebug() << "Type: " << snippetNames[(int)type] << ", LetterType: "
                << intFromLetterType(lt) << ", CredId:" << cred.id();
        snippet snip (type, lt, cred.id ());
        QString text;
        bool suc;
        std::tie(text, suc) =snip.read();
        if(suc)
            snippets.push_back (text);
        else
            qCritical() << "loading snippet failed";
    }
    return true;
}

bool letter::initVars()
{
    // general Vars:
    /*
     *  DATUM
    */
    variables.insert (qsl("Datum"), QDate::currentDate ().toString ());
    // project Vars:
    /*
    Meta.gmbh.address1, Meta.gmbh.address1, Meta.gmbh.strasse, Meta.gmbh.plz, Meta.gmbh.stadt
    Meta.gmbh.email, Meta.gmbh.url
    Meta.gmbh.ProjektInitialen, Meta.gmbh.Handelsregister
    Meta.gmbh.gefue1, Meta.gmbh.gefue2, Meta.gmbh.gefue3
    Meta.gmbh.dkv
     */
    QVector<QSqlRecord> records;
    if( executeSql (qsl("SELECT * FROM Meta WHERE Name LIKE 'gmbh.'"), records)) {
        for(const auto & record : qAsConst(records)) {
            variables.insert (record.value (0).toString (), record.value(1).toString ());
        }
    }

    // creditor Vars
    /*
    Kreditoren.Vorname
    .Nachname,
    .Strasse, Plz, Stadt, Land,
    .Email,
    .IBAN, BIC
    */
    // contract Vars
    /*
    Vertraege.Kennung
    Zinssatz
    Volumen
    */
    // buchungs Vars
    /*
    WertVorher
    WertNachher
    WertZinsen
    */

    return true;
}

////////// //////////////////////
//////// ///////////////////////
////// ////////////////////////
//// /////////////////////////
int writeDefaultSnippets(QSqlDatabase db)
{
    QVector<QPair<snippet, QString>> defaultSnippets = {
         { snippet(snippetType::address, letterType::all), qsl("<small>{{gmbh.address1}} {{gmbh.address2}}<br>{{gmbh.strasse}}, <b>{{gmbh.plz}}</b> {{gmbh.stadt}}</small><p>"
           "{{kreditoren.vorname}} {{kreditoren.nachname}} <p> {{kreditoren.strasse}} <br><b> {{kreditoren.plz}} </b> {{kreditoren.stadt}} <br><small> {{kreditoren.email}} </small>")}
        ,{ snippet(snippetType::date), qsl("{{gmbh.adresse1}} den {{datum}}")}
        ,{ snippet(snippetType::about, letterType::annPayoutL), qsl("Jahreszinsinformation {{Jahr}}")}
        ,{ snippet(snippetType::about, letterType::annReinvestL), qsl("Jahreszinsinformation {{Jahr}}")}
        ,{ snippet(snippetType::about, letterType::annInterestInfoL), qsl("Jahreszinsinformation {{Jahr}}")}
        ,{ snippet(snippetType::about, letterType::annInfoL), qsl("Jährliche Kreditinformation {{Jahr}}")}
        ,{ snippet(snippetType::greeting), qsl("Liebe/r {{Vorname}}")}
        ,{ snippet(snippetType::text1, letterType::annPayoutL), qsl("die Mitglieder des Wohnprojektes Esperanza wünschen ein schönes neues Jahr und bedanken sich herzlich für Deine Unterstützung.<p>"
           "Dies ist der Kontoauszug Deiner Direktkredite für das Jahr 2019 bei {{gmbh.adresse1}}. "
           "Vereinbarungsgemäß werden die Zinsen Deines Direktkredits in den nächsten Tagen ausgezahlt. Auf Wunsch erstellen wir eine gesonderte Zinsbescheinigung für die Steuererklärung.")}
        ,{ snippet(snippetType::text1, letterType::annReinvestL), qsl("die Mitglieder des Wohnprojektes Esperanza wünschen ein schönes neues Jahr und bedanken sich herzlich für Deine Unterstützung.<p>"
           "Dies ist der Kontoauszug Deiner Direktkredite für das Jahr {{Jahr}} bei {{gmbh.adresse1}}. "
           "Vereinbarungsgemäß wurden die Zinsen Deinem Direktkredit Konto gut geschrieben. Auf Wunsch erstellen wir eine gesonderte Zinsbescheinigung für die Steuererklärung.")}
        ,{ snippet(snippetType::text1, letterType::annInterestInfoL), qsl("die Mitglieder des Wohnprojektes Esperanza wünschen ein schönes neues Jahr und bedanken sich herzlich für Deine Unterstützung.<p>"
           "Vereinbarungsgemäß werden die Zinsen für Deinen Direktkredit bis zur Auszahlung des Kredits bei uns verwahrt.")}
        ,{ snippet(snippetType::text1, letterType::annInfoL), qsl("die Mitglieder des Wohnprojektes Esperanza wünschen ein schönes neues Jahr und bedanken sich herzlich für Deine Unterstützung.<p>")}
        ,{ snippet(snippetType::table, letterType::annPayoutL), qsl("<table></table>")}
        ,{ snippet(snippetType::table, letterType::annReinvestL), qsl("<table></table>")}
        ,{ snippet(snippetType::table, letterType::annInterestInfoL), qsl("<table></table>")}
        ,{ snippet(snippetType::table, letterType::annInfoL), qsl("<table></table>")}
        ,{ snippet(snippetType::text2, letterType::annPayoutL), qsl("Soltest Du noch Fragen zu dieser Abrechnung haben, so zögere bitte nicht, Dich bei uns per Post oder E-Mail zu melden.<p>"
           "Wir hoffen auch in diesem Jahr auf Deine Solidarität. Für weitere Umschuldungen benötigen wir auch weiterhin Direktkredite. "
           "Empfehle uns Deinen Freund*innen und Verwandten.")}
        ,{ snippet(snippetType::text2, letterType::annReinvestL), qsl("Soltest Du noch Fragen zu dieser Abrechnung haben, so zögere bitte nicht, Dich bei uns per Post oder E-Mail zu melden.<p>"
           "Wir hoffen auch in diesem Jahr auf Deine Solidarität. Für weitere Umschuldungen benötigen wir auch weiterhin Direktkredite. "
           "Empfehle uns Deinen Freund*innen und Verwandten.")}
        ,{ snippet(snippetType::text2, letterType::annInterestInfoL), qsl("Soltest Du noch Fragen zu dieser Abrechnung haben, so zögere bitte nicht, Dich bei uns per Post oder E-Mail zu melden.<p>"
           "Wir hoffen auch in diesem Jahr auf Deine Solidarität. Für weitere Umschuldungen benötigen wir auch weiterhin Direktkredite. "
           "Empfehle uns Deinen Freund*innen und Verwandten.")}
        ,{ snippet(snippetType::text2, letterType::annInfoL), qsl("Soltest Du noch Fragen zu Deinem Kredit haben, so zögere bitte nicht, Dich bei uns per Post oder E-Mail zu melden.<p>"
           "Wir hoffen auch in diesem Jahr auf Deine Solidarität. Für weitere Umschuldungen benötigen wir auch weiterhin Direktkredite. "
           "Empfehle uns Deinen Freund*innen und Verwandten.")}
        ,{ snippet(snippetType::salut, letterType::all), qsl("Mit freundlichen Grüßen")}
        ,{ snippet(snippetType::foot), qsl("<table width=100%><tr><td width=33%><small>Geschäftsführer*innen:<br>{{gmbh.gefue1}}<br>{{gmbh.gefue2}}<br>{{gmbh.gefue3}}</small></td><td width=33%></td><td width=33%></td></tr></table>")}
    };
    int ret =0;
    for (const auto& pair: qAsConst(defaultSnippets)) {
        if( pair.first.wInitDb(pair.second, db))
        {
            qDebug() << "successfully written snippet " << pair.first.name () << " : " << pair.second;
            ret++;
        }
    }
    return ret;
}

QVector<snippet> randomSnippets(int count)
{
    QRandomGenerator* rand =QRandomGenerator::system ();
    QVector<snippet> v;
    for( int i =0; i< count; i++) {
        const snippetType sid =snippetTypeFromInt(rand->bounded (200)%int(snippetKind::maxValue));
        letterType lid;
        qlonglong kid;
        switch(snippet_type[sid])
        {
        case snippetKind::allLettersAllKreditors:
            lid =letterType::all;
            kid =0;
            break;
        case snippetKind::allKreditors:
            kid =0;
            lid =letterTypeFromInt(rand->bounded (100)%int(letterType::maxValue));
            break;
        case snippetKind::allLetters:
            lid =letterType::all;
            kid = rand->bounded (count) +1;
            break;
        case snippetKind::individual:
            /* or */
        case snippetKind::maxValue:
            lid =letterTypeFromInt(rand->bounded (100)%int(letterType::maxValue));
            kid = rand->bounded (count) +1;
            break;
        }
        v.push_back (snippet(sid, lid, kid));
    }
    return v;
}
