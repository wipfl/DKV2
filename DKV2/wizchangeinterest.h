#ifndef CHANGEINTERESTRATEWIZ_H
#define CHANGEINTERESTRATEWIZ_H

#include "helper.h"
#include "contract.h"
#include "wiznew.h"

enum
{
    first_page = page_new_max,
    page_date_of_change
};

inline const QString pnChangeDate{qsl("changeDate")};
inline const QString pnConfirmChange{qsl("confirmChangeInterest")};

struct wpChangeInterest_DatePage : public QWizardPage
{
    wpChangeInterest_DatePage(QWidget* parent =nullptr);
    void cleanupPage() override  {};
    void initializePage() override;
    bool validatePage() override;
    int nextId() const override;
    Q_OBJECT;

private:
    QLabel *subTitleLabel = nullptr;
};

/*
 * wpContractConfirmation get users confirmation
 */
class wpChangeInterest_ConfirmPage : public QWizardPage
{
    Q_OBJECT;
public:
    wpChangeInterest_ConfirmPage(QWidget*);
    void onConfirmChangeInterest_toggled(int state);

    void initializePage() override;

private:
    QLabel *subTitleLabel = new QLabel(qsl(""));
};

struct wizChangeInterest : public wizNew
{
    wizChangeInterest(creditor& cred, contract* cont, QWidget* p =nullptr);
    QString creditorName;
    QString contractLabel;
    double  currentValue = 0.;
    double newValue = 0.;
    double maxValue = 0.;
    interestModel iModel = interestModel::zero;
    QDate earliestDate;
    QDate changeDate;
    bool confirmed = false;
    Q_OBJECT;
};


#endif // CHANGEINTERESTRATEWIZ_H
