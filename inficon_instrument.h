#ifndef INFICONINSTRUMENT_H
#define INFICONINSTRUMENT_H

#include <QObject>

class InficonInstrument : public QObject
{
    Q_OBJECT
public:
    explicit InficonInstrument(QObject *parent = nullptr);

signals:

};

#endif // INFICONINSTRUMENT_H
