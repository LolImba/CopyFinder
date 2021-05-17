#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCryptographicHash>
#include <QDirIterator>
#include <QFile>
#include <QFileDialog>
#include "dirdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
  //  qRegisterMetaType<QList<QTreeWidgetItem *>>();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionFind_copies_triggered()
{
    DirDialog fd;
    bool rejected = false;
    connect(&fd, &QDialog::rejected, this, [&]()
    {
        rejected = true;
    });

    fd.setFileMode(QFileDialog::DirectoryOnly);
    fd.setWindowTitle("Select first directory");
    fd.show();
    fd.exec();
    if(!rejected)
    {
        QString directory1 = fd.selectedFiles().at(0);
        rejected = false;
        fd.setWindowTitle("Select second directory");
        fd.show();
        fd.exec();
        if(!rejected)
        {
            QString directory2 = fd.selectedFiles().at(0);

            ui->treeWidget->clear();
           // QList<QTreeWidgetItem *> out;
            QThread* thread = new QThread;
            CopyFinder *copyFinder = new CopyFinder(directory1, directory2);
            copyFinder->moveToThread(thread);
            connect(thread, SIGNAL(started()), copyFinder, SLOT(findCopies()));
            connect(thread, &QThread::finished, thread, &QObject::deleteLater);

           // qRegisterMetaType<QList<QTreeWidgetItem *>>();
            connect(copyFinder, &CopyFinder::resultReady, this, [&](QList<QTreeWidgetItem *> out){
                for (auto item : out) {
                    if(item->childCount() > 0)
                        ui->treeWidget->addTopLevelItem(item);
                }
                ui->treeWidget->expandAll();
               // thread->exit(0);
              //  thread->deleteLater();
            });
            connect(copyFinder, SIGNAL(endThread()),  thread, SLOT(quit()));
            connect(copyFinder, &CopyFinder::endThread, copyFinder, &QObject::deleteLater);
            thread->start();
            //copyFinder->findCopiesTest(directory1, directory2, out);
           // QThread::currentThread()->wait();
        }
    }
}
