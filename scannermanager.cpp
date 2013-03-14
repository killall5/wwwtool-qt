#include "scannermanager.h"

static ScannerManager *m_instance = 0;

ScannerManager *ScannerManager::instance()
{
       if (!m_instance) {
           m_instance = new ScannerManager;
       }
       return m_instance;
}
