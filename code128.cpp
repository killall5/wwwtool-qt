#include "code128.h"

const QString BarArray[] = {
    "155", "515", "551", "449", "485", "845", "458", "494", "854",
    "548", "584", "944", "056", "416", "452", "065", "425", "461", "560", "506",
    "542", "164", "524", "212", "245", "605", "641", "254", "614", "650", "119",
    "191", "911", "089", "809", "881", "098", "818", "890", "188", "908", "980",
    "01:", "092", "812", "029", "0:1", "821", "221", "182", "902", "128", "1:0",
    "122", "209", "281", ":01", "218", "290", ":10", "230", "5<0", ";00", "04=",
    "0<5", "40=", "4<1", "<05", "<41", "05<", "0=4", "41<", "4=0", "<14", "<50",
    "=40", "50<", "320", "=04", "830", "047", "407", "443", "074", "434", "470",
    "344", "704", "740", "113", "131", "311", "00;", "083", "803", "038", "0;0",
    "308", "380", "023", "032", "203", "302", "A", "B", "C", "@"
};



QString Code128::encodeToCode128(const QString& src)
{
    int i, BInd, CCode, BCode[1024];
    QChar ch, ch2, CurMode;

    //Собираем строку кодов
    BInd = 0;
    CurMode = 0;
    for (i = 0; i < src.length(); ) {
      //Текущий символ в строке
      ch = src.at(i);
      ++i;
      //Разбираем только символы от 0 до 127
      if (ch.unicode() <= 127) {
        //Следующий символ
        if (i < src.length()) {
          ch2 = src.at(i);
        } else {
          ch2 = 0;
        }

        //Пара цифр - режим С

        if (ch.isDigit() && ch2.isDigit()) {
          ++i;
          if (BInd == 0) {
            //Начало с режима С
            CurMode = 'C';
            BCode[BInd] = 105;
            ++BInd;
          } else if (CurMode != 'C') {
            //Переключиться на режим С
            CurMode = 'C';
            BCode[BInd] = 99;
            ++BInd;
          }
          //Добавить символ режима С
          BCode[BInd] = ch.digitValue()*10 + ch2.digitValue();
          ++BInd;
        } else {
          if (BInd == 0) {
            if (ch < ' ') {
              //Начало с режима A
              CurMode = 'A';
              BCode[BInd] = 103;
              ++BInd;
            } else {
              //Начало с режима B
              CurMode = 'B';
              BCode[BInd] = 104;
              ++BInd;
            }
          }
          //Переключение по надобности в режим A
          if ((ch < ' ') && (CurMode != 'A')) {
            CurMode = 'A';
            BCode[BInd] = 101;
            ++BInd;
          //Переключение по надобности в режим B
          } else if ((('@' <= ch) && (CurMode != 'B')) || (CurMode == 'C')) {
            CurMode = 'B';
            BCode[BInd] = 100;
            ++BInd;
          }
          //Служебные символы
          if (ch < ' ') {
            BCode[BInd] = ch.unicode() + 64;
            ++BInd;
          //Все другие символы
          } else {
            BCode[BInd] = ch.unicode() - 32;
            ++BInd;
          }
        }
      }
    }

    //Подсчитываем контрольную сумму
    CCode = BCode[0] % 103;
    for (i = 1; i < BInd; ++i) {
        CCode = (CCode + BCode[i] * i) % 103;
    }
    BCode[BInd] = CCode;
    ++BInd;
    //Завершающий символ
    BCode[BInd++] = 106;
    ++BInd;
    //Собираем строку символов шрифта
    QString Result;
    for (i = 0; i < BInd; ++i) {
      Result.append(BarArray[BCode[i]]);
    }
    return Result;
}
