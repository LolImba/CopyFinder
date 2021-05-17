#include "dirdialog.h"

DirDialog::DirDialog():QFileDialog()
{

}

void DirDialog::closeEvent(QCloseEvent *bar)
{
    reject();
}
