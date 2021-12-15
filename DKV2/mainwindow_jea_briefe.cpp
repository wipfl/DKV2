#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "pch.h"

#include "letter.h"

letter* Letter =nullptr;

void writeDefaultSnippetsIfNeeded()
{
    QString tableName {snippet::getTableDef ().Name ()};
    ensureTable(tableName);
    if( rowCount(tableName) <1) {
        writeDefaultSnippets ();
    }
}

void MainWindow::on_action_JEA_Briefe_triggered()
{   LOG_CALL;
    busycursor b;
    QVector<int> years =abrechnungsJahre ();
    if( years.count () == 0) {
        QMessageBox::information (this, qsl("keine Daten"), qsl("Es wurden keine Jahresabrechnungen gefunden"));
        return;
    }
    static bool need_check =true;
    if( need_check)
        writeDefaultSnippetsIfNeeded();
    need_check =false;
    prepare_jea_briefe_page (years);
    ui->stackedWidget->setCurrentIndex (printPreviewPageIndex);
}

void MainWindow::prepare_jea_briefe_page(const QVector<int>& years)
{   LOG_CALL;
    ui->cbYear->clear ();
    for(const auto year :qAsConst(years)) {
        ui->cbYear->addItem (QString::number(year), year);
    }
    ui->cbYear->setCurrentIndex (0);

}

void MainWindow::on_cbYear_currentIndexChanged(int year)
{   LOG_CALL;
    ui->cbKennungen->clear();
    QVector<QPair<qlonglong, QString>> kennungen =bookingIdContractlabel_fromYear(ui->cbYear->itemData (year).toInt ());
    for(const auto & kennung : qAsConst(kennungen)) {
        ui->cbKennungen->addItem (kennung.second, kennung.first);
    }
    ui->cbKennungen->setCurrentIndex (0);
    ui->btnPrevBooking->setEnabled (false); // we start at the firsrt
    if( kennungen.count()>1)
        ui->btnNextBooking->setEnabled (true);
    else
        ui->btnNextBooking->setEnabled (false);
}

void MainWindow::on_cbKennungen_currentIndexChanged(int index)
{   LOG_CALL_W(QString::number(index));
    int maxIndex     =ui->cbKennungen->count () -1;
    if( index == 0)
        ui->btnPrevBooking->setEnabled (false);
    else
        ui->btnPrevBooking->setEnabled (true);

    if( index == maxIndex)
        ui->btnNextBooking->setEnabled (false);
    else
        ui->btnNextBooking->setEnabled (true);

    qlonglong bookingId =ui->cbKennungen->itemData (index ).toLongLong ();
    qDebug() << "Brief für Vertrag m Kennung " << ui->cbKennungen->itemText (index)
             << " mit Buchungs Id " <<  bookingId;
    prepare_printPreview(bookingId);
}

void MainWindow::on_btnNextBooking_clicked()
{   LOG_CALL;
    int current =ui->cbKennungen->currentIndex ();
    int maxIndex     =ui->cbKennungen->count () -1;
    if( current not_eq maxIndex)
        ui->cbKennungen->setCurrentIndex (current +1);
}

void MainWindow::on_btnPrevBooking_clicked()
{   LOG_CALL;
    int current =ui->cbKennungen->currentIndex ();
    if( current not_eq 0)
        ui->cbKennungen->setCurrentIndex (current -1);
}

void MainWindow::prepare_printPreview(qlonglong bookingId)
{   LOG_CALL;
    if( Letter not_eq nullptr)
        delete Letter;
    Letter =new letter(booking(bookingId));
    ui->txtAnrede->setPlainText (Letter->snippets[(int)snippetType::greeting]);
    ui->txtGruss->setPlainText (Letter->snippets[(int)snippetType::salut]);
    ui->txtText1->setPlainText (Letter->snippets[(int)snippetType::text1]);
    ui->txtText2->setPlainText (Letter->snippets[(int)snippetType::text2]);
}


void MainWindow::on_btnUpdatePreview_clicked()
{
    ui->wPreview->updatePreview();
}

void MainWindow::doPaint(QPrinter* pri)
{
    QPainter p(dynamic_cast<QPaintDevice*>(pri));
    p.drawText(QPoint(100, 100), qsl("Hallo World"));

    // Logo
    // Adresse
    // Datum
    // Anrede
    // Fußzeile
    // text(e)
}
