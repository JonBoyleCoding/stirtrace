#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imgproc.h"
#include <Qt/qfiledialog.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit_addnoise->setText("3,6,9,12,15");
    ui->lineEdit_median->setText("3,5,7,9");
    ui->lineEdit_rlines->setText("10,20,30,40,50,60,70,80,90,100");
    ui->lineEdit_rcolumns->setText("10,20,30,40,50,60,70,80,90,100");
    ui->lineEdit_rotate->setText("-20,-15,-10,-5.5,-5,7,7.5,13,18,20");
    ui->lineEdit_XStretch->setText("1.035,1.070,1.105,1.140,1.175,1.210,1.280,1.350");
    ui->lineEdit_YShear->setText("0.05,0.10,0.15,0.20,0.25,0.30");
    ui->lineEdit_scaling->setText("50,75,90,110,150,200");
    ui->lineEdit_cropping->setText("25,50,75");
    ui->lineEdit_banding->setText("0.005,0.010,0.025");
}

MainWindow::MainWindow(QStringList args, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if (args.length()>1) {
        QStringList validCMDLineSwitches;
        validCMDLineSwitches << "-n" << "-m" << "-l" << "-c" << "-r" << "-x" << "-y" << "-s" << "-o" << "-a" << "-b" << "-h" << "-d";
        for (int i=0; i< args.size(); i++) {
            //printf("[%d] %s\n",i, argv[i]);
            switch (validCMDLineSwitches.indexOf(args.at(i))) {
                case 0 : ui->lineEdit_addnoise->setText(args.at(++i)); break;
                case 1 : ui->lineEdit_median->setText(args.at(++i)); break;
                case 2 : ui->lineEdit_rlines->setText(args.at(++i)); break;
                case 3 : ui->lineEdit_rcolumns->setText(args.at(++i)); break;
                case 4 : ui->lineEdit_rotate->setText(args.at(++i)); break;
                case 5 : ui->lineEdit_XStretch->setText(args.at(++i)); break;
                case 6 : ui->lineEdit_YShear->setText(args.at(++i)); break;
                case 7 : ui->lineEdit_scaling->setText(args.at(++i)); break;
                case 8 : ui->lineEdit_cropping->setText(args.at(++i)); break;
                case 9 : ui->lineEdit_banding->setText(args.at(++i)); break;
                case 12 :ui->lineEdit->setText(args.at(++i));
                         ui->Button_StartProcessing->setEnabled(true);
                         break;
                default: break;
            }
        }
    } else {
        ui->lineEdit_addnoise->setText("3,6,9,12,15");
        ui->lineEdit_median->setText("3,5,7,9");
        ui->lineEdit_rlines->setText("10,20,30,40,50,60,70,80,90,100");
        ui->lineEdit_rcolumns->setText("10,20,30,40,50,60,70,80,90,100");
        ui->lineEdit_rotate->setText("-20,-15,-10,-5.5,-5,7,7.5,13,18,20");
        ui->lineEdit_XStretch->setText("1.035,1.070,1.105,1.140,1.175,1.210,1.280,1.350");
        ui->lineEdit_YShear->setText("0.05,0.10,0.15,0.20,0.25,0.30");
        ui->lineEdit_scaling->setText("50,75,90,110,150,200");
        ui->lineEdit_cropping->setText("25,50,75");
        ui->lineEdit_banding->setText("0.005,0.010,0.025");
    }
}

void MainWindow::slotStartProcessing() {
    //imgproc* processing=new imgproc(ui->lineEdit->text());
    imgproc* processing=new imgproc(ui->lineEdit->text(), ui->lineEdit_addnoise->text(), ui->lineEdit_median->text(), ui->lineEdit_rlines->text(), ui->lineEdit_rcolumns->text(), ui->lineEdit_rotate->text(), ui->lineEdit_XStretch->text(), ui->lineEdit_YShear->text(), ui->lineEdit_scaling->text(), ui->lineEdit_cropping->text(), ui->lineEdit_banding->text(),"","","", false,"","","", "",false, "", "", "");
    processing->startProcessing();
}

void MainWindow::showFolderBrowser() {
    QFileDialog folderbrowser;
    folderbrowser.setFileMode(QFileDialog::Directory);
    folderbrowser.setFilter(QDir::Dirs);
    folderbrowser.setNameFilter(tr("Image Files (*.png *.bmp)"));
    QString folder=folderbrowser.getExistingDirectory(this, tr("Select Image Folder"), "");
    ui->lineEdit->setText(folder);
    if (folder!="") ui->Button_StartProcessing->setEnabled(true);
        else ui->Button_StartProcessing->setEnabled(false);
}


MainWindow::~MainWindow()
{
    delete ui;
}
