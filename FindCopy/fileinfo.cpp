#include "fileinfo.h"

FileInfo::FileInfo(QFile *file, const QString &filePath):hash(QCryptographicHash::Md5)
{
    this->file = file;
    fPath = filePath;
    fSize = file->size();
    hasHash = false;
}

FileInfo::~FileInfo()
{
    if(file != nullptr)
        delete file;
}

void FileInfo::createHash()
{
    if(!file->open(QIODevice::ReadOnly))
        return;
    hash.addData(file->readAll());
    file->close();
//    hashResult = hash.result();
    hasHash = true;
}

QCryptographicHash &FileInfo::getHash()
{
    if(hasHash)
        return hash;
    else {
        createHash();
        return hash;
    }
}
