
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
    subTitleLabel = new QLabel(qsl(""));
    subTitleLabel->setWordWrap(true);

    QDateEdit *de = new QDateEdit;
    de->setDisplayFormat(qsl("dd.MM.yyyy"));
    registerField(pnChangeDate, de);

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
    setField(pnChangeDate, wiz->earliestDate);
}

bool wpChangeInterest_DatePage::validatePage()
{
    wizChangeInterest* wiz= qobject_cast<wizChangeInterest*>(this->wizard());
    QDate d {field(pnChangeDate).toDate()};
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

    setField(pnChangeDate, d);
    setField(pnCDate, d);
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

/*
* wpChangeInterest_ConfirmPage  -confirm the change data
*/
wpChangeInterest_ConfirmPage::wpChangeInterest_ConfirmPage(QWidget *p) : QWizardPage(p)
{
    LOG_CALL;
    setTitle(qsl("Bestätige die Vertragsdaten"));
    QCheckBox *cbConfirm = new QCheckBox(qsl("Die Angaben sind korrekt!"));
    cbConfirm->setCheckState(Qt::CheckState::Unchecked);
    registerField(pnConfirmChange + qsl("*"), cbConfirm);

    QVBoxLayout *bl = new QVBoxLayout;
    bl->addWidget(subTitleLabel);
    bl->addWidget(cbConfirm);
    setLayout(bl);
    connect(cbConfirm, &QCheckBox::stateChanged, this, &wpChangeInterest_ConfirmPage::onConfirmChangeInterest_toggled);
    onConfirmChangeInterest_toggled(false);
}

void wpChangeInterest_ConfirmPage::onConfirmChangeInterest_toggled(int state)
{
    LOG_CALL;
    wizChangeInterest *wiz = qobject_cast<wizChangeInterest *>(wizard());
    setFinalPage(state);

    if (wiz)
    {
        wiz->button(QWizard::CommitButton)->hide();
        wiz->button(QWizard::NextButton)->hide();
        wiz->button(QWizard::FinishButton)->show();
    }
}

void wpChangeInterest_ConfirmPage::initializePage()
{
    LOG_CALL;

    QString summary{qsl("Vertrag <b>%3</b> von <b>%1 %2</b> <p><table>"
                        "<tr><td>Betrag: </td><td><b>%4</b><br/></td></tr>"
                        "<tr><td>Änderungsdatum: </td><td><b>%5</b></td></tr>"
                        "<tr><td>Zinssatz: </td><td><b>%6 %</b></td></tr>"
                        // "<tr><td>Zinsanrechnung: </td><td><b>%6<p></b></td></tr>"
                        "</table>")};
    QLocale l;

    wizChangeInterest *wiz = qobject_cast<wizChangeInterest *>(wizard());
    if (wiz)
    {
        interestModel iMode{wiz->iPaymentMode};
        QString interestMode = interestModelDisplayString(iMode);
        subTitleLabel->setText(
            summary.arg(field(pnFName).toString(), field(pnLName).toString(), field(pnLabel).toString(), 
            d2euro(l.toDouble (field(pnAmount).toString())),
            field(pnChangeDate).toDate().toString(qsl("dd.MM.yyyy")), 
            QString::number(wiz->interest / 100., 'f', 2), 
            interestMode));

        // setCommitPage(true);
 
    }
    else
        Q_ASSERT(false);
}

///////////////////////////////////////////
/// wizChangeInterest
///////////////////////////////////////////

wizChangeInterest::wizChangeInterest(creditor& c, contract* contr, QWidget* p) : wizNew::wizNew(c,p)
{
    setPage(page_date_of_change, new wpChangeInterest_DatePage(this));
    setPage(page_confirm_change_interest, new wpChangeInterest_ConfirmPage(this));
    setStartId(page_date_of_change);

    updateMode = true;
    setField(pnFName, c.firstname());
    setField(pnLName, c.lastname());
    setField(pnStreet, c.street());
    setField(pnPcode, c.postalCode());
    setField(pnCity, c.city());
    setField(pnCountry, c.country());
    setField(pnEMail, c.email());
    setField(pnPhone, c.tel());
    setField(pnContact, c.contact());
    setField(pnComment, c.comment());
    setField(pnIban, c.iban());
    setField(pnBic, c.bic());
    setField(pnAccount, c.account());

    setField(pnLabel, contr->label());
    setField(pnAmount, contr->plannedInvest());
    setField(pnContractComment, contr->comment());
    setField(pnCDate, contr->conclusionDate());
    setField(pnEDate, contr->plannedEndDate());
    setField(pnPeriod, contr->noticePeriod());

}
