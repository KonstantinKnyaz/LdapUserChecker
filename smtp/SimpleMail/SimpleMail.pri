HEADERS += \
    $$PWD/SimpleMail.h \
    $$PWD/mimeattachment.h \
    $$PWD/mimecontentformatter.h \
    $$PWD/mimefile.h \
    $$PWD/mimehtml.h \
    $$PWD/mimeinlinefile.h \
    $$PWD/mimemessage.h \
    $$PWD/mimemessage_p.h \
    $$PWD/mimemultipart.h \
    $$PWD/mimemultipart_p.h \
    $$PWD/mimepart.h \
    $$PWD/mimepart_p.h \
    $$PWD/mimetext.h \
    $$PWD/quotedprintable.h \
    $$PWD/sender.h \
    $$PWD/sender_p.h \
    $$PWD/server.h \
    $$PWD/server_p.h \
    $$PWD/serverreply.h \
    $$PWD/serverreply_p.h \
    $$PWD/emailaddress.h \
    $$PWD/emailaddress_p.h \


SOURCES += \
    $$PWD/emailaddress.cpp \
    $$PWD/mimeattachment.cpp \
    $$PWD/mimecontentformatter.cpp \
    $$PWD/mimefile.cpp \
    $$PWD/mimehtml.cpp \
    $$PWD/mimeinlinefile.cpp \
    $$PWD/mimemessage.cpp \
    $$PWD/mimemultipart.cpp \
    $$PWD/mimepart.cpp \
    $$PWD/mimetext.cpp \
    $$PWD/quotedprintable.cpp \
    $$PWD/sender.cpp \
    $$PWD/serverreply.cpp \
    $$PWD/server.cpp

include ( $$PWD/demos/SimpleMailDemos.pri )
