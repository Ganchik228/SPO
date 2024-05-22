#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = new DataBase();
    db->connectToDataBase();
    this->setupModel(TABLE_EMP, QStringList() << "id сотрудника"
                                                            << "ФИО"
                                                            << "Дата рождения"
                                                            << "Пол"
                                                            << "Должность"
                                                            << "Организация"
                                                            << "ИНН");



    this->createUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupModel(const QString &tableName, const QStringList &headers)
{
    model = new QSqlRelationalTableModel(this);
    model->setTable(tableName);
    model->setRelation(4, QSqlRelation(TABLE_POST, POST_ID, POST_NAME));
    model->setRelation(5, QSqlRelation(TABLE_ORGA, ORGA_ID, ORGA_NAME));
    model->setRelation(7, QSqlRelation(TABLE_ORGA, ORGA_ID, ORGA_INN));

    qDebug() << "Number of columns in model:" << model->columnCount();

    for (int i = 0, j = 0; i < model->columnCount(); i++, j++)
    {
        qDebug() << "Header for column" << i << ":" << headers[j];
        model->setHeaderData(i, Qt::Horizontal, headers[j]);
    }

    model->select();
}


void MainWindow::createUI()
{
    ui->tableView->setModel(model);
    ui->tableView->setColumnHidden(0, true);
    ui->tableView->setColumnHidden(6, false);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setItemDelegate(new QSqlRelationalDelegate(ui->tableView));
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    for (int i = 0; i < model->columnCount(); ++i) {
        qDebug() << "Column" << i << "width:" << ui->tableView->columnWidth(i);
    }

    model->select();
}

void MainWindow::on_checkBox_stateChanged(int state)
{
    ui->tableView->setColumnHidden(0, !state);
}

void MainWindow::on_btnexit_clicked()
{
    this->close();
}

void MainWindow::on_btnabout_clicked()
{
    QMessageBox msgBox;
    msgBox.information(this, "О программе", "База данных, варианта номер 7");
}


