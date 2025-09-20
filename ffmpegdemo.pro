QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT       += multimedia multimediawidgets

CONFIG += c++17

# 如果你希望代码在使用已弃用 API 时直接编译失败，取消下一行注释。
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # 禁用 Qt 6.0.0 之前标记为弃用的所有 API

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# 部署的默认规则。
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
