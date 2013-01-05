#include "code128.h"

QString encodeToCode128(const QString& src)
{
    int ind;
    int checksum = 0;
    int mini;
    int dummy;
    bool tableB;
    QString code128;
    int srcLength;

    srcLength = src.length();

    tableB = true;
    ind = 0;

    while (ind < srcLength)
    {

        if (tableB)
        {
            if ((ind == 0) || (ind + 3 == srcLength-1))
            {
                mini = 4;
            }
            else
            {
                mini = 6;
            }

            mini = mini - 1;

            if ((ind + mini) <= srcLength - 1)
            {
                while (mini >= 0)
                {
                    if ((src[ind + mini].unicode() < 48) || (src[ind + mini].unicode() > 57))
                    {
                        break;
                    }
                    mini = mini - 1;
                }
            }


            if (mini < 0)
            {
                if (ind == 0)
                {
                    code128 = QChar((char) 205);

                }
                else
                {
                    code128.append(QChar((char)199));

                }
                tableB = false;
            }
            else
            {

                if (ind == 0)
                {
                    code128 = QChar((char) 204);
                }
            }
        }

        if (tableB == false)
        {
            mini = 2;
            mini = mini - 1;
            if (ind + mini < srcLength)
            {
                while (mini >= 0)
                {

                    if (((src[ind + mini]) < 48) || ((src[ind]) > 57))
                    {
                        break;
                    }
                    mini = mini - 1;
                }
            }
            if (mini < 0)
            {
                dummy = src.mid(ind, 2).toInt();

                if (dummy < 95)
                {
                    dummy = dummy + 32;
                } else
                {
                    dummy = dummy + 100;
                }
                code128 = code128 + (char)(dummy);

                ind = ind + 2;
            }
            else
            {
                code128.append(QChar((char) 200));
                tableB = true;
            }
        }
        if (tableB == true)
        {

            code128 = code128 + src[ind];
            ind = ind + 1;
        }
    }

    for (ind = 0; ind < code128.length(); ind++)
    {
        dummy = code128[ind].unicode();
        if (dummy < 127)
        {
            dummy = dummy - 32;
        }
        else
        {
            dummy = dummy - 100;
        }
        if (ind == 0)
        {
            checksum = dummy;
        }
        checksum = (checksum + (ind) * dummy) % 103;
    }

    if (checksum < 95)
    {
        checksum = checksum + 32;
    }
    else
    {
        checksum = checksum + 100;
    }
    code128.append(QChar((char) checksum));

    return code128;

}

ushort valChar(const QChar& c) {
    ushort code = c.unicode();
    if (32 <= code && code <= 126) return code - 32;
    if (195 <= code && code <= 211) return code - 100;
    return 0;
}

const int tblC128[] = {
    0x019B,    0x01B3,    0x0333,    0x00C9,    0x0189,    0x0191,
    0x0099,    0x0119,    0x0131,    0x0093,    0x0113,    0x0123,
    0x01CD,    0x01D9,    0x0399,    0x019D,    0x01B9,    0x0339,
    0x0273,    0x01D3,    0x0393,    0x013B,    0x0173,    0x03B7,
    0x0197,    0x01A7,    0x0327,    0x0137,    0x0167,    0x0267,
    0x00DB,    0x031B,    0x0363,    0x00C5,    0x00D1,    0x0311,
    0x008D,    0x00B1,    0x0231,    0x008B,    0x00A3,    0x0223,
    0x00ED,    0x038D,    0x03B1,    0x00DD,    0x031D,    0x0371,
    0x0377,    0x038B,    0x03A3,    0x00BB,    0x023B,    0x03BB,
    0x00D7,    0x0317,    0x0347,    0x00B7,    0x0237,    0x02C7,
    0x02F7,    0x0213,    0x028F,    0x0065,    0x0185,    0x0069,
    0x0309,    0x01A1,    0x0321,    0x004D,    0x010D,    0x0059,
    0x0219,    0x0161,    0x0261,    0x0243,    0x0053,    0x02EF,
    0x0143,    0x02F1,    0x01E5,    0x01E9,    0x03C9,    0x013D,
    0x0179,    0x0279,    0x012F,    0x014F,    0x024F,    0x03DB,
    0x037B,    0x036F,    0x00F5,    0x03C5,    0x03D1,    0x00BD,
    0x023D,    0x00AF,    0x022F,    0x03DD,    0x03BD,    0x03D7,
    0x03AF,    0x010B,    0x004B,    0x01CB,    0x1AE3};

void dessineBloc(qint32 Data, QPainter& painter, QPointF& point, qreal H, qreal Mul, int Nb)
{
    int i, N;
    QPointF newPoint = point;
    bool B1 = Data%2 == 1;
    QRectF Rec;

    i = 0;
    while (i < Nb) {
        N = i;
        do {
            ++i;
            Data >>= 1;
        } while ((i < Nb) && (B1 == (Data%2 == 1)));
        N = i - N;
        B1 = !B1;
        Rec.setLeft(newPoint.x());
        newPoint.setX(newPoint.x() + N*Mul);
        Rec.setRight(newPoint.x());
        Rec.setTop(newPoint.y());
        Rec.setBottom(newPoint.y() + H);
        if (!B1) {
            painter.fillRect(Rec, painter.brush());
        }
    }
    point = newPoint;
}

void Code128::drawBarcode(const QString &src, QPainter& painter, const QRectF& rect) {
    QString res = encodeToCode128(src);
    // barcode stroke width
    qreal Mul = rect.width()/(11*res.length() + 13);
    QPointF startPoint = rect.topLeft();
    for (int i = 0; i < res.length(); ++i) {
        dessineBloc(tblC128[valChar(res.at(i))], painter, startPoint, rect.height(), Mul, 11);
    }
    dessineBloc(0x1AE3, painter, startPoint, rect.height(), Mul, 13);
}
