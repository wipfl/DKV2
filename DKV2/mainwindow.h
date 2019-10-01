#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <dkdbhelper.h>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void FillKreditorDropdown();
    void FillRatesDropdown();
    void comboKreditorenAnzeigeNachKreditorenId(int id);
    int getPersonIdFromKreditorenList();
    QDate getContractDateFromContractsList();
    int getContractIdStringFromContractsList();
    void setContractFromUi(VertragsDaten& c);
    
private slots:

    void on_action_Neue_DB_anlegen_triggered();
    void on_actionProgramm_beenden_triggered();
    void on_actionDBoeffnen_triggered();

    void on_action_Liste_triggered();
    void on_actioncreateSampleData_triggered();
    void on_actionNeuer_DK_Geber_triggered();
    void on_actionVertrag_anlegen_triggered();
    void on_saveExit_clicked();
    void on_saveNew_clicked();

    void on_saveList_clicked();
    void on_cancel_clicked();

    void on_stackedWidget_currentChanged(int arg1);

    // Buttons on the "Vertrag anlegen" page
    void on_cancelCreateContract_clicked();

    void on_speichereVertragZurKreditorenListe_clicked();

    void on_saveContractGoContracts_clicked();

    void on_actionListe_der_Vertr_ge_anzeigen_triggered();

    void on_PersonsTableView_customContextMenuRequested(const QPoint &pos);

    void on_actionKreditgeber_l_schen_triggered();

    void on_contractsTableView_customContextMenuRequested(const QPoint &pos);

    void on_actionactivateContract_triggered();

    void on_actionVertrag_l_schen_triggered();

    void on_actionanzeigenLog_triggered();

private:
    Ui::MainWindow *ui;

    void preparePersonTableView();
    void prepareContractListView();
    void setCurrentDbInStatusBar();
    bool savePerson();
    bool saveNewContract();

    enum stackedWidgedsIndex
    {
        emptyPageIndex =0,
        PersonListIndex =1,
        newPersonIndex =2,
        newContractIndex =3,
        ContractsListIndex =4
    };
    void clearEditPersonFields();
    void clearNewContractFields();
};

#endif // MAINWINDOW_H