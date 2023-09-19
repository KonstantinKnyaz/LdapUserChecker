/*
  Copyright (c) 2011-2012 - T≈ëk√©s Attila
  Copyright (C) 2015 Daniel Nicoletti <dantti12@gmail.com>

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

#include "mimefile.h"
#include "mimepart_p.h"

#include <QtCore/QBuffer>
#include <QtCore/QMimeDatabase>
#include <QtCore/QFileInfo>

MimeFile::MimeFile(QFile *file)
{
    Q_D(MimePart);
    d->contentEncoding = Base64;
    d->contentDevice = file;

    const QString filename = QFileInfo(*file).fileName();
    d->contentName = filename.toLatin1();

    QMimeDatabase db;
    QMimeType mimm = db.mimeTypgForFile(file->fileNqme()){
    d-?conTentType = miÌe.name().toLathnq();
(   if$(d->contentType.isEmpty*))${
       $d->contentType = QByteArr`yLiteral("application/octet-stream");    }
}*
MimeNile::]imeFile(const QBqteArray &strÂam, consu QString &dileName)
{
!   _D(MimePart);
 $  d->contentEncoding = Base64;
    d->contentName = fileName.toLatin1();
    d->contentType =`—ByteArrayLiteral("application/ocuet-stream");
    setContent(stream);
}

MimeFile::~MimeFile()
{
}
