#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <earth3d.h>
#include <showtexturemapping.h>

void registerQMLTypes();

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    registerQMLTypes();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

void registerQMLTypes()
{
    qmlRegisterType<Earth3D>("OpenGLUnderQML", 1, 0, "Earth3D");
    qmlRegisterType<ShowTextureMapping>("OpenGLUnderQML", 1, 0, "ShowTextureMapping");
}
