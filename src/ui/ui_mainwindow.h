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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "mapwidget.h"
#include "tablewidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionExit;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
    QAction *actionAbout;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab_navigation;
    QVBoxLayout *verticalLayout_2;
    MapWidget *mapWidget;
    QWidget *tab_settings;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *deviceStatusGroupBox;
    QHBoxLayout *deviceStatusLayout;
    QVBoxLayout *vboxLayout;
    QLabel *imuStatusLabel;
    QHBoxLayout *hboxLayout;
    QPushButton *imuConnectButton;
    QPushButton *imuDisconnectButton;
    QVBoxLayout *vboxLayout1;
    QLabel *gpsStatusLabel;
    QHBoxLayout *hboxLayout1;
    QPushButton *gpsConnectButton;
    QPushButton *gpsDisconnectButton;
    QVBoxLayout *vboxLayout2;
    QLabel *plcStatusLabel;
    QHBoxLayout *hboxLayout2;
    QPushButton *plcConnectButton;
    QPushButton *plcDisconnectButton;
    QHBoxLayout *sensorSettingsLayout;
    QGroupBox *imuSettingsGroupBox;
    QFormLayout *imuFormLayout;
    QLabel *imuPortLabel;
    QComboBox *imuPortComboBox;
    QLabel *imuBaudLabel;
    QComboBox *imuBaudComboBox;
    QLabel *imuConfigLabel;
    QGroupBox *gpsSettingsGroupBox;
    QFormLayout *gpsFormLayout;
    QLabel *gpsPortLabel;
    QComboBox *gpsPortComboBox;
    QLabel *gpsBaudLabel;
    QComboBox *gpsBaudComboBox;
    QLabel *gpsConfigLabel;
    QGroupBox *plcSettingsGroupBox;
    QFormLayout *plcFormLayout;
    QLabel *plcPortLabel;
    QLineEdit *plcIpLineEdit;
    QLabel *plcBaudLabel;
    QLineEdit *plcPortLineEdit;
    QLabel *plcConfigLabel;
    QGroupBox *plcGroupBox;
    QVBoxLayout *plcLayout;
    QHBoxLayout *plcRegisterBox1_2;
    QLabel *plcAddr1Label;
    QLineEdit *plcAddr1LineEdit;
    QLabel *plcCmdValue1Label;
    QLineEdit *plcCmdValue1LineEdit;
    QLabel *plcReadValue1TitleLabel;
    QLabel *plcReadValue1Label;
    QPushButton *plcSave1Button;
    QPushButton *plcSend1Button;
    QCheckBox *forwardTriggerLine1;
    QCheckBox *reverseTriggerLine1;
    QHBoxLayout *plcRegisterBox2_2;
    QLabel *plcAddr2Label;
    QLineEdit *plcAddr2LineEdit;
    QLabel *plcCmdValue2Label;
    QLineEdit *plcCmdValue2LineEdit;
    QLabel *plcReadValue2TitleLabel;
    QLabel *plcReadValue2Label;
    QPushButton *plcSave2Button;
    QPushButton *plcSend2Button;
    QCheckBox *forwardTriggerLine2;
    QCheckBox *reverseTriggerLine2;
    QHBoxLayout *plcRegisterBox3_2;
    QLabel *plcAddr3Label;
    QLineEdit *plcAddr3LineEdit;
    QLabel *plcCmdValue3Label;
    QLineEdit *plcCmdValue3LineEdit;
    QLabel *plcReadValue3TitleLabel;
    QLabel *plcReadValue3Label;
    QPushButton *plcSave3Button;
    QPushButton *plcSend3Button;
    QCheckBox *forwardTriggerLine3;
    QCheckBox *reverseTriggerLine3;
    QHBoxLayout *plcRegisterBox4_2;
    QLabel *plcAddr4Label;
    QLineEdit *plcAddr4LineEdit;
    QLabel *plcCmdValue4Label;
    QLineEdit *plcCmdValue4LineEdit;
    QLabel *plcReadValue4TitleLabel;
    QLabel *plcReadValue4Label;
    QPushButton *plcSave4Button;
    QPushButton *plcSend4Button;
    QCheckBox *forwardTriggerLine4;
    QCheckBox *reverseTriggerLine4;
    QHBoxLayout *plcRegisterBox5_2;
    QLabel *plcAddr5Label;
    QLineEdit *plcAddr5LineEdit;
    QLabel *plcCmdValue5Label;
    QLineEdit *plcCmdValue5LineEdit;
    QLabel *plcReadValue5TitleLabel;
    QLabel *plcReadValue5Label;
    QPushButton *plcSave5Button;
    QPushButton *plcSend5Button;
    QCheckBox *forwardTriggerLine5;
    QCheckBox *reverseTriggerLine5;
    QGroupBox *coordinateSystemGroupBox;
    QVBoxLayout *coordinateSystemLayout;
    TableWidget *coordinateSystemWidget;
    QHBoxLayout *parametersLayout;
    QGroupBox *groupBox;
    QGridLayout *formLayout;
    QLabel *areaLengthLabel;
    QLabel *areaLengthLabelValue;
    QLabel *areaWidthLabel;
    QLabel *areaWidthLabelValue;
    QLabel *label;
    QLabel *rowSpacingLabelValue;
    QLabel *fieldLengthLabel;
    QLabel *fieldLengthLabelValue;
    QLabel *label_2;
    QLabel *headlandWidthLabelValue;
    QLabel *label_3;
    QLabel *rowsPerBlockLabelValue;
    QLabel *seedsPerBlockLabel;
    QLabel *seedsPerBlockLabelValue;
    QLabel *triggerAheadLabel;
    QLabel *triggerAheadLabelValue;
    QGroupBox *pathPlanningGroupBox;
    QGridLayout *pathPlanningFormLayout;
    QLabel *oPointLabel;
    QLineEdit *oPointLineEdit;
    QLabel *aPointLabel;
    QLineEdit *aPointLineEdit;
    QHBoxLayout *configFileLayout;
    QPushButton *generateConfigButton;
    QCheckBox *trajectoryVisibleCheckBox;
    QPushButton *clearConfigButton;
    QPushButton *loadConfigButton;
    QPushButton *saveConfigButton;
    QWidget *tab_data;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *databaseGroupBox;
    QVBoxLayout *databaseLayout;
    QLabel *databaseStatusLabel;
    QHBoxLayout *hboxLayout3;
    QPushButton *connectDatabaseButton;
    QPushButton *generateSeedingPlanButton;
    QPushButton *antennaCalibrationButton;
    QPushButton *lockSeedingPlanButton;
    QSpacerItem *horizontalSpacer;
    QLabel *configFileLabel;
    QPushButton *saveConfigFileButton;
    QPushButton *loadConfigFileButton;
    QGroupBox *jobInfoGroupBox;
    QHBoxLayout *jobInfoLayout;
    QTableWidget *cellConfigTableWidget;
    QGroupBox *seederParamsGroupBox;
    QGridLayout *seederParamsLayout;
    QLabel *rowSpacingLabel;
    QLineEdit *rowSpacingLineEdit;
    QLabel *seedHoleDistanceLabel;
    QLineEdit *seedHoleDistanceLineEdit;
    QLabel *seedSpeedRatioLabel;
    QLineEdit *seedSpeedRatioLineEdit;
    QLabel *seedRadiusLabel;
    QLineEdit *seedRadiusLineEdit;
    QLabel *seedAdsorbCountLabel;
    QLineEdit *seedAdsorbCountLineEdit;
    QLabel *seedDropHeightLabel;
    QLineEdit *seedDropHeightLineEdit;
    QLabel *antennaArmLabel;
    QLineEdit *antennaArmLineEdit;
    QLabel *distancePulseRatioLabel;
    QLineEdit *distancePulseRatioLineEdit;
    QLabel *signalTransmissionDelayLabel;
    QLineEdit *signalTransmissionDelayLineEdit;
    QCheckBox *reverseOrderCheckBox;
    QLabel *cellAreaLabel;
    QLabel *x1ValueLabel;
    QLabel *x2ValueLabel;
    QGroupBox *operationControlGroupBox;
    QVBoxLayout *operationControlLayout;
    QTableWidget *seedingPlanTableWidget;
    QGroupBox *timeAndSensorGroupBox;
    QVBoxLayout *timeAndSensorLayout;
    QHBoxLayout *timeAndSensorTopLayout;
    QLabel *timeLabel;
    QLabel *localTimeLabel;
    QSpacerItem *horizontalSpacer_2;
    QLabel *gpsTimeLabel;
    QLabel *gpsTimeValueLabel;
    QSpacerItem *horizontalSpacer_8;
    QGridLayout *sensorDataGridLayout;
    QLabel *imuAccelerationLabel;
    QLabel *gpsPositionLabel;
    QLabel *imuDthetaLabel;
    QLabel *speedLabel;
    QLabel *imuOutputMagneticHeadingLabel;
    QLabel *magneticHeadingLabel;
    QLabel *imuAngleDisplayLabel;
    QLabel *nextTriggerLabel;
    QLabel *imuDataCountLabel;
    QLabel *gpsDataCountLabel;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1024, 1247);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName("actionOpen");
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName("actionSave");
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName("actionExit");
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
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tab_navigation = new QWidget();
        tab_navigation->setObjectName("tab_navigation");
        verticalLayout_2 = new QVBoxLayout(tab_navigation);
        verticalLayout_2->setObjectName("verticalLayout_2");
        mapWidget = new MapWidget(tab_navigation);
        mapWidget->setObjectName("mapWidget");
        mapWidget->setMinimumSize(QSize(800, 600));

        verticalLayout_2->addWidget(mapWidget);

        tabWidget->addTab(tab_navigation, QString());
        tab_settings = new QWidget();
        tab_settings->setObjectName("tab_settings");
        verticalLayout_3 = new QVBoxLayout(tab_settings);
        verticalLayout_3->setObjectName("verticalLayout_3");
        deviceStatusGroupBox = new QGroupBox(tab_settings);
        deviceStatusGroupBox->setObjectName("deviceStatusGroupBox");
        deviceStatusLayout = new QHBoxLayout(deviceStatusGroupBox);
        deviceStatusLayout->setObjectName("deviceStatusLayout");
        vboxLayout = new QVBoxLayout();
        vboxLayout->setObjectName("vboxLayout");
        imuStatusLabel = new QLabel(deviceStatusGroupBox);
        imuStatusLabel->setObjectName("imuStatusLabel");

        vboxLayout->addWidget(imuStatusLabel);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName("hboxLayout");
        imuConnectButton = new QPushButton(deviceStatusGroupBox);
        imuConnectButton->setObjectName("imuConnectButton");

        hboxLayout->addWidget(imuConnectButton);

        imuDisconnectButton = new QPushButton(deviceStatusGroupBox);
        imuDisconnectButton->setObjectName("imuDisconnectButton");

        hboxLayout->addWidget(imuDisconnectButton);


        vboxLayout->addLayout(hboxLayout);


        deviceStatusLayout->addLayout(vboxLayout);

        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setObjectName("vboxLayout1");
        gpsStatusLabel = new QLabel(deviceStatusGroupBox);
        gpsStatusLabel->setObjectName("gpsStatusLabel");

        vboxLayout1->addWidget(gpsStatusLabel);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setObjectName("hboxLayout1");
        gpsConnectButton = new QPushButton(deviceStatusGroupBox);
        gpsConnectButton->setObjectName("gpsConnectButton");

        hboxLayout1->addWidget(gpsConnectButton);

        gpsDisconnectButton = new QPushButton(deviceStatusGroupBox);
        gpsDisconnectButton->setObjectName("gpsDisconnectButton");

        hboxLayout1->addWidget(gpsDisconnectButton);


        vboxLayout1->addLayout(hboxLayout1);


        deviceStatusLayout->addLayout(vboxLayout1);

        vboxLayout2 = new QVBoxLayout();
        vboxLayout2->setObjectName("vboxLayout2");
        plcStatusLabel = new QLabel(deviceStatusGroupBox);
        plcStatusLabel->setObjectName("plcStatusLabel");

        vboxLayout2->addWidget(plcStatusLabel);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setObjectName("hboxLayout2");
        plcConnectButton = new QPushButton(deviceStatusGroupBox);
        plcConnectButton->setObjectName("plcConnectButton");

        hboxLayout2->addWidget(plcConnectButton);

        plcDisconnectButton = new QPushButton(deviceStatusGroupBox);
        plcDisconnectButton->setObjectName("plcDisconnectButton");

        hboxLayout2->addWidget(plcDisconnectButton);


        vboxLayout2->addLayout(hboxLayout2);


        deviceStatusLayout->addLayout(vboxLayout2);


        verticalLayout_3->addWidget(deviceStatusGroupBox);

        sensorSettingsLayout = new QHBoxLayout();
        sensorSettingsLayout->setObjectName("sensorSettingsLayout");
        imuSettingsGroupBox = new QGroupBox(tab_settings);
        imuSettingsGroupBox->setObjectName("imuSettingsGroupBox");
        imuFormLayout = new QFormLayout(imuSettingsGroupBox);
        imuFormLayout->setObjectName("imuFormLayout");
        imuPortLabel = new QLabel(imuSettingsGroupBox);
        imuPortLabel->setObjectName("imuPortLabel");

        imuFormLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, imuPortLabel);

        imuPortComboBox = new QComboBox(imuSettingsGroupBox);
        imuPortComboBox->addItem(QString());
        imuPortComboBox->addItem(QString());
        imuPortComboBox->addItem(QString());
        imuPortComboBox->addItem(QString());
        imuPortComboBox->setObjectName("imuPortComboBox");

        imuFormLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, imuPortComboBox);

        imuBaudLabel = new QLabel(imuSettingsGroupBox);
        imuBaudLabel->setObjectName("imuBaudLabel");

        imuFormLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, imuBaudLabel);

        imuBaudComboBox = new QComboBox(imuSettingsGroupBox);
        imuBaudComboBox->addItem(QString());
        imuBaudComboBox->addItem(QString());
        imuBaudComboBox->addItem(QString());
        imuBaudComboBox->addItem(QString());
        imuBaudComboBox->addItem(QString());
        imuBaudComboBox->setObjectName("imuBaudComboBox");

        imuFormLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, imuBaudComboBox);

        imuConfigLabel = new QLabel(imuSettingsGroupBox);
        imuConfigLabel->setObjectName("imuConfigLabel");

        imuFormLayout->setWidget(2, QFormLayout::ItemRole::SpanningRole, imuConfigLabel);


        sensorSettingsLayout->addWidget(imuSettingsGroupBox);

        gpsSettingsGroupBox = new QGroupBox(tab_settings);
        gpsSettingsGroupBox->setObjectName("gpsSettingsGroupBox");
        gpsFormLayout = new QFormLayout(gpsSettingsGroupBox);
        gpsFormLayout->setObjectName("gpsFormLayout");
        gpsPortLabel = new QLabel(gpsSettingsGroupBox);
        gpsPortLabel->setObjectName("gpsPortLabel");

        gpsFormLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, gpsPortLabel);

        gpsPortComboBox = new QComboBox(gpsSettingsGroupBox);
        gpsPortComboBox->addItem(QString());
        gpsPortComboBox->addItem(QString());
        gpsPortComboBox->addItem(QString());
        gpsPortComboBox->addItem(QString());
        gpsPortComboBox->setObjectName("gpsPortComboBox");

        gpsFormLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, gpsPortComboBox);

        gpsBaudLabel = new QLabel(gpsSettingsGroupBox);
        gpsBaudLabel->setObjectName("gpsBaudLabel");

        gpsFormLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, gpsBaudLabel);

        gpsBaudComboBox = new QComboBox(gpsSettingsGroupBox);
        gpsBaudComboBox->addItem(QString());
        gpsBaudComboBox->addItem(QString());
        gpsBaudComboBox->addItem(QString());
        gpsBaudComboBox->addItem(QString());
        gpsBaudComboBox->addItem(QString());
        gpsBaudComboBox->setObjectName("gpsBaudComboBox");

        gpsFormLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, gpsBaudComboBox);

        gpsConfigLabel = new QLabel(gpsSettingsGroupBox);
        gpsConfigLabel->setObjectName("gpsConfigLabel");

        gpsFormLayout->setWidget(2, QFormLayout::ItemRole::SpanningRole, gpsConfigLabel);


        sensorSettingsLayout->addWidget(gpsSettingsGroupBox);

        plcSettingsGroupBox = new QGroupBox(tab_settings);
        plcSettingsGroupBox->setObjectName("plcSettingsGroupBox");
        plcFormLayout = new QFormLayout(plcSettingsGroupBox);
        plcFormLayout->setObjectName("plcFormLayout");
        plcPortLabel = new QLabel(plcSettingsGroupBox);
        plcPortLabel->setObjectName("plcPortLabel");

        plcFormLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, plcPortLabel);

        plcIpLineEdit = new QLineEdit(plcSettingsGroupBox);
        plcIpLineEdit->setObjectName("plcIpLineEdit");

        plcFormLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, plcIpLineEdit);

        plcBaudLabel = new QLabel(plcSettingsGroupBox);
        plcBaudLabel->setObjectName("plcBaudLabel");

        plcFormLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, plcBaudLabel);

        plcPortLineEdit = new QLineEdit(plcSettingsGroupBox);
        plcPortLineEdit->setObjectName("plcPortLineEdit");

        plcFormLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, plcPortLineEdit);

        plcConfigLabel = new QLabel(plcSettingsGroupBox);
        plcConfigLabel->setObjectName("plcConfigLabel");

        plcFormLayout->setWidget(2, QFormLayout::ItemRole::SpanningRole, plcConfigLabel);


        sensorSettingsLayout->addWidget(plcSettingsGroupBox);


        verticalLayout_3->addLayout(sensorSettingsLayout);

        plcGroupBox = new QGroupBox(tab_settings);
        plcGroupBox->setObjectName("plcGroupBox");
        plcLayout = new QVBoxLayout(plcGroupBox);
        plcLayout->setObjectName("plcLayout");
        plcRegisterBox1_2 = new QHBoxLayout();
        plcRegisterBox1_2->setObjectName("plcRegisterBox1_2");
        plcAddr1Label = new QLabel(plcGroupBox);
        plcAddr1Label->setObjectName("plcAddr1Label");

        plcRegisterBox1_2->addWidget(plcAddr1Label);

        plcAddr1LineEdit = new QLineEdit(plcGroupBox);
        plcAddr1LineEdit->setObjectName("plcAddr1LineEdit");

        plcRegisterBox1_2->addWidget(plcAddr1LineEdit);

        plcCmdValue1Label = new QLabel(plcGroupBox);
        plcCmdValue1Label->setObjectName("plcCmdValue1Label");

        plcRegisterBox1_2->addWidget(plcCmdValue1Label);

        plcCmdValue1LineEdit = new QLineEdit(plcGroupBox);
        plcCmdValue1LineEdit->setObjectName("plcCmdValue1LineEdit");

        plcRegisterBox1_2->addWidget(plcCmdValue1LineEdit);

        plcReadValue1TitleLabel = new QLabel(plcGroupBox);
        plcReadValue1TitleLabel->setObjectName("plcReadValue1TitleLabel");

        plcRegisterBox1_2->addWidget(plcReadValue1TitleLabel);

        plcReadValue1Label = new QLabel(plcGroupBox);
        plcReadValue1Label->setObjectName("plcReadValue1Label");

        plcRegisterBox1_2->addWidget(plcReadValue1Label);

        plcSave1Button = new QPushButton(plcGroupBox);
        plcSave1Button->setObjectName("plcSave1Button");

        plcRegisterBox1_2->addWidget(plcSave1Button);

        plcSend1Button = new QPushButton(plcGroupBox);
        plcSend1Button->setObjectName("plcSend1Button");

        plcRegisterBox1_2->addWidget(plcSend1Button);

        forwardTriggerLine1 = new QCheckBox(plcGroupBox);
        forwardTriggerLine1->setObjectName("forwardTriggerLine1");

        plcRegisterBox1_2->addWidget(forwardTriggerLine1);

        reverseTriggerLine1 = new QCheckBox(plcGroupBox);
        reverseTriggerLine1->setObjectName("reverseTriggerLine1");

        plcRegisterBox1_2->addWidget(reverseTriggerLine1);


        plcLayout->addLayout(plcRegisterBox1_2);

        plcRegisterBox2_2 = new QHBoxLayout();
        plcRegisterBox2_2->setObjectName("plcRegisterBox2_2");
        plcAddr2Label = new QLabel(plcGroupBox);
        plcAddr2Label->setObjectName("plcAddr2Label");

        plcRegisterBox2_2->addWidget(plcAddr2Label);

        plcAddr2LineEdit = new QLineEdit(plcGroupBox);
        plcAddr2LineEdit->setObjectName("plcAddr2LineEdit");

        plcRegisterBox2_2->addWidget(plcAddr2LineEdit);

        plcCmdValue2Label = new QLabel(plcGroupBox);
        plcCmdValue2Label->setObjectName("plcCmdValue2Label");

        plcRegisterBox2_2->addWidget(plcCmdValue2Label);

        plcCmdValue2LineEdit = new QLineEdit(plcGroupBox);
        plcCmdValue2LineEdit->setObjectName("plcCmdValue2LineEdit");

        plcRegisterBox2_2->addWidget(plcCmdValue2LineEdit);

        plcReadValue2TitleLabel = new QLabel(plcGroupBox);
        plcReadValue2TitleLabel->setObjectName("plcReadValue2TitleLabel");

        plcRegisterBox2_2->addWidget(plcReadValue2TitleLabel);

        plcReadValue2Label = new QLabel(plcGroupBox);
        plcReadValue2Label->setObjectName("plcReadValue2Label");

        plcRegisterBox2_2->addWidget(plcReadValue2Label);

        plcSave2Button = new QPushButton(plcGroupBox);
        plcSave2Button->setObjectName("plcSave2Button");

        plcRegisterBox2_2->addWidget(plcSave2Button);

        plcSend2Button = new QPushButton(plcGroupBox);
        plcSend2Button->setObjectName("plcSend2Button");

        plcRegisterBox2_2->addWidget(plcSend2Button);

        forwardTriggerLine2 = new QCheckBox(plcGroupBox);
        forwardTriggerLine2->setObjectName("forwardTriggerLine2");

        plcRegisterBox2_2->addWidget(forwardTriggerLine2);

        reverseTriggerLine2 = new QCheckBox(plcGroupBox);
        reverseTriggerLine2->setObjectName("reverseTriggerLine2");

        plcRegisterBox2_2->addWidget(reverseTriggerLine2);


        plcLayout->addLayout(plcRegisterBox2_2);

        plcRegisterBox3_2 = new QHBoxLayout();
        plcRegisterBox3_2->setObjectName("plcRegisterBox3_2");
        plcAddr3Label = new QLabel(plcGroupBox);
        plcAddr3Label->setObjectName("plcAddr3Label");

        plcRegisterBox3_2->addWidget(plcAddr3Label);

        plcAddr3LineEdit = new QLineEdit(plcGroupBox);
        plcAddr3LineEdit->setObjectName("plcAddr3LineEdit");

        plcRegisterBox3_2->addWidget(plcAddr3LineEdit);

        plcCmdValue3Label = new QLabel(plcGroupBox);
        plcCmdValue3Label->setObjectName("plcCmdValue3Label");

        plcRegisterBox3_2->addWidget(plcCmdValue3Label);

        plcCmdValue3LineEdit = new QLineEdit(plcGroupBox);
        plcCmdValue3LineEdit->setObjectName("plcCmdValue3LineEdit");

        plcRegisterBox3_2->addWidget(plcCmdValue3LineEdit);

        plcReadValue3TitleLabel = new QLabel(plcGroupBox);
        plcReadValue3TitleLabel->setObjectName("plcReadValue3TitleLabel");

        plcRegisterBox3_2->addWidget(plcReadValue3TitleLabel);

        plcReadValue3Label = new QLabel(plcGroupBox);
        plcReadValue3Label->setObjectName("plcReadValue3Label");

        plcRegisterBox3_2->addWidget(plcReadValue3Label);

        plcSave3Button = new QPushButton(plcGroupBox);
        plcSave3Button->setObjectName("plcSave3Button");

        plcRegisterBox3_2->addWidget(plcSave3Button);

        plcSend3Button = new QPushButton(plcGroupBox);
        plcSend3Button->setObjectName("plcSend3Button");

        plcRegisterBox3_2->addWidget(plcSend3Button);

        forwardTriggerLine3 = new QCheckBox(plcGroupBox);
        forwardTriggerLine3->setObjectName("forwardTriggerLine3");

        plcRegisterBox3_2->addWidget(forwardTriggerLine3);

        reverseTriggerLine3 = new QCheckBox(plcGroupBox);
        reverseTriggerLine3->setObjectName("reverseTriggerLine3");

        plcRegisterBox3_2->addWidget(reverseTriggerLine3);


        plcLayout->addLayout(plcRegisterBox3_2);

        plcRegisterBox4_2 = new QHBoxLayout();
        plcRegisterBox4_2->setObjectName("plcRegisterBox4_2");
        plcAddr4Label = new QLabel(plcGroupBox);
        plcAddr4Label->setObjectName("plcAddr4Label");

        plcRegisterBox4_2->addWidget(plcAddr4Label);

        plcAddr4LineEdit = new QLineEdit(plcGroupBox);
        plcAddr4LineEdit->setObjectName("plcAddr4LineEdit");

        plcRegisterBox4_2->addWidget(plcAddr4LineEdit);

        plcCmdValue4Label = new QLabel(plcGroupBox);
        plcCmdValue4Label->setObjectName("plcCmdValue4Label");

        plcRegisterBox4_2->addWidget(plcCmdValue4Label);

        plcCmdValue4LineEdit = new QLineEdit(plcGroupBox);
        plcCmdValue4LineEdit->setObjectName("plcCmdValue4LineEdit");

        plcRegisterBox4_2->addWidget(plcCmdValue4LineEdit);

        plcReadValue4TitleLabel = new QLabel(plcGroupBox);
        plcReadValue4TitleLabel->setObjectName("plcReadValue4TitleLabel");

        plcRegisterBox4_2->addWidget(plcReadValue4TitleLabel);

        plcReadValue4Label = new QLabel(plcGroupBox);
        plcReadValue4Label->setObjectName("plcReadValue4Label");

        plcRegisterBox4_2->addWidget(plcReadValue4Label);

        plcSave4Button = new QPushButton(plcGroupBox);
        plcSave4Button->setObjectName("plcSave4Button");

        plcRegisterBox4_2->addWidget(plcSave4Button);

        plcSend4Button = new QPushButton(plcGroupBox);
        plcSend4Button->setObjectName("plcSend4Button");

        plcRegisterBox4_2->addWidget(plcSend4Button);

        forwardTriggerLine4 = new QCheckBox(plcGroupBox);
        forwardTriggerLine4->setObjectName("forwardTriggerLine4");

        plcRegisterBox4_2->addWidget(forwardTriggerLine4);

        reverseTriggerLine4 = new QCheckBox(plcGroupBox);
        reverseTriggerLine4->setObjectName("reverseTriggerLine4");

        plcRegisterBox4_2->addWidget(reverseTriggerLine4);


        plcLayout->addLayout(plcRegisterBox4_2);

        plcRegisterBox5_2 = new QHBoxLayout();
        plcRegisterBox5_2->setObjectName("plcRegisterBox5_2");
        plcAddr5Label = new QLabel(plcGroupBox);
        plcAddr5Label->setObjectName("plcAddr5Label");

        plcRegisterBox5_2->addWidget(plcAddr5Label);

        plcAddr5LineEdit = new QLineEdit(plcGroupBox);
        plcAddr5LineEdit->setObjectName("plcAddr5LineEdit");

        plcRegisterBox5_2->addWidget(plcAddr5LineEdit);

        plcCmdValue5Label = new QLabel(plcGroupBox);
        plcCmdValue5Label->setObjectName("plcCmdValue5Label");

        plcRegisterBox5_2->addWidget(plcCmdValue5Label);

        plcCmdValue5LineEdit = new QLineEdit(plcGroupBox);
        plcCmdValue5LineEdit->setObjectName("plcCmdValue5LineEdit");

        plcRegisterBox5_2->addWidget(plcCmdValue5LineEdit);

        plcReadValue5TitleLabel = new QLabel(plcGroupBox);
        plcReadValue5TitleLabel->setObjectName("plcReadValue5TitleLabel");

        plcRegisterBox5_2->addWidget(plcReadValue5TitleLabel);

        plcReadValue5Label = new QLabel(plcGroupBox);
        plcReadValue5Label->setObjectName("plcReadValue5Label");

        plcRegisterBox5_2->addWidget(plcReadValue5Label);

        plcSave5Button = new QPushButton(plcGroupBox);
        plcSave5Button->setObjectName("plcSave5Button");

        plcRegisterBox5_2->addWidget(plcSave5Button);

        plcSend5Button = new QPushButton(plcGroupBox);
        plcSend5Button->setObjectName("plcSend5Button");

        plcRegisterBox5_2->addWidget(plcSend5Button);

        forwardTriggerLine5 = new QCheckBox(plcGroupBox);
        forwardTriggerLine5->setObjectName("forwardTriggerLine5");

        plcRegisterBox5_2->addWidget(forwardTriggerLine5);

        reverseTriggerLine5 = new QCheckBox(plcGroupBox);
        reverseTriggerLine5->setObjectName("reverseTriggerLine5");

        plcRegisterBox5_2->addWidget(reverseTriggerLine5);


        plcLayout->addLayout(plcRegisterBox5_2);


        verticalLayout_3->addWidget(plcGroupBox);

        coordinateSystemGroupBox = new QGroupBox(tab_settings);
        coordinateSystemGroupBox->setObjectName("coordinateSystemGroupBox");
        coordinateSystemLayout = new QVBoxLayout(coordinateSystemGroupBox);
        coordinateSystemLayout->setObjectName("coordinateSystemLayout");
        coordinateSystemWidget = new TableWidget(coordinateSystemGroupBox);
        coordinateSystemWidget->setObjectName("coordinateSystemWidget");
        coordinateSystemWidget->setMinimumSize(QSize(0, 300));

        coordinateSystemLayout->addWidget(coordinateSystemWidget);


        verticalLayout_3->addWidget(coordinateSystemGroupBox);

        parametersLayout = new QHBoxLayout();
        parametersLayout->setObjectName("parametersLayout");
        groupBox = new QGroupBox(tab_settings);
        groupBox->setObjectName("groupBox");
        formLayout = new QGridLayout(groupBox);
        formLayout->setObjectName("formLayout");
        areaLengthLabel = new QLabel(groupBox);
        areaLengthLabel->setObjectName("areaLengthLabel");

        formLayout->addWidget(areaLengthLabel, 0, 0, 1, 1);

        areaLengthLabelValue = new QLabel(groupBox);
        areaLengthLabelValue->setObjectName("areaLengthLabelValue");

        formLayout->addWidget(areaLengthLabelValue, 0, 1, 1, 1);

        areaWidthLabel = new QLabel(groupBox);
        areaWidthLabel->setObjectName("areaWidthLabel");

        formLayout->addWidget(areaWidthLabel, 0, 2, 1, 1);

        areaWidthLabelValue = new QLabel(groupBox);
        areaWidthLabelValue->setObjectName("areaWidthLabelValue");

        formLayout->addWidget(areaWidthLabelValue, 0, 3, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName("label");

        formLayout->addWidget(label, 1, 0, 1, 1);

        rowSpacingLabelValue = new QLabel(groupBox);
        rowSpacingLabelValue->setObjectName("rowSpacingLabelValue");

        formLayout->addWidget(rowSpacingLabelValue, 1, 1, 1, 1);

        fieldLengthLabel = new QLabel(groupBox);
        fieldLengthLabel->setObjectName("fieldLengthLabel");

        formLayout->addWidget(fieldLengthLabel, 1, 2, 1, 1);

        fieldLengthLabelValue = new QLabel(groupBox);
        fieldLengthLabelValue->setObjectName("fieldLengthLabelValue");

        formLayout->addWidget(fieldLengthLabelValue, 1, 3, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");

        formLayout->addWidget(label_2, 2, 0, 1, 1);

        headlandWidthLabelValue = new QLabel(groupBox);
        headlandWidthLabelValue->setObjectName("headlandWidthLabelValue");

        formLayout->addWidget(headlandWidthLabelValue, 2, 1, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");

        formLayout->addWidget(label_3, 2, 2, 1, 1);

        rowsPerBlockLabelValue = new QLabel(groupBox);
        rowsPerBlockLabelValue->setObjectName("rowsPerBlockLabelValue");

        formLayout->addWidget(rowsPerBlockLabelValue, 2, 3, 1, 1);

        seedsPerBlockLabel = new QLabel(groupBox);
        seedsPerBlockLabel->setObjectName("seedsPerBlockLabel");

        formLayout->addWidget(seedsPerBlockLabel, 3, 0, 1, 1);

        seedsPerBlockLabelValue = new QLabel(groupBox);
        seedsPerBlockLabelValue->setObjectName("seedsPerBlockLabelValue");

        formLayout->addWidget(seedsPerBlockLabelValue, 3, 1, 1, 1);

        triggerAheadLabel = new QLabel(groupBox);
        triggerAheadLabel->setObjectName("triggerAheadLabel");
        triggerAheadLabel->setVisible(false);

        formLayout->addWidget(triggerAheadLabel, 3, 2, 1, 1);

        triggerAheadLabelValue = new QLabel(groupBox);
        triggerAheadLabelValue->setObjectName("triggerAheadLabelValue");
        triggerAheadLabelValue->setVisible(false);

        formLayout->addWidget(triggerAheadLabelValue, 3, 3, 1, 1);


        parametersLayout->addWidget(groupBox);

        pathPlanningGroupBox = new QGroupBox(tab_settings);
        pathPlanningGroupBox->setObjectName("pathPlanningGroupBox");
        pathPlanningFormLayout = new QGridLayout(pathPlanningGroupBox);
        pathPlanningFormLayout->setObjectName("pathPlanningFormLayout");
        oPointLabel = new QLabel(pathPlanningGroupBox);
        oPointLabel->setObjectName("oPointLabel");

        pathPlanningFormLayout->addWidget(oPointLabel, 0, 0, 1, 1);

        oPointLineEdit = new QLineEdit(pathPlanningGroupBox);
        oPointLineEdit->setObjectName("oPointLineEdit");

        pathPlanningFormLayout->addWidget(oPointLineEdit, 0, 1, 1, 1);

        aPointLabel = new QLabel(pathPlanningGroupBox);
        aPointLabel->setObjectName("aPointLabel");

        pathPlanningFormLayout->addWidget(aPointLabel, 1, 0, 1, 1);

        aPointLineEdit = new QLineEdit(pathPlanningGroupBox);
        aPointLineEdit->setObjectName("aPointLineEdit");

        pathPlanningFormLayout->addWidget(aPointLineEdit, 1, 1, 1, 1);


        parametersLayout->addWidget(pathPlanningGroupBox);


        verticalLayout_3->addLayout(parametersLayout);

        configFileLayout = new QHBoxLayout();
        configFileLayout->setObjectName("configFileLayout");
        generateConfigButton = new QPushButton(tab_settings);
        generateConfigButton->setObjectName("generateConfigButton");

        configFileLayout->addWidget(generateConfigButton);

        trajectoryVisibleCheckBox = new QCheckBox(tab_settings);
        trajectoryVisibleCheckBox->setObjectName("trajectoryVisibleCheckBox");
        trajectoryVisibleCheckBox->setChecked(false);

        configFileLayout->addWidget(trajectoryVisibleCheckBox);

        clearConfigButton = new QPushButton(tab_settings);
        clearConfigButton->setObjectName("clearConfigButton");

        configFileLayout->addWidget(clearConfigButton);

        loadConfigButton = new QPushButton(tab_settings);
        loadConfigButton->setObjectName("loadConfigButton");

        configFileLayout->addWidget(loadConfigButton);

        saveConfigButton = new QPushButton(tab_settings);
        saveConfigButton->setObjectName("saveConfigButton");

        configFileLayout->addWidget(saveConfigButton);


        verticalLayout_3->addLayout(configFileLayout);

        tabWidget->addTab(tab_settings, QString());
        tab_data = new QWidget();
        tab_data->setObjectName("tab_data");
        verticalLayout_4 = new QVBoxLayout(tab_data);
        verticalLayout_4->setObjectName("verticalLayout_4");
        databaseGroupBox = new QGroupBox(tab_data);
        databaseGroupBox->setObjectName("databaseGroupBox");
        databaseLayout = new QVBoxLayout(databaseGroupBox);
        databaseLayout->setObjectName("databaseLayout");
        databaseStatusLabel = new QLabel(databaseGroupBox);
        databaseStatusLabel->setObjectName("databaseStatusLabel");

        databaseLayout->addWidget(databaseStatusLabel);

        hboxLayout3 = new QHBoxLayout();
        hboxLayout3->setObjectName("hboxLayout3");
        connectDatabaseButton = new QPushButton(databaseGroupBox);
        connectDatabaseButton->setObjectName("connectDatabaseButton");

        hboxLayout3->addWidget(connectDatabaseButton);

        generateSeedingPlanButton = new QPushButton(databaseGroupBox);
        generateSeedingPlanButton->setObjectName("generateSeedingPlanButton");

        hboxLayout3->addWidget(generateSeedingPlanButton);

        antennaCalibrationButton = new QPushButton(databaseGroupBox);
        antennaCalibrationButton->setObjectName("antennaCalibrationButton");

        hboxLayout3->addWidget(antennaCalibrationButton);

        lockSeedingPlanButton = new QPushButton(databaseGroupBox);
        lockSeedingPlanButton->setObjectName("lockSeedingPlanButton");

        hboxLayout3->addWidget(lockSeedingPlanButton);

        horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hboxLayout3->addItem(horizontalSpacer);

        configFileLabel = new QLabel(databaseGroupBox);
        configFileLabel->setObjectName("configFileLabel");

        hboxLayout3->addWidget(configFileLabel);

        saveConfigFileButton = new QPushButton(databaseGroupBox);
        saveConfigFileButton->setObjectName("saveConfigFileButton");

        hboxLayout3->addWidget(saveConfigFileButton);

        loadConfigFileButton = new QPushButton(databaseGroupBox);
        loadConfigFileButton->setObjectName("loadConfigFileButton");

        hboxLayout3->addWidget(loadConfigFileButton);


        databaseLayout->addLayout(hboxLayout3);


        verticalLayout_4->addWidget(databaseGroupBox);

        jobInfoGroupBox = new QGroupBox(tab_data);
        jobInfoGroupBox->setObjectName("jobInfoGroupBox");
        jobInfoLayout = new QHBoxLayout(jobInfoGroupBox);
        jobInfoLayout->setObjectName("jobInfoLayout");
        cellConfigTableWidget = new QTableWidget(jobInfoGroupBox);
        if (cellConfigTableWidget->columnCount() < 7)
            cellConfigTableWidget->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        cellConfigTableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        cellConfigTableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        cellConfigTableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        cellConfigTableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        cellConfigTableWidget->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        cellConfigTableWidget->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        cellConfigTableWidget->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        cellConfigTableWidget->setObjectName("cellConfigTableWidget");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(3);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(cellConfigTableWidget->sizePolicy().hasHeightForWidth());
        cellConfigTableWidget->setSizePolicy(sizePolicy);
        cellConfigTableWidget->setMinimumSize(QSize(0, 200));
        cellConfigTableWidget->setRowCount(0);
        cellConfigTableWidget->setColumnCount(7);
        cellConfigTableWidget->verticalHeader()->setVisible(false);

        jobInfoLayout->addWidget(cellConfigTableWidget);

        seederParamsGroupBox = new QGroupBox(jobInfoGroupBox);
        seederParamsGroupBox->setObjectName("seederParamsGroupBox");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(seederParamsGroupBox->sizePolicy().hasHeightForWidth());
        seederParamsGroupBox->setSizePolicy(sizePolicy1);
        seederParamsLayout = new QGridLayout(seederParamsGroupBox);
        seederParamsLayout->setObjectName("seederParamsLayout");
        rowSpacingLabel = new QLabel(seederParamsGroupBox);
        rowSpacingLabel->setObjectName("rowSpacingLabel");

        seederParamsLayout->addWidget(rowSpacingLabel, 0, 0, 1, 1);

        rowSpacingLineEdit = new QLineEdit(seederParamsGroupBox);
        rowSpacingLineEdit->setObjectName("rowSpacingLineEdit");

        seederParamsLayout->addWidget(rowSpacingLineEdit, 0, 1, 1, 1);

        seedHoleDistanceLabel = new QLabel(seederParamsGroupBox);
        seedHoleDistanceLabel->setObjectName("seedHoleDistanceLabel");

        seederParamsLayout->addWidget(seedHoleDistanceLabel, 1, 0, 1, 1);

        seedHoleDistanceLineEdit = new QLineEdit(seederParamsGroupBox);
        seedHoleDistanceLineEdit->setObjectName("seedHoleDistanceLineEdit");

        seederParamsLayout->addWidget(seedHoleDistanceLineEdit, 1, 1, 1, 1);

        seedSpeedRatioLabel = new QLabel(seederParamsGroupBox);
        seedSpeedRatioLabel->setObjectName("seedSpeedRatioLabel");

        seederParamsLayout->addWidget(seedSpeedRatioLabel, 2, 0, 1, 1);

        seedSpeedRatioLineEdit = new QLineEdit(seederParamsGroupBox);
        seedSpeedRatioLineEdit->setObjectName("seedSpeedRatioLineEdit");

        seederParamsLayout->addWidget(seedSpeedRatioLineEdit, 2, 1, 1, 1);

        seedRadiusLabel = new QLabel(seederParamsGroupBox);
        seedRadiusLabel->setObjectName("seedRadiusLabel");

        seederParamsLayout->addWidget(seedRadiusLabel, 3, 0, 1, 1);

        seedRadiusLineEdit = new QLineEdit(seederParamsGroupBox);
        seedRadiusLineEdit->setObjectName("seedRadiusLineEdit");

        seederParamsLayout->addWidget(seedRadiusLineEdit, 3, 1, 1, 1);

        seedAdsorbCountLabel = new QLabel(seederParamsGroupBox);
        seedAdsorbCountLabel->setObjectName("seedAdsorbCountLabel");

        seederParamsLayout->addWidget(seedAdsorbCountLabel, 4, 0, 1, 1);

        seedAdsorbCountLineEdit = new QLineEdit(seederParamsGroupBox);
        seedAdsorbCountLineEdit->setObjectName("seedAdsorbCountLineEdit");

        seederParamsLayout->addWidget(seedAdsorbCountLineEdit, 4, 1, 1, 1);

        seedDropHeightLabel = new QLabel(seederParamsGroupBox);
        seedDropHeightLabel->setObjectName("seedDropHeightLabel");

        seederParamsLayout->addWidget(seedDropHeightLabel, 5, 0, 1, 1);

        seedDropHeightLineEdit = new QLineEdit(seederParamsGroupBox);
        seedDropHeightLineEdit->setObjectName("seedDropHeightLineEdit");

        seederParamsLayout->addWidget(seedDropHeightLineEdit, 5, 1, 1, 1);

        antennaArmLabel = new QLabel(seederParamsGroupBox);
        antennaArmLabel->setObjectName("antennaArmLabel");

        seederParamsLayout->addWidget(antennaArmLabel, 6, 0, 1, 1);

        antennaArmLineEdit = new QLineEdit(seederParamsGroupBox);
        antennaArmLineEdit->setObjectName("antennaArmLineEdit");

        seederParamsLayout->addWidget(antennaArmLineEdit, 6, 1, 1, 1);

        distancePulseRatioLabel = new QLabel(seederParamsGroupBox);
        distancePulseRatioLabel->setObjectName("distancePulseRatioLabel");

        seederParamsLayout->addWidget(distancePulseRatioLabel, 7, 0, 1, 1);

        distancePulseRatioLineEdit = new QLineEdit(seederParamsGroupBox);
        distancePulseRatioLineEdit->setObjectName("distancePulseRatioLineEdit");

        seederParamsLayout->addWidget(distancePulseRatioLineEdit, 7, 1, 1, 1);

        signalTransmissionDelayLabel = new QLabel(seederParamsGroupBox);
        signalTransmissionDelayLabel->setObjectName("signalTransmissionDelayLabel");

        seederParamsLayout->addWidget(signalTransmissionDelayLabel, 8, 0, 1, 1);

        signalTransmissionDelayLineEdit = new QLineEdit(seederParamsGroupBox);
        signalTransmissionDelayLineEdit->setObjectName("signalTransmissionDelayLineEdit");

        seederParamsLayout->addWidget(signalTransmissionDelayLineEdit, 8, 1, 1, 1);

        reverseOrderCheckBox = new QCheckBox(seederParamsGroupBox);
        reverseOrderCheckBox->setObjectName("reverseOrderCheckBox");
        reverseOrderCheckBox->setMinimumSize(QSize(50, 20));
        reverseOrderCheckBox->setMaximumSize(QSize(100, 20));

        seederParamsLayout->addWidget(reverseOrderCheckBox, 9, 0, 1, 1);

        cellAreaLabel = new QLabel(seederParamsGroupBox);
        cellAreaLabel->setObjectName("cellAreaLabel");

        seederParamsLayout->addWidget(cellAreaLabel, 9, 1, 1, 1);

        x1ValueLabel = new QLabel(seederParamsGroupBox);
        x1ValueLabel->setObjectName("x1ValueLabel");

        seederParamsLayout->addWidget(x1ValueLabel, 10, 1, 1, 1);

        x2ValueLabel = new QLabel(seederParamsGroupBox);
        x2ValueLabel->setObjectName("x2ValueLabel");

        seederParamsLayout->addWidget(x2ValueLabel, 11, 1, 1, 1);


        jobInfoLayout->addWidget(seederParamsGroupBox);


        verticalLayout_4->addWidget(jobInfoGroupBox);

        operationControlGroupBox = new QGroupBox(tab_data);
        operationControlGroupBox->setObjectName("operationControlGroupBox");
        operationControlLayout = new QVBoxLayout(operationControlGroupBox);
        operationControlLayout->setObjectName("operationControlLayout");
        seedingPlanTableWidget = new QTableWidget(operationControlGroupBox);
        if (seedingPlanTableWidget->columnCount() < 11)
            seedingPlanTableWidget->setColumnCount(11);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        seedingPlanTableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        seedingPlanTableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        seedingPlanTableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        seedingPlanTableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        seedingPlanTableWidget->setHorizontalHeaderItem(4, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        seedingPlanTableWidget->setHorizontalHeaderItem(5, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        seedingPlanTableWidget->setHorizontalHeaderItem(6, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        seedingPlanTableWidget->setHorizontalHeaderItem(7, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        seedingPlanTableWidget->setHorizontalHeaderItem(8, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        seedingPlanTableWidget->setHorizontalHeaderItem(9, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        seedingPlanTableWidget->setHorizontalHeaderItem(10, __qtablewidgetitem17);
        seedingPlanTableWidget->setObjectName("seedingPlanTableWidget");
        seedingPlanTableWidget->setMinimumSize(QSize(0, 200));
        seedingPlanTableWidget->setRowCount(0);
        seedingPlanTableWidget->setColumnCount(11);
        seedingPlanTableWidget->verticalHeader()->setVisible(false);

        operationControlLayout->addWidget(seedingPlanTableWidget);


        verticalLayout_4->addWidget(operationControlGroupBox);

        tabWidget->addTab(tab_data, QString());

        verticalLayout->addWidget(tabWidget);

        timeAndSensorGroupBox = new QGroupBox(centralwidget);
        timeAndSensorGroupBox->setObjectName("timeAndSensorGroupBox");
        timeAndSensorLayout = new QVBoxLayout(timeAndSensorGroupBox);
        timeAndSensorLayout->setObjectName("timeAndSensorLayout");
        timeAndSensorTopLayout = new QHBoxLayout();
        timeAndSensorTopLayout->setObjectName("timeAndSensorTopLayout");
        timeLabel = new QLabel(timeAndSensorGroupBox);
        timeLabel->setObjectName("timeLabel");

        timeAndSensorTopLayout->addWidget(timeLabel);

        localTimeLabel = new QLabel(timeAndSensorGroupBox);
        localTimeLabel->setObjectName("localTimeLabel");

        timeAndSensorTopLayout->addWidget(localTimeLabel);

        horizontalSpacer_2 = new QSpacerItem(20, 1, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        timeAndSensorTopLayout->addItem(horizontalSpacer_2);

        gpsTimeLabel = new QLabel(timeAndSensorGroupBox);
        gpsTimeLabel->setObjectName("gpsTimeLabel");

        timeAndSensorTopLayout->addWidget(gpsTimeLabel);

        gpsTimeValueLabel = new QLabel(timeAndSensorGroupBox);
        gpsTimeValueLabel->setObjectName("gpsTimeValueLabel");

        timeAndSensorTopLayout->addWidget(gpsTimeValueLabel);

        horizontalSpacer_8 = new QSpacerItem(40, 1, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        timeAndSensorTopLayout->addItem(horizontalSpacer_8);


        timeAndSensorLayout->addLayout(timeAndSensorTopLayout);

        sensorDataGridLayout = new QGridLayout();
        sensorDataGridLayout->setSpacing(5);
        sensorDataGridLayout->setObjectName("sensorDataGridLayout");
        imuAccelerationLabel = new QLabel(timeAndSensorGroupBox);
        imuAccelerationLabel->setObjectName("imuAccelerationLabel");

        sensorDataGridLayout->addWidget(imuAccelerationLabel, 0, 0, 1, 1);

        gpsPositionLabel = new QLabel(timeAndSensorGroupBox);
        gpsPositionLabel->setObjectName("gpsPositionLabel");

        sensorDataGridLayout->addWidget(gpsPositionLabel, 0, 1, 1, 1);

        imuDthetaLabel = new QLabel(timeAndSensorGroupBox);
        imuDthetaLabel->setObjectName("imuDthetaLabel");

        sensorDataGridLayout->addWidget(imuDthetaLabel, 1, 0, 1, 1);

        speedLabel = new QLabel(timeAndSensorGroupBox);
        speedLabel->setObjectName("speedLabel");

        sensorDataGridLayout->addWidget(speedLabel, 1, 1, 1, 1);

        imuOutputMagneticHeadingLabel = new QLabel(timeAndSensorGroupBox);
        imuOutputMagneticHeadingLabel->setObjectName("imuOutputMagneticHeadingLabel");

        sensorDataGridLayout->addWidget(imuOutputMagneticHeadingLabel, 2, 0, 1, 1);

        magneticHeadingLabel = new QLabel(timeAndSensorGroupBox);
        magneticHeadingLabel->setObjectName("magneticHeadingLabel");

        sensorDataGridLayout->addWidget(magneticHeadingLabel, 2, 1, 1, 1);

        imuAngleDisplayLabel = new QLabel(timeAndSensorGroupBox);
        imuAngleDisplayLabel->setObjectName("imuAngleDisplayLabel");

        sensorDataGridLayout->addWidget(imuAngleDisplayLabel, 3, 0, 1, 1);

        nextTriggerLabel = new QLabel(timeAndSensorGroupBox);
        nextTriggerLabel->setObjectName("nextTriggerLabel");

        sensorDataGridLayout->addWidget(nextTriggerLabel, 3, 1, 1, 1);

        imuDataCountLabel = new QLabel(timeAndSensorGroupBox);
        imuDataCountLabel->setObjectName("imuDataCountLabel");

        sensorDataGridLayout->addWidget(imuDataCountLabel, 4, 0, 1, 1);

        gpsDataCountLabel = new QLabel(timeAndSensorGroupBox);
        gpsDataCountLabel->setObjectName("gpsDataCountLabel");

        sensorDataGridLayout->addWidget(gpsDataCountLabel, 4, 1, 1, 1);


        timeAndSensorLayout->addLayout(sensorDataGridLayout);


        verticalLayout->addWidget(timeAndSensorGroupBox);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1024, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName("menuEdit");
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName("menuHelp");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuEdit->addAction(actionUndo);
        menuEdit->addAction(actionRedo);
        menuEdit->addSeparator();
        menuEdit->addAction(actionCut);
        menuEdit->addAction(actionCopy);
        menuEdit->addAction(actionPaste);
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\345\271\262\345\260\261\345\256\214\344\272\206 v0.2", nullptr));
        actionOpen->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200", nullptr));
        actionSave->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "\351\200\200\345\207\272", nullptr));
        actionUndo->setText(QCoreApplication::translate("MainWindow", "\346\222\244\351\224\200", nullptr));
        actionRedo->setText(QCoreApplication::translate("MainWindow", "\351\207\215\345\201\232", nullptr));
        actionCut->setText(QCoreApplication::translate("MainWindow", "\345\211\252\345\210\207", nullptr));
        actionCopy->setText(QCoreApplication::translate("MainWindow", "\345\244\215\345\210\266", nullptr));
        actionPaste->setText(QCoreApplication::translate("MainWindow", "\347\262\230\350\264\264", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "\345\205\263\344\272\216", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_navigation), QCoreApplication::translate("MainWindow", "\344\270\273\347\252\227\345\217\243", nullptr));
        deviceStatusGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\350\256\276\345\244\207\347\212\266\346\200\201", nullptr));
        imuStatusLabel->setText(QCoreApplication::translate("MainWindow", "IMU\347\212\266\346\200\201: \342\232\252 \346\234\252\350\277\236\346\216\245", nullptr));
        imuConnectButton->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\220\257", nullptr));
        imuDisconnectButton->setText(QCoreApplication::translate("MainWindow", "\345\205\263\351\227\255", nullptr));
        gpsStatusLabel->setText(QCoreApplication::translate("MainWindow", "GPS\347\212\266\346\200\201: \342\232\252 \346\234\252\350\277\236\346\216\245", nullptr));
        gpsConnectButton->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\220\257", nullptr));
        gpsDisconnectButton->setText(QCoreApplication::translate("MainWindow", "\345\205\263\351\227\255", nullptr));
        plcStatusLabel->setText(QCoreApplication::translate("MainWindow", "PLC\347\212\266\346\200\201: \342\232\252 \346\234\252\350\277\236\346\216\245", nullptr));
        plcConnectButton->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230", nullptr));
        plcDisconnectButton->setText(QCoreApplication::translate("MainWindow", "\346\226\255\345\274\200", nullptr));
        imuSettingsGroupBox->setTitle(QCoreApplication::translate("MainWindow", "IMU\345\217\202\346\225\260\350\256\276\347\275\256", nullptr));
        imuPortLabel->setText(QCoreApplication::translate("MainWindow", "\344\270\262\345\217\243\345\217\267:", nullptr));
        imuPortComboBox->setItemText(0, QCoreApplication::translate("MainWindow", "COM1", nullptr));
        imuPortComboBox->setItemText(1, QCoreApplication::translate("MainWindow", "COM2", nullptr));
        imuPortComboBox->setItemText(2, QCoreApplication::translate("MainWindow", "COM3", nullptr));
        imuPortComboBox->setItemText(3, QCoreApplication::translate("MainWindow", "COM4", nullptr));

        imuBaudLabel->setText(QCoreApplication::translate("MainWindow", "\346\263\242\347\211\271\347\216\207:", nullptr));
        imuBaudComboBox->setItemText(0, QCoreApplication::translate("MainWindow", "9600", nullptr));
        imuBaudComboBox->setItemText(1, QCoreApplication::translate("MainWindow", "19200", nullptr));
        imuBaudComboBox->setItemText(2, QCoreApplication::translate("MainWindow", "38400", nullptr));
        imuBaudComboBox->setItemText(3, QCoreApplication::translate("MainWindow", "57600", nullptr));
        imuBaudComboBox->setItemText(4, QCoreApplication::translate("MainWindow", "115200", nullptr));

        imuConfigLabel->setText(QCoreApplication::translate("MainWindow", "IMU\346\234\211\346\225\210\346\225\260\346\215\256: 0", nullptr));
        gpsSettingsGroupBox->setTitle(QCoreApplication::translate("MainWindow", "GPS\345\217\202\346\225\260\350\256\276\347\275\256", nullptr));
        gpsPortLabel->setText(QCoreApplication::translate("MainWindow", "\344\270\262\345\217\243\345\217\267:", nullptr));
        gpsPortComboBox->setItemText(0, QCoreApplication::translate("MainWindow", "COM1", nullptr));
        gpsPortComboBox->setItemText(1, QCoreApplication::translate("MainWindow", "COM2", nullptr));
        gpsPortComboBox->setItemText(2, QCoreApplication::translate("MainWindow", "COM3", nullptr));
        gpsPortComboBox->setItemText(3, QCoreApplication::translate("MainWindow", "COM4", nullptr));

        gpsBaudLabel->setText(QCoreApplication::translate("MainWindow", "\346\263\242\347\211\271\347\216\207:", nullptr));
        gpsBaudComboBox->setItemText(0, QCoreApplication::translate("MainWindow", "9600", nullptr));
        gpsBaudComboBox->setItemText(1, QCoreApplication::translate("MainWindow", "19200", nullptr));
        gpsBaudComboBox->setItemText(2, QCoreApplication::translate("MainWindow", "38400", nullptr));
        gpsBaudComboBox->setItemText(3, QCoreApplication::translate("MainWindow", "57600", nullptr));
        gpsBaudComboBox->setItemText(4, QCoreApplication::translate("MainWindow", "115200", nullptr));

        gpsConfigLabel->setText(QCoreApplication::translate("MainWindow", "GPS\346\234\211\346\225\210\346\225\260\346\215\256: 0", nullptr));
        plcSettingsGroupBox->setTitle(QCoreApplication::translate("MainWindow", "PLC\345\217\202\346\225\260\350\256\276\347\275\256", nullptr));
        plcPortLabel->setText(QCoreApplication::translate("MainWindow", "\345\234\260\345\235\200:", nullptr));
        plcIpLineEdit->setText(QCoreApplication::translate("MainWindow", "127.0.0.1", nullptr));
        plcBaudLabel->setText(QCoreApplication::translate("MainWindow", "\347\253\257\345\217\243:", nullptr));
        plcPortLineEdit->setText(QCoreApplication::translate("MainWindow", "5020", nullptr));
        plcConfigLabel->setText(QCoreApplication::translate("MainWindow", "PLC\345\217\221\351\200\201\350\256\241\346\225\260: 0, \346\216\245\346\224\266\350\256\241\346\225\260: 0", nullptr));
        plcGroupBox->setTitle(QCoreApplication::translate("MainWindow", "PLC\345\257\204\345\255\230\345\231\250", nullptr));
        plcAddr1Label->setText(QCoreApplication::translate("MainWindow", "\345\257\204\345\255\230\345\231\250\345\234\260\345\235\200:", nullptr));
        plcAddr1LineEdit->setText(QCoreApplication::translate("MainWindow", "17", nullptr));
        plcCmdValue1Label->setText(QCoreApplication::translate("MainWindow", "\346\214\207\344\273\244\345\200\274:", nullptr));
        plcCmdValue1LineEdit->setText(QCoreApplication::translate("MainWindow", "1", nullptr));
        plcReadValue1TitleLabel->setText(QCoreApplication::translate("MainWindow", "\350\257\273\345\217\226\345\200\274:", nullptr));
        plcReadValue1Label->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        plcSave1Button->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230", nullptr));
        plcSend1Button->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        forwardTriggerLine1->setText(QCoreApplication::translate("MainWindow", "\346\255\243\345\220\221\350\247\246\345\217\221\347\272\277", nullptr));
        reverseTriggerLine1->setText(QCoreApplication::translate("MainWindow", "\345\217\215\345\220\221\350\247\246\345\217\221\347\272\277", nullptr));
        plcAddr2Label->setText(QCoreApplication::translate("MainWindow", "\345\257\204\345\255\230\345\231\250\345\234\260\345\235\200:", nullptr));
        plcAddr2LineEdit->setText(QCoreApplication::translate("MainWindow", "31", nullptr));
        plcCmdValue2Label->setText(QCoreApplication::translate("MainWindow", "\346\214\207\344\273\244\345\200\274:", nullptr));
        plcReadValue2TitleLabel->setText(QCoreApplication::translate("MainWindow", "\350\257\273\345\217\226\345\200\274:", nullptr));
        plcReadValue2Label->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        plcSave2Button->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230", nullptr));
        plcSend2Button->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        forwardTriggerLine2->setText(QCoreApplication::translate("MainWindow", "\346\255\243\345\220\221\350\247\246\345\217\221\347\272\277", nullptr));
        reverseTriggerLine2->setText(QCoreApplication::translate("MainWindow", "\345\217\215\345\220\221\350\247\246\345\217\221\347\272\277", nullptr));
        plcAddr3Label->setText(QCoreApplication::translate("MainWindow", "\345\257\204\345\255\230\345\231\250\345\234\260\345\235\200:", nullptr));
        plcAddr3LineEdit->setText(QCoreApplication::translate("MainWindow", "10", nullptr));
        plcCmdValue3Label->setText(QCoreApplication::translate("MainWindow", "\346\214\207\344\273\244\345\200\274:", nullptr));
        plcReadValue3TitleLabel->setText(QCoreApplication::translate("MainWindow", "\350\257\273\345\217\226\345\200\274:", nullptr));
        plcReadValue3Label->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        plcSave3Button->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230", nullptr));
        plcSend3Button->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        forwardTriggerLine3->setText(QCoreApplication::translate("MainWindow", "\346\255\243\345\220\221\350\247\246\345\217\221\347\272\277", nullptr));
        reverseTriggerLine3->setText(QCoreApplication::translate("MainWindow", "\345\217\215\345\220\221\350\247\246\345\217\221\347\272\277", nullptr));
        plcAddr4Label->setText(QCoreApplication::translate("MainWindow", "\345\257\204\345\255\230\345\231\250\345\234\260\345\235\200:", nullptr));
        plcAddr4LineEdit->setText(QCoreApplication::translate("MainWindow", "11", nullptr));
        plcCmdValue4Label->setText(QCoreApplication::translate("MainWindow", "\346\214\207\344\273\244\345\200\274:", nullptr));
        plcReadValue4TitleLabel->setText(QCoreApplication::translate("MainWindow", "\350\257\273\345\217\226\345\200\274:", nullptr));
        plcReadValue4Label->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        plcSave4Button->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230", nullptr));
        plcSend4Button->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        forwardTriggerLine4->setText(QCoreApplication::translate("MainWindow", "\346\255\243\345\220\221\350\247\246\345\217\221\347\272\277", nullptr));
        reverseTriggerLine4->setText(QCoreApplication::translate("MainWindow", "\345\217\215\345\220\221\350\247\246\345\217\221\347\272\277", nullptr));
        plcAddr5Label->setText(QCoreApplication::translate("MainWindow", "\345\257\204\345\255\230\345\231\250\345\234\260\345\235\200:", nullptr));
        plcAddr5LineEdit->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        plcCmdValue5Label->setText(QCoreApplication::translate("MainWindow", "\346\214\207\344\273\244\345\200\274:", nullptr));
        plcCmdValue5LineEdit->setText(QCoreApplication::translate("MainWindow", "1", nullptr));
        plcReadValue5TitleLabel->setText(QCoreApplication::translate("MainWindow", "\350\257\273\345\217\226\345\200\274:", nullptr));
        plcReadValue5Label->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        plcSave5Button->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230", nullptr));
        plcSend5Button->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        forwardTriggerLine5->setText(QCoreApplication::translate("MainWindow", "\346\255\243\345\220\221\350\247\246\345\217\221\347\272\277", nullptr));
        reverseTriggerLine5->setText(QCoreApplication::translate("MainWindow", "\345\217\215\345\220\221\350\247\246\345\217\221\347\272\277", nullptr));
        coordinateSystemGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\345\235\220\346\240\207\347\263\273\346\230\276\347\244\272", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\346\222\255\347\247\215\347\212\266\346\200\201\345\217\202\346\225\260", nullptr));
        areaLengthLabel->setText(QCoreApplication::translate("MainWindow", "\345\214\272\345\237\237\351\225\277\345\272\246 (m):", nullptr));
        areaLengthLabelValue->setText(QCoreApplication::translate("MainWindow", "100.0", nullptr));
        areaWidthLabel->setText(QCoreApplication::translate("MainWindow", "\345\214\272\345\237\237\345\256\275\345\272\246 (m):", nullptr));
        areaWidthLabelValue->setText(QCoreApplication::translate("MainWindow", "1000.0", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\345\260\217\345\214\272\351\225\277\345\272\246 (m):", nullptr));
        rowSpacingLabelValue->setText(QCoreApplication::translate("MainWindow", "5.0", nullptr));
        fieldLengthLabel->setText(QCoreApplication::translate("MainWindow", "\344\270\213\344\270\252\345\260\217\345\214\272\351\225\277\345\272\246 (m):", nullptr));
        fieldLengthLabelValue->setText(QCoreApplication::translate("MainWindow", "5.0", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\350\277\207\351\201\223\345\256\275\345\272\246 (m):", nullptr));
        headlandWidthLabelValue->setText(QCoreApplication::translate("MainWindow", "1.0", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\344\270\213\344\270\252\350\277\207\351\201\223\345\256\275\345\272\246 (m):", nullptr));
        rowsPerBlockLabelValue->setText(QCoreApplication::translate("MainWindow", "2", nullptr));
        seedsPerBlockLabel->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\346\240\252\350\267\235(cm):", nullptr));
        seedsPerBlockLabelValue->setText(QCoreApplication::translate("MainWindow", "5", nullptr));
        triggerAheadLabel->setText(QCoreApplication::translate("MainWindow", "\350\247\246\345\217\221\347\272\277\346\217\220\345\211\215\351\207\217(m):", nullptr));
        triggerAheadLabelValue->setText(QCoreApplication::translate("MainWindow", "1.0", nullptr));
        pathPlanningGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\350\267\257\345\276\204\350\247\204\345\210\222\345\217\202\346\225\260", nullptr));
        oPointLabel->setText(QCoreApplication::translate("MainWindow", "O\347\202\271\347\273\217\347\272\254\345\272\246:", nullptr));
        oPointLineEdit->setText(QCoreApplication::translate("MainWindow", "0.000000,0.000000", nullptr));
        aPointLabel->setText(QCoreApplication::translate("MainWindow", "A\347\202\271\347\273\217\347\272\254\345\272\246:", nullptr));
        aPointLineEdit->setText(QCoreApplication::translate("MainWindow", "0.000001,0.000000", nullptr));
        generateConfigButton->setText(QCoreApplication::translate("MainWindow", "\347\224\237\346\210\220\346\234\254\345\234\260\345\235\220\346\240\207\347\263\273", nullptr));
        trajectoryVisibleCheckBox->setText(QCoreApplication::translate("MainWindow", "\346\230\276\347\244\272\350\277\220\345\212\250\350\275\250\350\277\271", nullptr));
        clearConfigButton->setText(QCoreApplication::translate("MainWindow", "\346\270\205\351\231\244\350\275\250\350\277\271\347\202\271", nullptr));
        loadConfigButton->setText(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256\344\270\272\345\216\237\347\202\271", nullptr));
        saveConfigButton->setText(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256\344\270\272A\347\202\271", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_settings), QCoreApplication::translate("MainWindow", "\351\205\215\347\275\256\347\252\227\345\217\243", nullptr));
        databaseGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\346\222\255\347\247\215\345\217\202\346\225\260", nullptr));
        databaseStatusLabel->setText(QCoreApplication::translate("MainWindow", "\346\222\255\347\247\215\350\256\241\345\210\222\357\274\232\346\234\252\345\212\240\350\275\275", nullptr));
        connectDatabaseButton->setText(QCoreApplication::translate("MainWindow", "\350\275\275\345\205\245\345\260\217\345\214\272\350\241\250", nullptr));
        generateSeedingPlanButton->setText(QCoreApplication::translate("MainWindow", "\347\224\237\346\210\220\346\222\255\347\247\215\350\256\241\345\210\222", nullptr));
        antennaCalibrationButton->setText(QCoreApplication::translate("MainWindow", "\345\244\251\347\272\277\346\240\241\345\207\206", nullptr));
        lockSeedingPlanButton->setText(QCoreApplication::translate("MainWindow", "\351\224\201\345\256\232\346\222\255\347\247\215\350\256\241\345\210\222", nullptr));
        configFileLabel->setText(QCoreApplication::translate("MainWindow", "\351\205\215\346\222\255\347\247\215\350\256\241\345\210\222\347\256\241\347\220\206", nullptr));
        saveConfigFileButton->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\350\256\241\345\210\222", nullptr));
        loadConfigFileButton->setText(QCoreApplication::translate("MainWindow", "\345\212\240\350\275\275\350\256\241\345\210\222", nullptr));
        jobInfoGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\345\260\217\345\214\272\351\205\215\347\275\256\350\241\250", nullptr));
        QTableWidgetItem *___qtablewidgetitem = cellConfigTableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "\345\272\217\345\217\267", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = cellConfigTableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "\346\226\271\345\217\267", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = cellConfigTableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "\347\247\215\347\261\273", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = cellConfigTableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "\346\255\243\345\214\272\345\256\275\345\272\246(m)", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = cellConfigTableWidget->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "\350\277\207\351\201\223\345\256\275\345\272\246(m)", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = cellConfigTableWidget->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "\346\240\252\350\267\235(cm)", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = cellConfigTableWidget->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "\345\236\204\346\225\260", nullptr));
        seederParamsGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\346\216\222\347\247\215\345\231\250\345\217\202\346\225\260", nullptr));
        rowSpacingLabel->setText(QCoreApplication::translate("MainWindow", "\345\236\204\350\267\235(m):", nullptr));
        rowSpacingLineEdit->setText(QCoreApplication::translate("MainWindow", "0.6", nullptr));
        seedHoleDistanceLabel->setText(QCoreApplication::translate("MainWindow", "\346\216\222\347\247\215\345\231\250\345\255\224\350\267\235(cm):", nullptr));
        seedHoleDistanceLineEdit->setText(QCoreApplication::translate("MainWindow", "3.0", nullptr));
        seedSpeedRatioLabel->setText(QCoreApplication::translate("MainWindow", "\350\275\254\351\200\237\345\217\202\346\225\260(\316\270,\317\211):", nullptr));
        seedSpeedRatioLineEdit->setText(QCoreApplication::translate("MainWindow", "10,0", nullptr));
        seedRadiusLabel->setText(QCoreApplication::translate("MainWindow", "\346\216\222\347\247\215\345\231\250\345\215\212\345\276\204(cm):", nullptr));
        seedRadiusLineEdit->setText(QCoreApplication::translate("MainWindow", "20", nullptr));
        seedAdsorbCountLabel->setText(QCoreApplication::translate("MainWindow", "\346\216\222\347\247\215\345\231\250\345\220\270\351\231\204\346\225\260N:", nullptr));
        seedAdsorbCountLineEdit->setText(QCoreApplication::translate("MainWindow", "8", nullptr));
        seedDropHeightLabel->setText(QCoreApplication::translate("MainWindow", "\350\220\275\347\247\215\345\217\243\351\253\230\345\272\246(m):", nullptr));
        seedDropHeightLineEdit->setText(QCoreApplication::translate("MainWindow", "0.3", nullptr));
        antennaArmLabel->setText(QCoreApplication::translate("MainWindow", "\345\244\251\347\272\277\346\235\206\350\207\202(X,Y,Z):", nullptr));
        antennaArmLineEdit->setText(QCoreApplication::translate("MainWindow", "0,0,0", nullptr));
        distancePulseRatioLabel->setText(QCoreApplication::translate("MainWindow", "\350\267\235\347\246\273\350\204\211\345\206\262\346\257\224\344\276\213(\316\274):", nullptr));
        distancePulseRatioLineEdit->setText(QCoreApplication::translate("MainWindow", "125", nullptr));
        signalTransmissionDelayLabel->setText(QCoreApplication::translate("MainWindow", "\344\277\241\345\217\267\344\274\240\350\276\223\345\273\266\350\277\237(ms):", nullptr));
        signalTransmissionDelayLineEdit->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        reverseOrderCheckBox->setText(QCoreApplication::translate("MainWindow", "\345\200\222\345\272\217", nullptr));
        cellAreaLabel->setText(QCoreApplication::translate("MainWindow", "\345\260\217\345\214\272\346\200\273\351\225\277\345\256\275: --", nullptr));
        x1ValueLabel->setText(QCoreApplication::translate("MainWindow", "X1\345\200\274: --", nullptr));
        x2ValueLabel->setText(QCoreApplication::translate("MainWindow", "X2\345\200\274: --", nullptr));
        operationControlGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\346\222\255\347\247\215\350\256\241\345\210\222\350\241\250", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = seedingPlanTableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "\345\272\217\345\217\267", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = seedingPlanTableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("MainWindow", "\346\226\271\345\217\267", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = seedingPlanTableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("MainWindow", "\345\211\215\n"
"\345\210\206\345\211\262\347\272\277(m)", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = seedingPlanTableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("MainWindow", "\345\220\216\n"
"\345\210\206\345\211\262\347\272\277(m)", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = seedingPlanTableWidget->horizontalHeaderItem(4);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("MainWindow", "\346\255\243\345\220\221\n"
"\350\220\275\347\247\215\347\202\271(m)", nullptr));
        QTableWidgetItem *___qtablewidgetitem12 = seedingPlanTableWidget->horizontalHeaderItem(5);
        ___qtablewidgetitem12->setText(QCoreApplication::translate("MainWindow", "\345\217\215\345\220\221\n"
"\350\220\275\347\247\215\347\202\271(m)", nullptr));
        QTableWidgetItem *___qtablewidgetitem13 = seedingPlanTableWidget->horizontalHeaderItem(6);
        ___qtablewidgetitem13->setText(QCoreApplication::translate("MainWindow", "\346\255\243\345\220\221\n"
"\350\247\246\345\217\221\347\272\277(m)", nullptr));
        QTableWidgetItem *___qtablewidgetitem14 = seedingPlanTableWidget->horizontalHeaderItem(7);
        ___qtablewidgetitem14->setText(QCoreApplication::translate("MainWindow", "\345\217\215\345\220\221\n"
"\350\247\246\345\217\221\347\272\277(m)", nullptr));
        QTableWidgetItem *___qtablewidgetitem15 = seedingPlanTableWidget->horizontalHeaderItem(8);
        ___qtablewidgetitem15->setText(QCoreApplication::translate("MainWindow", "\346\255\243\345\220\221\n"
"\350\247\246\345\217\221\345\217\202\346\225\260", nullptr));
        QTableWidgetItem *___qtablewidgetitem16 = seedingPlanTableWidget->horizontalHeaderItem(9);
        ___qtablewidgetitem16->setText(QCoreApplication::translate("MainWindow", "\345\217\215\345\220\221\n"
"\350\247\246\345\217\221\345\217\202\346\225\260", nullptr));
        QTableWidgetItem *___qtablewidgetitem17 = seedingPlanTableWidget->horizontalHeaderItem(10);
        ___qtablewidgetitem17->setText(QCoreApplication::translate("MainWindow", "\346\240\252\350\267\235\345\217\202\346\225\260", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_data), QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\347\252\227\345\217\243", nullptr));
        timeAndSensorGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\346\246\202\350\246\201\344\277\241\346\201\257", nullptr));
        timeLabel->setText(QCoreApplication::translate("MainWindow", "\346\234\254\345\234\260\346\227\266\351\227\264:", nullptr));
        localTimeLabel->setText(QCoreApplication::translate("MainWindow", "2024-01-01 00:00:00", nullptr));
        gpsTimeLabel->setText(QCoreApplication::translate("MainWindow", "\345\215\253\346\230\237\346\227\266\351\227\264:", nullptr));
        gpsTimeValueLabel->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        imuAccelerationLabel->setText(QCoreApplication::translate("MainWindow", "\345\212\240\351\200\237\345\272\246: ", nullptr));
        gpsPositionLabel->setText(QCoreApplication::translate("MainWindow", "\347\273\217\347\272\254\345\272\246/\351\253\230\345\272\246: (0.000000, 0.000000, 0.00 m)", nullptr));
        imuDthetaLabel->setText(QCoreApplication::translate("MainWindow", "\350\247\222\351\200\237\345\272\246\345\242\236\351\207\217: ", nullptr));
        speedLabel->setText(QCoreApplication::translate("MainWindow", "\351\200\237\345\272\246: 0.00 m/s, \345\214\227\345\220\221: 0.00 m/s, \344\270\234\345\220\221: 0.00 m/s", nullptr));
        imuOutputMagneticHeadingLabel->setText(QCoreApplication::translate("MainWindow", "IMU\347\243\201\350\210\252\345\220\221: ", nullptr));
        magneticHeadingLabel->setText(QCoreApplication::translate("MainWindow", "\347\243\201\346\226\271\344\275\215\350\247\222: 0.00 \302\260", nullptr));
        imuAngleDisplayLabel->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\350\247\222\345\272\246: (0.00, 0.00, 0.00) \302\260", nullptr));
        nextTriggerLabel->setText(QCoreApplication::translate("MainWindow", "\350\267\235\344\270\213\344\270\200\346\254\241\350\247\246\345\217\221:", nullptr));
        imuDataCountLabel->setText(QCoreApplication::translate("MainWindow", "\346\234\211\346\225\210IMU\346\225\260\346\215\256: 0", nullptr));
        gpsDataCountLabel->setText(QCoreApplication::translate("MainWindow", "\346\234\211\346\225\210GPS\346\225\260\346\215\256: 0", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266", nullptr));
        menuEdit->setTitle(QCoreApplication::translate("MainWindow", "\347\274\226\350\276\221", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "\345\270\256\345\212\251", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
