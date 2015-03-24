#include "MainWindow.h"

#ifdef _MSC_VER
#    pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setApplicationName("Xenoviewer");
	MainWindow mainWin;
	qApp->installEventFilter(&mainWin);

#if defined(Q_OS_SYMBIAN)
	mainWin.showMaximized();
#else
	mainWin.show();
	mainWin.setWindowState(Qt::WindowActive);
	mainWin.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, mainWin.size(), app.desktop()->availableGeometry()));
#endif
	return app.exec();
}
