
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
                             "da davor bereits Zinsen verbucht sind.")).arg(wiz->earliestDate.toString(qsl("dd.MM.yyyy")));

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
    return page_interest_selection_Mode;
}


///////////////////////////////////////////
/// wizChangeInterestRate
///////////////////////////////////////////

wizChangeInterest::wizChangeInterest(creditor& c, QWidget* p) : wizNew::wizNew(c,p)
{
    // QFont f = font();
    // f.setPointSize(10);
    // setFont(f);
    // Add new start dialog
    setPage(page_date_of_change, new wpChangeInterest_DatePage);
    setStartId(page_date_of_change);
    updateMode = true;
}
