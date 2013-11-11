#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
    setStyleSheet("selection-color: Black;");

    Ptr2HomeView = new HomeView("SMF");
    //Ptr2HomeView->getFriends(this);
    Ptr2HomeView->SetupUI(this);
}

MainWindow::~MainWindow()
{
    //delete ui;
    delete Ptr2HomeView;

}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
