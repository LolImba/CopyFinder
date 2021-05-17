#ifndef DIRDIALOG_H
#define DIRDIALOG_H

#include <QFileDialog>



class DirDialog : public QFileDialog
{
    Q_OBJECT
public:
    DirDialog();

private:
    void closeEvent(QCloseEvent *bar);
};

#endif // DIRDIALOG_H
