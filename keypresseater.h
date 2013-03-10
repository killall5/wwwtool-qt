#ifndef KEYPRESSEATER_H
#define KEYPRESSEATER_H

#include <QObject>
#include <QEvent>
#include <QKeyEvent>
#include <QTimer>

class KeyPressEater : public QObject
{
    Q_OBJECT
public:
    explicit KeyPressEater(QObject *parent = 0);
    ~KeyPressEater();
    
protected:
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void keyPressed(QKeyEvent *event);
    void textHaveBeenScanned(const QString& scannedText);
public slots:
    void timerFinished();
private:
    QTimer *timer;
    QString scannedText;
};

#endif // KEYPRESSEATER_H
