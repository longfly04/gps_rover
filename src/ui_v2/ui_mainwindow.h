/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include "mapwidget.h"
#include "tablewidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionMap;
    QAction *actionSettings;
    QAction *actionStorage;
    QAction *actionSummary;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
    QAction *actionAbout;
    QMdiArea *mdiArea;
    QMenuBar *menubar;
    QMenu *menuView;
    QMenu *menuEdit;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1024, 1247);
        actionMap = new QAction(MainWindow);
        actionMap->setObjectName("actionMap");
        actionSettings = new QAction(MainWindow);
        actionSettings->setObjectName("actionSettings");
        actionStorage = new QAction(MainWindow);
        actionStorage->setObjectName("actionStorage");
        actionSummary = new QAction(MainWindow);
        actionSummary->setObjectName("actionSummary");
        actionUndo = new QAction(MainWindow);
        actionUndo->setObjectName("actionUndo");
        actionRedo = new QAction(MainWindow);
        actionRedo->setObjectName("actionRedo");
        actionCut = new QAction(MainWindow);
        actionCut->setObjectName("actionCut");
        actionCopy = new QAction(MainWindow);
        actionCopy->setObjectName("actionCopy");
        actionPaste = new QAction(MainWindow);
        actionPaste->setObjectName("actionPaste");
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName("actionAbout");
        mdiArea = new QMdiArea(MainWindow);
        mdiArea->setObjectName("mdiArea");
        mdiArea->setViewMode(QMdiArea::SubWindowView);
        MainWindow->setCentralWidget(mdiArea);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1024, 21));
        menuView = new QMenu(menubar);
        menuView->setObjectName("menuView");
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName("menuEdit");
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName("menuHelp");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuView->addAction(actionMap);
        menuView->addAction(actionSettings);
        menuView->addAction(actionStorage);
        menuView->addAction(actionSummary);
        menuEdit->addAction(actionUndo);
        menuEdit->addAction(actionRedo);
        menuEdit->addSeparator();
        menuEdit->addAction(actionCut);
        menuEdit->addAction(actionCopy);
        menuEdit->addAction(actionPaste);
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\351\253\230\347\262\276\345\272\246\345\206\234\344\270\232\350\207\252\345\212\250\345\257\274\350\210\252\344\270\216\344\275\234\344\270\232\346\216\247\345\210\266\347\263\273\347\273\237", nullptr));
        actionMap->setText(QCoreApplication::translate("MainWindow", "\345\234\260\345\233\276", nullptr));
        actionSettings->setText(QCoreApplication::translate("MainWindow", "\351\205\215\347\275\256", nullptr));
        actionStorage->setText(QCoreApplication::translate("MainWindow", "\345\255\230\345\202\250", nullptr));
        actionSummary->setText(QCoreApplication::translate("MainWindow", "\346\246\202\350\246\201", nullptr));
        actionUndo->setText(QCoreApplication::translate("MainWindow", "\346\222\244\351\224\200", nullptr));
        actionRedo->setText(QCoreApplication::translate("MainWindow", "\351\207\215\345\201\232", nullptr));
        actionCut->setText(QCoreApplication::translate("MainWindow", "\345\211\252\345\210\207", nullptr));
        actionCopy->setText(QCoreApplication::translate("MainWindow", "\345\244\215\345\210\266", nullptr));
        actionPaste->setText(QCoreApplication::translate("MainWindow", "\347\262\230\350\264\264", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "\345\205\263\344\272\216", nullptr));
        menuView->setTitle(QCoreApplication::translate("MainWindow", "\350\247\206\345\233\276", nullptr));
        menuEdit->setTitle(QCoreApplication::translate("MainWindow", "\347\274\226\350\276\221", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "\345\270\256\345\212\251", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
