#ifndef CHANGEINTERESTRATEWIZ_H
#define CHANGEINTERESTRATEWIZ_H

#include "helper.h"


struct wpChangeInterestRate_ValuePage : public QWizardPage
{
    wpChangeInterestRate_ValuePage(QWidget* parent =nullptr);
    void cleanupPage() override  {};
    void initializePage() override;
    bool validatePage() override;
    int nextId() const override;
    Q_OBJECT;

private:
    QLabel *subTitleLabel = nullptr;
};

struct wpChangeInterestRate_DatePage : public QWizardPage
{
    wpChangeInterestRate_DatePage(QWidget* parent =nullptr);
    void cleanupPage() override  {};
    void initializePage() override;
    bool validatePage() override;
    int nextId() const override;
    Q_OBJECT;

private:
    QLabel *subTitleLabel = nullptr;
};

class wpChangeInterestRate_Summary : public QWizardPage{
    Q_OBJECT;
public:
    wpChangeInterestRate_Summary(QWidget* p =nullptr);
    void initializePage() override;
    bool isComplete() const override;
public slots:
    void onConfirmData_toggled(int);

private:
    QLabel *subTitleLabel = nullptr;
};

struct wizChangeInterestRate : public QWizard
{
    wizChangeInterestRate(QWidget* p =nullptr);
    QString creditorName;
    QString contractLabel;
    double  currentValue = 0.;
    double newValue = 0.;
    double maxValue = 0.;
    QDate earlierstDate;
    QDate date;
    Q_OBJECT;
};


#endif // CHANGEINTERESTRATEWIZ_H
