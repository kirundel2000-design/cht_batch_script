QT += core gui
QT += widgets printsupport

CONFIG += c++17 cmdline
CONFIG += console


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp

INCLUDEPATH += \
    "C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.22621.0\\um" \


LIBS += \
    -L"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x64\\" -luiautomationcore \
    -L"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x64\\" -luser32 \
    -L"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x64\\" -lole32 \
    -L"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x64\\" -loleaut32 \
    -L"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x64\\" -luuid \
    -L"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x64\\" -loleacc \
    -L"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x64\\" -ltdh

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res_files.qrc

HEADERS += \
    ChangeFiles.h \
    Definition.h \
    uiAutoFunc.h

#OTHER_FILES += cht_batch_script.manifest

#QMAKE_LFLAGS += /MANIFEST:EMBED
#QMAKE_LFLAGS += /MANIFESTINPUT:\"$$PWD/cht_batch_script.manifest\"
