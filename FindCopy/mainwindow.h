#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "copyfinder.h"
#include <QThread>
#include <QTreeWidget>


//Q_DECLARE_METATYPE(QList<QTreeWidgetItem *>)

namespace Ui {

class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void on_actionFind_copies_triggered();
signals:
    void operate();
};

#endif // MAINWINDOW_H
