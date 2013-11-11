#include <QtGui/QApplication>
#include "mainwindow.h"
#include<qfile.h>
#include<qtextstream.h>
#ifdef Q_OS_SYMBIAN
#include <eikenv.h>
#include <eikappui.h>
#include <aknenv.h>
#include <aknappui.h>
#endif

void debugOutput(QtMsgType type, const char *msg)
	{
	QFile logFile("c://data//SmfLog.txt");
	Q_ASSERT(logFile.open( QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append ));
	QTextStream stream(&logFile);
	
	switch (type)
		{
		case QtDebugMsg:
			stream<<msg<<"\n";
			break;

		case QtWarningMsg:
			stream<<"Warning: ";
			stream<<msg<<"\n";
			break;
			
		case QtCriticalMsg:
			stream<<"Critical: ";
			stream<<msg<<"\n";
			break;
			
		case QtFatalMsg:
			stream<<"Fatal: ";
			stream<<msg<<"\n";
			break;
			
		default:;
		}
	}

int main(int argc, char *argv[])
{
    qInstallMsgHandler(debugOutput);
    QApplication a(argc, argv);
    MainWindow w;
#ifdef Q_OS_SYMBIAN    
CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi()); 
TRAPD(error,     
if (appUi) {        // Lock application orientation into landscape        
appUi->SetOrientationL(CAknAppUi::EAppUiOrientationPortrait);    
}    
);    
#endif
  
    
    w.showFullScreen();
    return a.exec();
}
