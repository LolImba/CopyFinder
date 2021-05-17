#ifndef COPYFINDER_H
#define COPYFINDER_H

#include <QFile>
#include <QObject>
#include <QTreeWidget>
#include "fileinfo.h"
#include "item.h"

Q_DECLARE_METATYPE(QList<QTreeWidgetItem *>)

class CopyFinder : public QObject
{
    Q_OBJECT
public:
    explicit CopyFinder(QString &dir1, QString dir2, QObject *parent = nullptr);
   // void findCopies(const QString& dir1, const QString& dir2, QList<QTreeWidgetItem *> &out);

signals:
    void resultReady(QList<QTreeWidgetItem *> out);
    void endThread();

public slots:
    void createFileMap(const QString& dir,  QMultiMap<quint64, FileInfo*>& out);
    void findCopies();

private:
    bool fileComprassion(FileInfo& firstFileInfo, FileInfo& secondFileInfo);
 //   bool fileComprassionTest(FileInfo& firstFileInfo, FileInfo& secondFile);
    void createItemList(QMap<QString, QFile*>& files1, QMap<QString, QFile*>& files2, QList<Item>& items);
    void createCopyList(QList<Item>& items);
private:
    QString dir1;
    QString dir2;
};

#endif // COPYFINDER_H
