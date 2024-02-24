
#include "helper.h"
#include "helperfin.h"
#include "appconfig.h"

#include "wizchangeinterestrate.h"

enum change_value_pages {
    value_page,
    date_page,
/*    imodel_page, */
    summary_page
};

///////////////////////////////////////////
/// wpChangeInterestRate_ValuePage
///////////////////////////////////////////

wpChangeInterestRate_ValuePage::wpChangeInterestRate_ValuePage(QWidget* parent) : QWizardPage(parent)
{
    double maxValue =dbConfig::readValue(MAX_INTEREST).toDouble() / 100.;

    setTitle(qsl("Zinssatz ändern"));



    subTitleLabel = new QLabel(qsl("Keine Daten!"));
    subTitleLabel->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    QLineEdit* le = new QLineEdit;

    QDoubleValidator* dv =new QDoubleValidator(0., maxValue, 2, le);
    le->setValidator(dv);
    registerField(qsl("value"), le);

    layout->addWidget(subTitleLabel);
    layout->addWidget(le);
    setLayout(layout);
}

void wpChangeInterestRate_ValuePage::initializePage()
{
    wizChangeInterestRate* wiz= qobject_cast<wizChangeInterestRate*>(this->wizard());
    double maxValue =dbConfig::readValue(MAX_INTEREST).toDouble() / 100.;
    QLocale l;

    setTitle(qsl("Zinssatz"));
    QString subt = qsl("In dieser Dialogfolge kannst Du den Zinssatz zum Vertrag "
                       "<br/><b>%1</b> "
                       "<br/>von <b>%2</b> ändern."
                       "<br/>"
                       "<br/>Der aktuelle Zinssatz ist %3 ."
                       "<br/>Der maximale Zinssatz ist %4 .");
    subt = subt.arg(wiz->contractLabel, wiz->creditorName, prozent2prozent_str(wiz->currentValue), prozent2prozent_str(maxValue));
    subTitleLabel->setText(subt);
    subTitleLabel->setWordWrap(true);
    setField(qsl("value"), l.toString (wiz->currentValue));
}

bool wpChangeInterestRate_ValuePage::validatePage()
{
    double maxValue =dbConfig::readValue(MAX_INTEREST).toDouble() / 100.;
    QLocale l;

    // cave! QLocale l ist notwendig, damit Werte mit Dezimalkomma (d) richtig "verstanden" werden
    double value = r2(l.toDouble(field(qsl("value")).toString()));

    if( value < 0) {
        QMessageBox::information(this, qsl("Falscher Zinssatz"), qsl("Der Zinssatz darf nicht negativ sein!"));
        return false;
    }

    wizChangeInterestRate* wiz= qobject_cast<wizChangeInterestRate*>(this->wizard());

    if (value > maxValue) {
        QMessageBox::information(this, qsl("Falscher Zinssatz"), 
            qsl("Der Zinssatz darf nicht größer als %1 sein!").arg(prozent2prozent_str(maxValue)));
        return false;
    }

    wiz->newValue = r2(l.toDouble(field(qsl("value")).toString()));
    return true;
}

int wpChangeInterestRate_ValuePage::nextId() const
{
    return date_page;
}

///////////////////////////////////////////
/// wpChangeInterestRate_DatePage
///////////////////////////////////////////

wpChangeInterestRate_DatePage::wpChangeInterestRate_DatePage(QWidget* parent) : QWizardPage(parent)
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

void wpChangeInterestRate_DatePage::initializePage()
{
    wizChangeInterestRate* wiz= qobject_cast<wizChangeInterestRate*>(this->wizard());
    QString subt=QString(qsl("Das früheste Datum für eine Zinsänderung ist der <b>%1</b>), \n"
                             "da davor bereits Zinsen verbucht sind.")).arg(wiz->earlierstDate.toString(qsl("dd.MM.yyyy")));

    setTitle(qsl("Datum der Zinsänderung"));
    subTitleLabel->setText(subt + qsl(  "<br></br>Gib das Datum der Zinsänderung an."
                                        "<br></br>Die Zinsen werden ab dem Folgetag mit dem neuen Zinssatz berechnet!"));
    setField(qsl("date"), wiz->earlierstDate);
}

bool wpChangeInterestRate_DatePage::validatePage()
{
    wizChangeInterestRate* wiz= qobject_cast<wizChangeInterestRate*>(this->wizard());
    QDate d {field(qsl("date")).toDate()};
    QString msg;

    if( d < wiz->earlierstDate)
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

int wpChangeInterestRate_DatePage::nextId() const
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
/// wpChangeInterestRate_iModelPage
///////////////////////////////////////////

wpChangeInterestRate_iModelPage::wpChangeInterestRate_iModelPage(QWidget* parent) : QWizardPage(parent)
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

void wpChangeInterestRate_iModelPage::initializePage()
{
    wizChangeInterestRate* wiz= qobject_cast<wizChangeInterestRate*>(this->wizard());
    QString subt=QString(qsl("Das früheste Datum für eine Zinsänderung ist der <b>%1</b>), \n"
                             "da davor bereits Zinsen verbucht sind.")).arg(wiz->earlierstDate.toString(qsl("dd.MM.yyyy")));

    setTitle(qsl("Datum der Zinsänderung"));
    subTitleLabel->setText(subt + qsl(  "<br></br>Gib das Datum der Zinsänderung an."
                                        "<br></br>Die Zinsen werden ab dem Folgetag mit dem neuen Zinssatz berechnet!"));
    setField(qsl("date"), wiz->earlierstDate);
}

bool wpChangeInterestRate_iModelPage::validatePage()
{
    wizChangeInterestRate* wiz= qobject_cast<wizChangeInterestRate*>(this->wizard());
    QDate d {field(qsl("date")).toDate()};
    QString msg;

    if( d < wiz->earlierstDate)
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

int wpChangeInterestRate_iModelPage::nextId() const
{
    wizChangeInterestRate* wiz= qobject_cast<wizChangeInterestRate*>(this->wizard());

    if (wiz->currentValue == 0. && wiz->newValue != 0. ) {
        return imodel_page;
    }
    return summary_page;
}
#endif

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

///////////////////////////////////////////
/// wizChangeInterestRate
///////////////////////////////////////////

wizChangeInterestRate::wizChangeInterestRate(QWidget* p) : QWizard(p)
{
    QFont f =font(); f.setPointSize(10); setFont(f);
    setPage(date_page, new wpChangeInterestRate_DatePage);
    setPage(value_page, new wpChangeInterestRate_ValuePage);
/*    setPage(imodel_page, new wpChangeInterestRate_iModelPage); */
    setPage(summary_page, new wpChangeInterestRate_Summary);
}
