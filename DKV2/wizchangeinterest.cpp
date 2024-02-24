
#include "helper.h"
#include "helperfin.h"
#include "appconfig.h"

#include "wizchangeinterest.h"

enum change_value_pages {
    value_page,
    date_page,
/*    imodel_page, */
    summary_page
};


///////////////////////////////////////////
/// wpChangeInterestRate_DatePage
///////////////////////////////////////////

wpChangeInterest_DatePage::wpChangeInterest_DatePage(QWidget* parent) : QWizardPage(parent)
{
    subTitleLabel = new QLabel(qsl("Keine Daten!"));
    subTitleLabel->setWordWrap(true);

    QDateEdit *de = new QDateEdit;
    de->setDisplayFormat(qsl("dd.MM.yyyy"));
    registerField(qsl("date"), de);

    QVBoxLayout*  layout = new QVBoxLayout;
    layout->addWidget(subTitleLabel);
    layout->addWidget(de);
    setLayout(layout);
}

void wpChangeInterest_DatePage::initializePage()
{
    wizChangeInterest* wiz= qobject_cast<wizChangeInterest*>(this->wizard());
    QString subt=QString(qsl("Das früheste Datum für eine Zinsänderung ist der <b>%1</b>), \n"
                             "da davor bereits Zinsen verbucht sind.")).arg(wiz->earlierstDate.toString(qsl("dd.MM.yyyy")));

    setTitle(qsl("Datum der Zinsänderung"));
    subTitleLabel->setText(subt + qsl(  "<br></br>Gib das Datum der Zinsänderung an."
                                        "<br></br>Die Zinsen werden ab dem Folgetag mit dem neuen Zinssatz berechnet!"));
    setField(qsl("date"), wiz->earliestDate);
}

bool wpChangeInterest_DatePage::validatePage()
{
    wizChangeInterest* wiz= qobject_cast<wizChangeInterest*>(this->wizard());
    QDate d {field(qsl("date")).toDate()};
    QString msg;

    if( d < wiz->earliestDate)
        msg =qsl("Das Vertragsdatum muss nach der letzten Buchung liegen");
    // if( d.month() == 12 and d.day() == 31)
    //     // avoid interest bookings on the date of anual settlements.
    //     // it is a holiday anyways
    //     msg += qsl("Eine Ein- oder Auszahlung darf nicht am 31.12. sein");
    if( not msg.isEmpty ()){
        QMessageBox::information (this, "Info", msg);
        return false;
    }

    wiz->date = d;

    return true;
}

int wpChangeInterest_DatePage::nextId() const
{
/*
    wizChangeInterestRate* wiz= qobject_cast<wizChangeInterestRate*>(this->wizard());
    if (wiz->currentValue == 0. && wiz->newValue != 0. ) {
        return imodel_page;
    }
*/
    return summary_page;
}


#if 0
///////////////////////////////////////////
/// wpChangeInterestRate_Summary
///////////////////////////////////////////

wpChangeInterestRate_Summary::wpChangeInterestRate_Summary(QWidget* p) : QWizardPage(p)
{
    setTitle(qsl("Zusammenfassung"));
    subTitleLabel = new QLabel("Keine Daten!");
    subTitleLabel->setWordWrap(true);

    QCheckBox* cb = new QCheckBox(qsl("Die Eingaben sind korrekt!"));
    registerField(qsl("confirmed"), cb);
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(subTitleLabel);
    layout->addWidget(cb);
    setLayout(layout);
    connect(cb, &QCheckBox::stateChanged, this, &wpChangeInterestRate_Summary::onConfirmData_toggled);
}


void wpChangeInterestRate_Summary::initializePage()
{
    wizChangeInterestRate* wiz= qobject_cast<wizChangeInterestRate*>(this->wizard());

    QString subtitle =qsl("zum Vertrag <b>%1</b><p>von <b>%2</b>:<p>"
                      "<table width=100%><tr><td align=center>Vorheriger Zinssatz</td><td align=center>neuer Zinssatz</td></tr>"
                      "<b><tr><td align=center>%3</td><td align=center>%4</td></tr></table>"
                      "<p>Datum: %5</b>");
    double oldValue = wiz->currentValue, newValue =0;
    setTitle(qsl("Zusammenfassung der Zinsänderung"));
    subtitle = qsl("Zinsänderung ") +subtitle;
    newValue = wiz->newValue;
 
    subTitleLabel->setText(subtitle.arg(wiz->contractLabel, wiz->creditorName, 
                            prozent2prozent_str(oldValue), prozent2prozent_str(newValue),
                            field(qsl("date")).toDate().toString(qsl("dd.MM.yyyy"))));
}

bool wpChangeInterestRate_Summary::isComplete() const
{
    return field(qsl("confirmed")).toBool();
}

void wpChangeInterestRate_Summary::onConfirmData_toggled(int)
{
    emit completeChanged();
}
#endif
///////////////////////////////////////////
/// wizChangeInterestRate
///////////////////////////////////////////

wizChangeInterest::wizChangeInterest(QWidget* p) : QWizard(p)
{
    QFont f =font(); f.setPointSize(10); setFont(f);
    setPage(date_page, new wpChangeInterest_DatePage);
}

