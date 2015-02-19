#ifndef SCANNERMANAGER_H
#define SCANNERMANAGER_H

#include <QString>
#include <QList>

struct ScannerManagerListener {
    virtual void onTextScanned(const QString&) { }
    virtual void onQuestionFixed(int) { }
};

class ScannerManager
{
public:
    static ScannerManager *instance();

    void addListener(ScannerManagerListener *l) {
        clients.push_back(l);
    }

    void removeListener(ScannerManagerListener *l) {
        clients.removeAll(l);
    }

    void textScanned(const QString& text);
    void fixQuestion(int number);

private:
    qint64 prevTime;
    QString prevText;
    ScannerManager() {}
    
    QList<ScannerManagerListener*> clients;
};

#endif // SCANNERMANAGER_H
