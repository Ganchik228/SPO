#ifndef DATABASE_H
#define DATABASE_H

#include <QDate>
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#define DATABASE_HOSTNAME "ExampleDataBase"
#define DATABASE_NAME "3mydb.db"

#define TABLE_EMP "employees"
#define EMP_ID "employees_id"
#define EMP_FIO "fio"
#define EMP_DR "dr"
#define EMP_GENDER "gender"
#define EMP_POST "post_id"
#define EMP_ORGA "orga_id"
#define EMP_additional_field1 "additional_field1"

#define TABLE_POST "post"
#define POST_ID "post_id"
#define POST_NAME "name"


#define TABLE_ORGA "orga"
#define ORGA_ID "orga_id"
#define ORGA_NAME "name"
#define ORGA_INN "inn"


class DataBase : public QObject
{
    Q_OBJECT
  public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();
    void connectToDataBase();
    bool inserIntoMainTable(const QVariantList &data);
    bool inserIntoDeviceTable(const QVariantList &data);

  private:
    QSqlDatabase db;

  private:
    bool openDataBase();
    bool restoreDataBase();
    void closeDataBase();
    bool createMainTable();
    bool createDeviceTable();
};

#endif   // DATABASE_H
