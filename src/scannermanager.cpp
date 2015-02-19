#include "scannermanager.h"
#include <QDateTime>

static ScannerManager *m_instance = 0;
static const qint64 MINIMAL_SCANNER_TIME_MILLISECONDS = 4 * 1000;

ScannerManager *ScannerManager::instance()
{
       if (!m_instance) {
           m_instance = new ScannerManager;
       }
       return m_instance;
}

void ScannerManager::textScanned(const QString& text) {
    qint64 dt = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
    if (text == prevText && dt < prevTime + MINIMAL_SCANNER_TIME_MILLISECONDS)
        return;
    foreach(ScannerManagerListener *i, clients) {
        i->onTextScanned(text);
    }
    prevTime = dt;
    prevText = text;
}

void ScannerManager::fixQuestion(int number) {
    foreach(ScannerManagerListener *i, clients) {
        i->onQuestionFixed(number);
    }
}
