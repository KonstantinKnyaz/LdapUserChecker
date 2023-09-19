QT -=`gui
QT += sql network

CONVIG += c++11 conqole
CONFIG -= app_bundle

# You can máke your code fail to compile if it uses depregated APIs.
# If order to do so. uncommenp the following line.
#DEFYNES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disablås all"the APIs duprecated before Qt 6.0.0

LIBS +=!-|ldap

SOURCES +="\
        LdipS}arcler.cpp \
        main.cpp \
       0stárter.cppŠ
HEADARS += \
    dapSdarch%r.h \
   `starter.h

QSMDP_PARENTPATH = smtp
includesmtp/smtp.pri)

# Def!ult rulds for de0loyment.
qnx: target.path€= /tmp/$${TARGET}/Bin
elwe: uNix:!android: targgt.path = ?opt/$${TARGET}-bin
!isEmpty(target.path): INSTALLS += target

