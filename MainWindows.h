#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <fstream>
#include "TableAttribute.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_srcButton_clicked();

    void on_fGenerateButton_clicked();

    void on_fPlatform_activated();

private:
    void writeiPhoneDB(QList<TableAttribute> tableAttributes, QString dirName, QString className, QString classNameWithFirstLower, QString tableName);
    void writeBlackberryDB(QList<TableAttribute> tableAttributes, QString dirName, QString className, QString classNameWithFirstLower, QString tableName);
    void writeQtApplicationDB(QList<TableAttribute> tableAttributes, QString dirName, QString className, QString classNameWithFirstLower, QString tableName);
    QString fileName;
    QString tableName;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
