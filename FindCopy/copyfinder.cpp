#include "copyfinder.h"

#include <QCryptographicHash>
#include <QDirIterator>
#include <QElapsedTimer>
#include <QFuture>
#include <QTime>
#include "item.h"
#include "QDebug"
#include "fileinfo.h"
#include <QtConcurrent/QtConcurrentMap>
#include <QtConcurrent/QtConcurrentRun>

CopyFinder::CopyFinder(QString &dir1, QString dir2, QObject *parent) : QObject(parent)
{
    this->dir1 = dir1;
    this->dir2 = dir2;
    qRegisterMetaType<QList<QTreeWidgetItem *>>();
}


void CopyFinder::findCopies()    //Main function for find copies
{
    QList<QTreeWidgetItem *> out;
       QElapsedTimer timerf;
       timerf.start();
       QMultiMap<quint64, FileInfo*> files1;

       QFuture<void> future1 = QtConcurrent::run([&](){
       QDirIterator it(dir1, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
       while (it.hasNext()) {
           QString filePath = it.next();
           QFile *file = new QFile(filePath);
           FileInfo* fileInfo = new FileInfo(file, filePath);

           files1.insert(file->size(), fileInfo);
       }
        });

       QMultiMap<quint64, FileInfo*> files2;
       QFuture<void> future2 = QtConcurrent::run([&](){
           QDirIterator it2(dir2, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
           while (it2.hasNext()) {
               QString filePath = it2.next();
               QFile *file = new QFile(filePath);
               FileInfo* fileInfo = new FileInfo(file, filePath);
               files2.insert(file->size(), fileInfo);
           }
       });
       future1.waitForFinished();
       future2.waitForFinished();
       qDebug() << "Maps created in: " << timerf.elapsed() << " milliseconds";
       QElapsedTimer timer;
       timer.start();
       QList<Item> items;
       int itemIndex = 0;
       auto keys = files1.uniqueKeys();
       int iterations = 0;
       int index = -1;
       QMutex mutexKeyIndex;
       QMutex mutexItemsVec;
       std::function<void(const quint64&)> func = [&](const quint64& key){
           Item item;
           FileInfo* first = files1.values(key).first();
           item.path = first->path();
               if(files2.contains(key))
               {
                   auto fileList2 = files2.values(key);
                   auto fileList1 = files1.values(key);

                   auto j = fileList1.begin();
                   FileInfo* target = *fileList1.begin();
                   bool copyFinded = false;

                   while (true) {
                       iterations++;
                       bool itemAdded = false;
                       if(!copyFinded){
                           int index = 0;
                           for (auto k = fileList2.begin(); k != fileList2.end();) {
                               if(fileComprassion(**j, **k))
                               {
                                   if(!itemAdded)
                                   {
                                       mutexItemsVec.lock();
                                       items.push_back(item);
                                       mutexItemsVec.unlock();

                                       itemIndex = items.size() - 1;
                                       items[itemIndex].copies.push_back((*k)->path());
                                       fileList2.erase(k);
                                       itemAdded = true;
                                   }
                                   else
                                   {
                                       items[itemIndex].copies.push_back((*k)->path());
                                       fileList2.erase(k);
                                   }
                                   copyFinded = true;
                                   k = fileList2.begin() + index;
                               }
                               else{
                                   k++;
                                   index++;
                               }
                           }
                           if(copyFinded)
                           {
                               target = *j;
                               int idx = std::distance(fileList1.begin(), j);
                               fileList1.erase(j);
                               j = fileList1.begin() + idx;
                           }
                           else {
                               j++;
                               copyFinded = false;
                               itemAdded = false;
                           }
                       }
                       else {
                           if(j != fileList1.end()){
                               if(fileComprassion(*target, **j))
                               {
                                   items[itemIndex].copies.push_back((*j)->path());
                                   int idx = std::distance(fileList1.begin(), j);
                                   fileList1.erase(j);
                                   j = fileList1.begin() + idx;
                               }
                               else {
                                   j++;
                               }
                           }
                           else {

                           }

                       }
                       if(j == fileList1.end())
                       {

                           if(!copyFinded || fileList1.size() == 0)
                           {
                               break;
                           }
                           else {
                               copyFinded = false;
                               j = fileList1.begin();
                           }
                       }
                   }
               }
       };
       QFuture<void> cf = QtConcurrent::map(keys, func);

       cf.waitForFinished();
           for (int i = 0; i < items.size(); i++) {                //create tree list
               out.push_back(new QTreeWidgetItem());
               out[i]->setText(0, items[i].path);
               for (int j = 0; j < items[i].copies.size();  j++) {
                   QTreeWidgetItem *itemCopy = new QTreeWidgetItem();
                   itemCopy->setText(0,items[i].copies[j]);
                   out[i]->addChild(itemCopy);
               }
           }
           //auto time = timer.elapsed();
           qDebug() << "The operation took" << timer.elapsed() << "milliseconds"
                    << " And there is " << items.size() << " copies" << " Iterations count: " << iterations
                    << " Files Count: " << files1.count() << " Unique Keys Count: " << keys.count();
           qDeleteAll(files1);
           qDeleteAll(files2);

           emit resultReady(out);
           emit endThread();
}

void CopyFinder::createFileMap(const QString& dir,  QMultiMap<quint64, FileInfo*>& out)
{
    QDirIterator it(dir, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filePath = it.next();
        QFile *file = new QFile(filePath);
        FileInfo* fileInfo = new FileInfo(file, filePath);
        out.insert(file->size(), fileInfo);
    }
}


bool CopyFinder::fileComprassion(FileInfo& firstFileInfo, FileInfo& secondFileInfo)
{
    QByteArray sig = firstFileInfo.getHash().result();
    QByteArray sig2 = secondFileInfo.getHash().result();

    if(sig.size() != sig2.size())
        return false;
    for (int i = 0; i < sig.size(); i++) {
        if(sig[i] != sig2[i])
            return false;
    }
    return true;
}

