TEMPLATE = app

QT += qml quick
CONFIG += c++11

DEFINES += TEST_ANDROID_LOCAL

SOURCES += main.cpp \
    earth3d.cpp \
    earth3drenderer.cpp \
    showtexturemapping.cpp \
    spheregenerator.cpp

RESOURCES += qml.qrc

HEADERS += \
    earth3d.h \
    earth3drenderer.h \
    showtexturemapping.h \
    spheregenerator.h

OTHER_FILES += style.astylerc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

DISTFILES += \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/AndroidManifest.xml \
    android/gradlew.bat \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

