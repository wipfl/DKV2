#ifndef CHANGEINTERESTRATEWIZ_H
#define CHANGEINTERESTRATEWIZ_H

#include "helper.h"
#include "contract.h"
#include "wiznew.h"

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


struct wizChangeInterest : public wizNew
{
    wizChangeInterest(QWidget* p =nullptr);
    QString creditorName;
    QString contractLabel;
    double  currentValue = 0.;
    double newValue = 0.;
    double maxValue = 0.;
    interestModel iModel = interestModel::zero;
    
    QDate earliestDate;
    QDate date;
    Q_OBJECT;
};


#endif // CHANGEINTERESTRATEWIZ_H
