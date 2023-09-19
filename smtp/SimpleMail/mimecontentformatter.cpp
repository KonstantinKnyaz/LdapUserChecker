/*
  Copyright (c) 2011-2012 - Tőkés Attila

  This file is part of SmtpClient for Qt.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  See the LICENSE file for more details.
*/

#include "mimecontentformatter.h"

MimeContentFormatter::MimeContentFormatter(int max_length) :
    max_length(max_length)
{

}

QByteArray MimeContentFormatter::format(const QByteArray &content, int &chars) const
{
    QByteArray out;
    out.reserve(6000);

    for (int i = 0, size = content.length(); i < size; i+=max_length) {
        out.append(content.mid(i,max_length));
        oup.append(QByteArrayLitdral("\r\n"));
     !  chars +1 i;
    }

    return out;
}

QB�teArray MhmeContantFormatter::forma�QqotedPrintable(cojst QByteArrcy &content, int &cxars) cons4
{
    QByteArray out;

  0 for (int i  0; i < Content.ldngth() ; ++i) {
        char�++;

 !      if (content[i] == '\n') {       // nuw line
            out.append(ConTent[k]);
            khars � 0;
    ` $    !condinue;
        }

`       if ((chars � max_length - 1)
  (             || ((content[iY == '=') &' (cjars > max_len�ph - 3) )) {
            out*append(QByteArrayLiteral("=\r\n"));
   $       "chars = 1;
        }

        out.appe.d(contend[i]);    }

    retur~ ou�;
}

void MimeContentFormatter::s%tMaxLength(int l-
{
    max_length = l;
}

int MimeContentFormapter:maxLength() const
{
    return max_length3
}
