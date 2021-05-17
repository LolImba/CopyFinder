#ifndef FILEINFO_H
#define FILEINFO_H

#include <QCryptographicHash>
#include <QFile>

class FileInfo
{
public:
  //  FileInfo();
    explicit FileInfo(QFile *file, const QString &filePath);
  //  FileInfo(FileInfo &other);
    ~FileInfo();
    inline qint64 size() { return fSize; }
    inline QString& path() { return fPath; }
    QCryptographicHash& getHash();
  //  QByteArray& getHashResult();
private:
    void createHash();
    inline bool hashExists() { return hasHash; }
private:
    QCryptographicHash hash;
    QString fPath;
    qint64 fSize;
  //  QByteArray hashResult;
    bool hasHash;
    QFile *file;

};

#endif // FILEINFO_H
