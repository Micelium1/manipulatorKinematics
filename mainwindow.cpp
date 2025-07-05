#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "validatedcellwidget.h"
#include <QMessageBox>
#include <QGenericMatrix>
#include <QSharedPointer>

QGenericMatrix<4,4,double> createAMatrix(double theta,double a, double d, double alpha);



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->table->setItemDelegate(new ValidatedCellWidget(ui->table));

    for(int i = 0;i <= 5; ++i) {
        addButton_clicked();
    }
    ui->table->setItem(0,1,new QTableWidgetItem(QString::number(15)));
    ui->table->setItem(1,1,new QTableWidgetItem(QString::number(-50)));
    ui->table->setItem(2,1,new QTableWidgetItem(QString::number(-60)));
    ui->table->setItem(3,1,new QTableWidgetItem(QString::number(95)));
    ui->table->setItem(4,1,new QTableWidgetItem(QString::number(50)));
    ui->table->setItem(5,1,new QTableWidgetItem(QString::number(0)));

    ui->table->setItem(0,2,new QTableWidgetItem(QString::number(0)));
    ui->table->setItem(1,2,new QTableWidgetItem(QString::number(-0.8)));
    ui->table->setItem(2,2,new QTableWidgetItem(QString::number(-0.59)));
    ui->table->setItem(3,2,new QTableWidgetItem(QString::number(0)));
    ui->table->setItem(4,2,new QTableWidgetItem(QString::number(0)));
    ui->table->setItem(5,2,new QTableWidgetItem(QString::number(0)));

    ui->table->setItem(0,3,new QTableWidgetItem(QString::number(0.213)));
    ui->table->setItem(1,3,new QTableWidgetItem(QString::number(0.193)));
    ui->table->setItem(2,3,new QTableWidgetItem(QString::number(-0.16)));
    ui->table->setItem(3,3,new QTableWidgetItem(QString::number(0.25)));
    ui->table->setItem(4,3,new QTableWidgetItem(QString::number(0.28)));
    ui->table->setItem(5,3,new QTableWidgetItem(QString::number(0.25)));

    ui->table->setItem(0,4,new QTableWidgetItem(QString::number(M_PI/2)));
    ui->table->setItem(1,4,new QTableWidgetItem(QString::number(0)));
    ui->table->setItem(2,4,new QTableWidgetItem(QString::number(0)));
    ui->table->setItem(3,4,new QTableWidgetItem(QString::number(M_PI/2)));
    ui->table->setItem(4,4,new QTableWidgetItem(QString::number(-M_PI/2)));
    ui->table->setItem(5,4,new QTableWidgetItem(QString::number(0)));

    connect(ui->addButton,&QPushButton::clicked,this,&MainWindow::addButton_clicked);
    connect(ui->deleteButton,&QPushButton::clicked,this,&MainWindow::deleteButton_clicked);
    connect(ui->table,&QTableWidget::cellChanged,this,&MainWindow::calculateXYZ);
    emit ui->table->cellChanged(0,0);
}

void MainWindow::addButton_clicked() {
    ui->table->insertRow(ui->table->rowCount());
    ui->table->blockSignals(true);
    for(int i = 1,RodsRows = ui->table->columnCount(); i <= RodsRows;++i)
    {
        ui->table->setItem(ui->table->rowCount()-1,i,new QTableWidgetItem("0"));
    }
    ui->table->setItem(ui->table->rowCount()-1,0,new QTableWidgetItem(QString::number(ui->table->rowCount())));
    ui->table->blockSignals(false);
    emit ui->table->cellChanged(0,0);
}

void MainWindow::deleteButton_clicked() {
    int rowsCount = ui->table->rowCount();
    if (!rowsCount)
    {
        QMessageBox::warning(this,"Warning","Нечего удалять");
        return;
    }
    QList<QTableWidgetSelectionRange> deletingObjects= ui->table->selectedRanges();

    if (deletingObjects.isEmpty()) {
        ui->table->removeRow(rowsCount - 1);
        emit ui->table->cellChanged(0,0);
        return;
    }

    for (QTableWidgetSelectionRange range: deletingObjects) {
        for (int bottom = range.bottomRow(),top = range.topRow();bottom >= top;--bottom) {
            ui->table->removeRow(bottom);
        }
    }
    emit ui->table->cellChanged(0,0);
}
QGenericMatrix<4,4,double> createAMatrix(double theta,double a, double d, double alpha) {
    QGenericMatrix<4,4,double> matrix;

    double thetaRads = theta * M_PI / 180;
    double cosTheta = cos(thetaRads);
    double sinTheta = sin(thetaRads);

    double cosAlpha = cos(alpha);
    double sinAlpha = sin(alpha);

    matrix(0,0) = cosTheta;
    matrix(1,0) = sinTheta;
    matrix(2,0) = 0;
    matrix(3,0) = 0;

    matrix(0,1) = -sinTheta * cosAlpha;
    matrix(1,1) = cosTheta * cosAlpha;
    matrix(2,1) = sinAlpha;
    matrix(3,1) = 0;

    matrix(0,2) = sinTheta * sinAlpha;
    matrix(1,2) = -cosTheta * sinAlpha;
    matrix(2,2) = cosAlpha;
    matrix(3,2) = 0;

    matrix(0,3) = a * cosTheta;
    matrix(1,3) = a * sinTheta;
    matrix(2,3) = d;
    matrix(3,3) = 1;
    return matrix;
}
QGenericMatrix<4, 4, double> createIdentityMatrix() {
    QGenericMatrix<4, 4, double> mat;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            mat(i, j) = (i == j) ? 1.0 : 0.0;
        }
    }
    return mat;
}

void MainWindow::calculateXYZ() {
    QGenericMatrix<4,4,double> TMatrix = createIdentityMatrix();
    for (int currentRow = 0; currentRow < ui->table->rowCount(); ++currentRow) {

        double theta = ui->table->item(currentRow,1)->text().replace(",",".").toDouble();
        double a = ui->table->item(currentRow,2)->text().replace(",",".").toDouble();
        double d = ui->table->item(currentRow,3)->text().replace(",",".").toDouble();
        double alpha = ui->table->item(currentRow,4)->text().replace(",",".").toDouble();

        QGenericMatrix<4,4,double> AMatrix = createAMatrix(theta,a,d,alpha);
        TMatrix = TMatrix * AMatrix;

    }
    ui->xLabel->setText(QString("x = %1").arg(TMatrix(0,3)));
    ui->yLabel->setText(QString("y = %1").arg(TMatrix(1,3)));
    ui->zLabel->setText(QString("z = %1").arg(TMatrix(2,3)));
}
MainWindow::~MainWindow()
{
    delete ui;
}
