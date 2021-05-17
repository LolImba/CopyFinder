#ifndef ITEM_H
#define ITEM_H

#include <QVector>



class Item
{
public:
    Item();
    QVector<QString> copies;
    QString path;
    QString name;
};

#endif // ITEM_H
