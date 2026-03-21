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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
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
    QHBoxLayout *plcLayout;
    QGroupBox *plcInputGroupBox;
    QVBoxLayout *plcInputLayout;
    QLabel *plcInputDataLabel;
    QGroupBox *plcOutputGroupBox;
    QVBoxLayout *plcOutputLayout;
    QTextEdit *plcCommandTextEdit;
    QPushButton *plcSendButton;
    QGroupBox *coordinateSystemGroupBox;
    QVBoxLayout *coordinateSystemLayout;
    TableWidget *coordinateSystemWidget;
    QHBoxLayout *parametersLayout;
    QGroupBox *groupBox;
    QGridLayout *formLayout;
    QLabel *areaLengthLabel;
    QDoubleSpinBox *areaLengthSpinBox;
    QLabel *areaWidthLabel;
    QDoubleSpinBox *areaWidthSpinBox;
    QLabel *label;
    QDoubleSpinBox *rowSpacingSpinBox;
    QLabel *fieldLengthLabel;
    QDoubleSpinBox *fieldLengthSpinBox;
    QLabel *label_2;
    QDoubleSpinBox *headlandWidthSpinBox;
    QLabel *label_3;
    QSpinBox *rowsPerBlockSpinBox;
    QLabel *seedsPerBlockLabel;
    QSpinBox *seedsPerBlockSpinBox;
    QLabel *triggerDistanceLabel;
    QDoubleSpinBox *triggerDistanceSpinBox;
    QLabel *stopTriggerDistanceLabel;
    QDoubleSpinBox *stopTriggerDistanceSpinBox;
    QGroupBox *pathPlanningGroupBox;
    QGridLayout *pathPlanningFormLayout;
    QLabel *oPointLatitudeLabel;
    QDoubleSpinBox *oPointLatitudeSpinBox;
    QLabel *oPointLongitudeLabel;
    QDoubleSpinBox *oPointLongitudeSpinBox;
    QLabel *aPointLatitudeLabel;
    QDoubleSpinBox *aPointLatitudeSpinBox;
    QLabel *aPointLongitudeLabel;
    QDoubleSpinBox *aPointLongitudeSpinBox;
    QLabel *speedPulseFactorLabel;
    QDoubleSpinBox *speedPulseFactorSpinBox;
    QLabel *speedFrequencyFactorLabel;
    QDoubleSpinBox *speedFrequencyFactorSpinBox;
    QHBoxLayout *configFileLayout;
    QPushButton *generateConfigButton;
    QPushButton *clearConfigButton;
    QPushButton *loadConfigButton;
    QPushButton *saveConfigButton;
    QSpacerItem *verticalSpacer;
    QWidget *tab_data;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *databaseGroupBox;
    QVBoxLayout *databaseLayout;
    QLabel *databaseStatusLabel;
    QPushButton *connectDatabaseButton;
    QGroupBox *jobInfoGroupBox;
    QHBoxLayout *jobInfoLayout;
    QLabel *jobIdLabel;
    QLabel *jobIdValueLabel;
    QSpacerItem *horizontalSpacer_4;
    QLabel *jobDateLabel;
    QLabel *jobDateValueLabel;
    QSpacerItem *horizontalSpacer_5;
    QLabel *jobAreaLabel;
    QLabel *jobAreaValueLabel;
    QSpacerItem *horizontalSpacer_6;
    QLabel *jobSeedlingsLabel;
    QLabel *jobSeedlingsValueLabel;
    QSpacerItem *horizontalSpacer_7;
    QGroupBox *operationControlGroupBox;
    QVBoxLayout *operationControlLayout;
    QHBoxLayout *seedingStatusLayout;
    QLabel *seedingStatusLabel;
    QLabel *seedingStatusIndicator;
    QSpacerItem *horizontalSpacer;
    QListWidget *fieldListWidget;
    QGridLayout *controlDataGridLayout;
    QLabel *pathPlanningLabel;
    QLabel *currentSectionLabel;
    QLabel *targetPointLabel;
    QLabel *triggerPointLabel;
    QLabel *operationDataLabel;
    QLabel *seedlingsCountLabel;
    QLabel *operationProgressLabel;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *configButton;
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
        plcLayout = new QHBoxLayout(plcGroupBox);
        plcLayout->setObjectName("plcLayout");
        plcInputGroupBox = new QGroupBox(plcGroupBox);
        plcInputGroupBox->setObjectName("plcInputGroupBox");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(plcInputGroupBox->sizePolicy().hasHeightForWidth());
        plcInputGroupBox->setSizePolicy(sizePolicy);
        plcInputLayout = new QVBoxLayout(plcInputGroupBox);
        plcInputLayout->setObjectName("plcInputLayout");
        plcInputDataLabel = new QLabel(plcInputGroupBox);
        plcInputDataLabel->setObjectName("plcInputDataLabel");

        plcInputLayout->addWidget(plcInputDataLabel);


        plcLayout->addWidget(plcInputGroupBox);

        plcOutputGroupBox = new QGroupBox(plcGroupBox);
        plcOutputGroupBox->setObjectName("plcOutputGroupBox");
        sizePolicy.setHeightForWidth(plcOutputGroupBox->sizePolicy().hasHeightForWidth());
        plcOutputGroupBox->setSizePolicy(sizePolicy);
        plcOutputLayout = new QVBoxLayout(plcOutputGroupBox);
        plcOutputLayout->setObjectName("plcOutputLayout");
        plcCommandTextEdit = new QTextEdit(plcOutputGroupBox);
        plcCommandTextEdit->setObjectName("plcCommandTextEdit");

        plcOutputLayout->addWidget(plcCommandTextEdit);

        plcSendButton = new QPushButton(plcOutputGroupBox);
        plcSendButton->setObjectName("plcSendButton");

        plcOutputLayout->addWidget(plcSendButton);


        plcLayout->addWidget(plcOutputGroupBox);


        verticalLayout_3->addWidget(plcGroupBox);

        coordinateSystemGroupBox = new QGroupBox(tab_settings);
        coordinateSystemGroupBox->setObjectName("coordinateSystemGroupBox");
        coordinateSystemLayout = new QVBoxLayout(coordinateSystemGroupBox);
        coordinateSystemLayout->setObjectName("coordinateSystemLayout");
        coordinateSystemWidget = new TableWidget(coordinateSystemGroupBox);
        coordinateSystemWidget->setObjectName("coordinateSystemWidget");
        coordinateSystemWidget->setMinimumSize(QSize(0, 200));

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

        areaLengthSpinBox = new QDoubleSpinBox(groupBox);
        areaLengthSpinBox->setObjectName("areaLengthSpinBox");
        areaLengthSpinBox->setDecimals(1);
        areaLengthSpinBox->setMinimum(1.000000000000000);
        areaLengthSpinBox->setMaximum(1000.000000000000000);
        areaLengthSpinBox->setSingleStep(1.000000000000000);
        areaLengthSpinBox->setValue(100.000000000000000);

        formLayout->addWidget(areaLengthSpinBox, 0, 1, 1, 1);

        areaWidthLabel = new QLabel(groupBox);
        areaWidthLabel->setObjectName("areaWidthLabel");

        formLayout->addWidget(areaWidthLabel, 0, 2, 1, 1);

        areaWidthSpinBox = new QDoubleSpinBox(groupBox);
        areaWidthSpinBox->setObjectName("areaWidthSpinBox");
        areaWidthSpinBox->setDecimals(1);
        areaWidthSpinBox->setMinimum(1.000000000000000);
        areaWidthSpinBox->setMaximum(2000.000000000000000);
        areaWidthSpinBox->setSingleStep(10.000000000000000);
        areaWidthSpinBox->setValue(1000.000000000000000);

        formLayout->addWidget(areaWidthSpinBox, 0, 3, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName("label");

        formLayout->addWidget(label, 1, 0, 1, 1);

        rowSpacingSpinBox = new QDoubleSpinBox(groupBox);
        rowSpacingSpinBox->setObjectName("rowSpacingSpinBox");
        rowSpacingSpinBox->setDecimals(1);
        rowSpacingSpinBox->setMinimum(0.100000000000000);
        rowSpacingSpinBox->setMaximum(2.000000000000000);
        rowSpacingSpinBox->setSingleStep(0.100000000000000);
        rowSpacingSpinBox->setValue(0.600000000000000);

        formLayout->addWidget(rowSpacingSpinBox, 1, 1, 1, 1);

        fieldLengthLabel = new QLabel(groupBox);
        fieldLengthLabel->setObjectName("fieldLengthLabel");

        formLayout->addWidget(fieldLengthLabel, 1, 2, 1, 1);

        fieldLengthSpinBox = new QDoubleSpinBox(groupBox);
        fieldLengthSpinBox->setObjectName("fieldLengthSpinBox");
        fieldLengthSpinBox->setDecimals(1);
        fieldLengthSpinBox->setMinimum(1.000000000000000);
        fieldLengthSpinBox->setMaximum(100.000000000000000);
        fieldLengthSpinBox->setSingleStep(0.100000000000000);
        fieldLengthSpinBox->setValue(5.000000000000000);

        formLayout->addWidget(fieldLengthSpinBox, 1, 3, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");

        formLayout->addWidget(label_2, 2, 0, 1, 1);

        headlandWidthSpinBox = new QDoubleSpinBox(groupBox);
        headlandWidthSpinBox->setObjectName("headlandWidthSpinBox");
        headlandWidthSpinBox->setDecimals(1);
        headlandWidthSpinBox->setMinimum(1.000000000000000);
        headlandWidthSpinBox->setMaximum(5.000000000000000);
        headlandWidthSpinBox->setSingleStep(0.100000000000000);
        headlandWidthSpinBox->setValue(1.000000000000000);

        formLayout->addWidget(headlandWidthSpinBox, 2, 1, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");

        formLayout->addWidget(label_3, 2, 2, 1, 1);

        rowsPerBlockSpinBox = new QSpinBox(groupBox);
        rowsPerBlockSpinBox->setObjectName("rowsPerBlockSpinBox");
        rowsPerBlockSpinBox->setMinimum(1);
        rowsPerBlockSpinBox->setMaximum(50);
        rowsPerBlockSpinBox->setValue(2);

        formLayout->addWidget(rowsPerBlockSpinBox, 2, 3, 1, 1);

        seedsPerBlockLabel = new QLabel(groupBox);
        seedsPerBlockLabel->setObjectName("seedsPerBlockLabel");

        formLayout->addWidget(seedsPerBlockLabel, 3, 0, 1, 1);

        seedsPerBlockSpinBox = new QSpinBox(groupBox);
        seedsPerBlockSpinBox->setObjectName("seedsPerBlockSpinBox");
        seedsPerBlockSpinBox->setMinimum(1);
        seedsPerBlockSpinBox->setMaximum(10000);
        seedsPerBlockSpinBox->setValue(25);

        formLayout->addWidget(seedsPerBlockSpinBox, 3, 1, 1, 1);

        triggerDistanceLabel = new QLabel(groupBox);
        triggerDistanceLabel->setObjectName("triggerDistanceLabel");

        formLayout->addWidget(triggerDistanceLabel, 3, 2, 1, 1);

        triggerDistanceSpinBox = new QDoubleSpinBox(groupBox);
        triggerDistanceSpinBox->setObjectName("triggerDistanceSpinBox");
        triggerDistanceSpinBox->setDecimals(1);
        triggerDistanceSpinBox->setMinimum(0.100000000000000);
        triggerDistanceSpinBox->setMaximum(5.000000000000000);
        triggerDistanceSpinBox->setSingleStep(0.100000000000000);
        triggerDistanceSpinBox->setValue(1.000000000000000);

        formLayout->addWidget(triggerDistanceSpinBox, 3, 3, 1, 1);

        stopTriggerDistanceLabel = new QLabel(groupBox);
        stopTriggerDistanceLabel->setObjectName("stopTriggerDistanceLabel");

        formLayout->addWidget(stopTriggerDistanceLabel, 4, 0, 1, 1);

        stopTriggerDistanceSpinBox = new QDoubleSpinBox(groupBox);
        stopTriggerDistanceSpinBox->setObjectName("stopTriggerDistanceSpinBox");
        stopTriggerDistanceSpinBox->setDecimals(1);
        stopTriggerDistanceSpinBox->setMinimum(0.100000000000000);
        stopTriggerDistanceSpinBox->setMaximum(5.000000000000000);
        stopTriggerDistanceSpinBox->setSingleStep(0.100000000000000);
        stopTriggerDistanceSpinBox->setValue(0.500000000000000);

        formLayout->addWidget(stopTriggerDistanceSpinBox, 4, 1, 1, 1);


        parametersLayout->addWidget(groupBox);

        pathPlanningGroupBox = new QGroupBox(tab_settings);
        pathPlanningGroupBox->setObjectName("pathPlanningGroupBox");
        pathPlanningFormLayout = new QGridLayout(pathPlanningGroupBox);
        pathPlanningFormLayout->setObjectName("pathPlanningFormLayout");
        oPointLatitudeLabel = new QLabel(pathPlanningGroupBox);
        oPointLatitudeLabel->setObjectName("oPointLatitudeLabel");

        pathPlanningFormLayout->addWidget(oPointLatitudeLabel, 0, 0, 1, 1);

        oPointLatitudeSpinBox = new QDoubleSpinBox(pathPlanningGroupBox);
        oPointLatitudeSpinBox->setObjectName("oPointLatitudeSpinBox");
        oPointLatitudeSpinBox->setDecimals(6);
        oPointLatitudeSpinBox->setMinimum(-90.000000000000000);
        oPointLatitudeSpinBox->setMaximum(90.000000000000000);
        oPointLatitudeSpinBox->setSingleStep(0.000001000000000);
        oPointLatitudeSpinBox->setValue(0.000000000000000);

        pathPlanningFormLayout->addWidget(oPointLatitudeSpinBox, 0, 1, 1, 1);

        oPointLongitudeLabel = new QLabel(pathPlanningGroupBox);
        oPointLongitudeLabel->setObjectName("oPointLongitudeLabel");

        pathPlanningFormLayout->addWidget(oPointLongitudeLabel, 0, 2, 1, 1);

        oPointLongitudeSpinBox = new QDoubleSpinBox(pathPlanningGroupBox);
        oPointLongitudeSpinBox->setObjectName("oPointLongitudeSpinBox");
        oPointLongitudeSpinBox->setDecimals(6);
        oPointLongitudeSpinBox->setMinimum(-180.000000000000000);
        oPointLongitudeSpinBox->setMaximum(180.000000000000000);
        oPointLongitudeSpinBox->setSingleStep(0.000001000000000);
        oPointLongitudeSpinBox->setValue(0.000000000000000);

        pathPlanningFormLayout->addWidget(oPointLongitudeSpinBox, 0, 3, 1, 1);

        aPointLatitudeLabel = new QLabel(pathPlanningGroupBox);
        aPointLatitudeLabel->setObjectName("aPointLatitudeLabel");

        pathPlanningFormLayout->addWidget(aPointLatitudeLabel, 1, 0, 1, 1);

        aPointLatitudeSpinBox = new QDoubleSpinBox(pathPlanningGroupBox);
        aPointLatitudeSpinBox->setObjectName("aPointLatitudeSpinBox");
        aPointLatitudeSpinBox->setDecimals(6);
        aPointLatitudeSpinBox->setMinimum(-90.000000000000000);
        aPointLatitudeSpinBox->setMaximum(90.000000000000000);
        aPointLatitudeSpinBox->setSingleStep(0.000001000000000);
        aPointLatitudeSpinBox->setValue(0.000001000000000);

        pathPlanningFormLayout->addWidget(aPointLatitudeSpinBox, 1, 1, 1, 1);

        aPointLongitudeLabel = new QLabel(pathPlanningGroupBox);
        aPointLongitudeLabel->setObjectName("aPointLongitudeLabel");

        pathPlanningFormLayout->addWidget(aPointLongitudeLabel, 1, 2, 1, 1);

        aPointLongitudeSpinBox = new QDoubleSpinBox(pathPlanningGroupBox);
        aPointLongitudeSpinBox->setObjectName("aPointLongitudeSpinBox");
        aPointLongitudeSpinBox->setDecimals(6);
        aPointLongitudeSpinBox->setMinimum(-180.000000000000000);
        aPointLongitudeSpinBox->setMaximum(180.000000000000000);
        aPointLongitudeSpinBox->setSingleStep(0.000001000000000);
        aPointLongitudeSpinBox->setValue(0.000000000000000);

        pathPlanningFormLayout->addWidget(aPointLongitudeSpinBox, 1, 3, 1, 1);

        speedPulseFactorLabel = new QLabel(pathPlanningGroupBox);
        speedPulseFactorLabel->setObjectName("speedPulseFactorLabel");

        pathPlanningFormLayout->addWidget(speedPulseFactorLabel, 2, 0, 1, 1);

        speedPulseFactorSpinBox = new QDoubleSpinBox(pathPlanningGroupBox);
        speedPulseFactorSpinBox->setObjectName("speedPulseFactorSpinBox");
        speedPulseFactorSpinBox->setDecimals(2);
        speedPulseFactorSpinBox->setMinimum(0.000000000000000);
        speedPulseFactorSpinBox->setMaximum(100.000000000000000);
        speedPulseFactorSpinBox->setSingleStep(0.010000000000000);
        speedPulseFactorSpinBox->setValue(1.000000000000000);

        pathPlanningFormLayout->addWidget(speedPulseFactorSpinBox, 2, 1, 1, 1);

        speedFrequencyFactorLabel = new QLabel(pathPlanningGroupBox);
        speedFrequencyFactorLabel->setObjectName("speedFrequencyFactorLabel");

        pathPlanningFormLayout->addWidget(speedFrequencyFactorLabel, 2, 2, 1, 1);

        speedFrequencyFactorSpinBox = new QDoubleSpinBox(pathPlanningGroupBox);
        speedFrequencyFactorSpinBox->setObjectName("speedFrequencyFactorSpinBox");
        speedFrequencyFactorSpinBox->setDecimals(2);
        speedFrequencyFactorSpinBox->setMinimum(0.000000000000000);
        speedFrequencyFactorSpinBox->setMaximum(100.000000000000000);
        speedFrequencyFactorSpinBox->setSingleStep(0.010000000000000);
        speedFrequencyFactorSpinBox->setValue(1.000000000000000);

        pathPlanningFormLayout->addWidget(speedFrequencyFactorSpinBox, 2, 3, 1, 1);


        parametersLayout->addWidget(pathPlanningGroupBox);


        verticalLayout_3->addLayout(parametersLayout);

        configFileLayout = new QHBoxLayout();
        configFileLayout->setObjectName("configFileLayout");
        generateConfigButton = new QPushButton(tab_settings);
        generateConfigButton->setObjectName("generateConfigButton");

        configFileLayout->addWidget(generateConfigButton);

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

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

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

        connectDatabaseButton = new QPushButton(databaseGroupBox);
        connectDatabaseButton->setObjectName("connectDatabaseButton");

        databaseLayout->addWidget(connectDatabaseButton);


        verticalLayout_4->addWidget(databaseGroupBox);

        jobInfoGroupBox = new QGroupBox(tab_data);
        jobInfoGroupBox->setObjectName("jobInfoGroupBox");
        jobInfoLayout = new QHBoxLayout(jobInfoGroupBox);
        jobInfoLayout->setObjectName("jobInfoLayout");
        jobIdLabel = new QLabel(jobInfoGroupBox);
        jobIdLabel->setObjectName("jobIdLabel");

        jobInfoLayout->addWidget(jobIdLabel);

        jobIdValueLabel = new QLabel(jobInfoGroupBox);
        jobIdValueLabel->setObjectName("jobIdValueLabel");

        jobInfoLayout->addWidget(jobIdValueLabel);

        horizontalSpacer_4 = new QSpacerItem(20, 1, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        jobInfoLayout->addItem(horizontalSpacer_4);

        jobDateLabel = new QLabel(jobInfoGroupBox);
        jobDateLabel->setObjectName("jobDateLabel");

        jobInfoLayout->addWidget(jobDateLabel);

        jobDateValueLabel = new QLabel(jobInfoGroupBox);
        jobDateValueLabel->setObjectName("jobDateValueLabel");

        jobInfoLayout->addWidget(jobDateValueLabel);

        horizontalSpacer_5 = new QSpacerItem(20, 1, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        jobInfoLayout->addItem(horizontalSpacer_5);

        jobAreaLabel = new QLabel(jobInfoGroupBox);
        jobAreaLabel->setObjectName("jobAreaLabel");

        jobInfoLayout->addWidget(jobAreaLabel);

        jobAreaValueLabel = new QLabel(jobInfoGroupBox);
        jobAreaValueLabel->setObjectName("jobAreaValueLabel");

        jobInfoLayout->addWidget(jobAreaValueLabel);

        horizontalSpacer_6 = new QSpacerItem(20, 1, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        jobInfoLayout->addItem(horizontalSpacer_6);

        jobSeedlingsLabel = new QLabel(jobInfoGroupBox);
        jobSeedlingsLabel->setObjectName("jobSeedlingsLabel");

        jobInfoLayout->addWidget(jobSeedlingsLabel);

        jobSeedlingsValueLabel = new QLabel(jobInfoGroupBox);
        jobSeedlingsValueLabel->setObjectName("jobSeedlingsValueLabel");

        jobInfoLayout->addWidget(jobSeedlingsValueLabel);

        horizontalSpacer_7 = new QSpacerItem(40, 1, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        jobInfoLayout->addItem(horizontalSpacer_7);


        verticalLayout_4->addWidget(jobInfoGroupBox);

        operationControlGroupBox = new QGroupBox(tab_data);
        operationControlGroupBox->setObjectName("operationControlGroupBox");
        operationControlLayout = new QVBoxLayout(operationControlGroupBox);
        operationControlLayout->setObjectName("operationControlLayout");
        seedingStatusLayout = new QHBoxLayout();
        seedingStatusLayout->setObjectName("seedingStatusLayout");
        seedingStatusLabel = new QLabel(operationControlGroupBox);
        seedingStatusLabel->setObjectName("seedingStatusLabel");

        seedingStatusLayout->addWidget(seedingStatusLabel);

        seedingStatusIndicator = new QLabel(operationControlGroupBox);
        seedingStatusIndicator->setObjectName("seedingStatusIndicator");

        seedingStatusLayout->addWidget(seedingStatusIndicator);

        horizontalSpacer = new QSpacerItem(40, 1, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        seedingStatusLayout->addItem(horizontalSpacer);


        operationControlLayout->addLayout(seedingStatusLayout);

        fieldListWidget = new QListWidget(operationControlGroupBox);
        fieldListWidget->setObjectName("fieldListWidget");
        fieldListWidget->setMinimumSize(QSize(0, 50));

        operationControlLayout->addWidget(fieldListWidget);

        controlDataGridLayout = new QGridLayout();
        controlDataGridLayout->setObjectName("controlDataGridLayout");
        pathPlanningLabel = new QLabel(operationControlGroupBox);
        pathPlanningLabel->setObjectName("pathPlanningLabel");

        controlDataGridLayout->addWidget(pathPlanningLabel, 0, 0, 1, 1);

        currentSectionLabel = new QLabel(operationControlGroupBox);
        currentSectionLabel->setObjectName("currentSectionLabel");

        controlDataGridLayout->addWidget(currentSectionLabel, 0, 1, 1, 1);

        targetPointLabel = new QLabel(operationControlGroupBox);
        targetPointLabel->setObjectName("targetPointLabel");

        controlDataGridLayout->addWidget(targetPointLabel, 1, 1, 1, 1);

        triggerPointLabel = new QLabel(operationControlGroupBox);
        triggerPointLabel->setObjectName("triggerPointLabel");

        controlDataGridLayout->addWidget(triggerPointLabel, 2, 1, 1, 1);

        operationDataLabel = new QLabel(operationControlGroupBox);
        operationDataLabel->setObjectName("operationDataLabel");

        controlDataGridLayout->addWidget(operationDataLabel, 0, 2, 1, 1);

        seedlingsCountLabel = new QLabel(operationControlGroupBox);
        seedlingsCountLabel->setObjectName("seedlingsCountLabel");

        controlDataGridLayout->addWidget(seedlingsCountLabel, 1, 2, 1, 1);

        operationProgressLabel = new QLabel(operationControlGroupBox);
        operationProgressLabel->setObjectName("operationProgressLabel");

        controlDataGridLayout->addWidget(operationProgressLabel, 2, 2, 1, 1);


        operationControlLayout->addLayout(controlDataGridLayout);


        verticalLayout_4->addWidget(operationControlGroupBox);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        startButton = new QPushButton(tab_data);
        startButton->setObjectName("startButton");

        horizontalLayout_2->addWidget(startButton);

        stopButton = new QPushButton(tab_data);
        stopButton->setObjectName("stopButton");

        horizontalLayout_2->addWidget(stopButton);

        configButton = new QPushButton(tab_data);
        configButton->setObjectName("configButton");

        horizontalLayout_2->addWidget(configButton);


        verticalLayout_4->addLayout(horizontalLayout_2);

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

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\351\253\230\347\262\276\345\272\246\345\206\234\344\270\232\350\207\252\345\212\250\345\257\274\350\210\252\344\270\216\344\275\234\344\270\232\346\216\247\345\210\266\347\263\273\347\273\237", nullptr));
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
        plcDisconnectButton->setText(QCoreApplication::translate("MainWindow", "\346\265\213\350\257\225", nullptr));
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

        imuConfigLabel->setText(QCoreApplication::translate("MainWindow", "IMU\346\234\211\346\225\210\346\225\260\346\215\256\350\256\241\346\225\260: 0", nullptr));
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

        gpsConfigLabel->setText(QCoreApplication::translate("MainWindow", "GPS\346\234\211\346\225\210\346\225\260\346\215\256\350\256\241\346\225\260: 0", nullptr));
        plcSettingsGroupBox->setTitle(QCoreApplication::translate("MainWindow", "PLC\345\217\202\346\225\260\350\256\276\347\275\256", nullptr));
        plcPortLabel->setText(QCoreApplication::translate("MainWindow", "\345\234\260\345\235\200:", nullptr));
        plcIpLineEdit->setText(QCoreApplication::translate("MainWindow", "127.0.0.1", nullptr));
        plcBaudLabel->setText(QCoreApplication::translate("MainWindow", "\347\253\257\345\217\243:", nullptr));
        plcPortLineEdit->setText(QCoreApplication::translate("MainWindow", "502", nullptr));
        plcConfigLabel->setText(QCoreApplication::translate("MainWindow", "PLC\345\217\221\351\200\201\350\256\241\346\225\260: 0, \346\216\245\346\224\266\350\256\241\346\225\260: 0", nullptr));
        plcGroupBox->setTitle(QCoreApplication::translate("MainWindow", "PLC\346\225\260\346\215\256", nullptr));
        plcInputGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\350\257\273\345\217\226\346\225\260\346\215\256", nullptr));
        plcInputDataLabel->setText(QCoreApplication::translate("MainWindow", "\346\227\240\346\225\260\346\215\256", nullptr));
        plcOutputGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\346\214\207\344\273\244", nullptr));
        plcCommandTextEdit->setPlainText(QString());
        plcSendButton->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        coordinateSystemGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\345\235\220\346\240\207\347\263\273\346\230\276\347\244\272", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\346\222\255\347\247\215\345\214\272\345\237\237", nullptr));
        areaLengthLabel->setText(QCoreApplication::translate("MainWindow", "\345\214\272\345\237\237\351\225\277\345\272\246 (m):", nullptr));
        areaWidthLabel->setText(QCoreApplication::translate("MainWindow", "\345\214\272\345\237\237\345\256\275\345\272\246 (m):", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\345\236\204\347\237\251 (m):", nullptr));
        fieldLengthLabel->setText(QCoreApplication::translate("MainWindow", "\345\260\217\345\214\272\351\225\277\345\272\246 (m):", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\350\277\207\351\201\223\345\256\275\345\272\246 (m):", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\345\260\217\345\214\272\345\206\205\345\236\204\346\225\260:", nullptr));
        seedsPerBlockLabel->setText(QCoreApplication::translate("MainWindow", "\345\260\217\345\214\272\345\206\205\346\222\255\347\247\215\346\225\260:", nullptr));
        triggerDistanceLabel->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\350\247\246\345\217\221\350\267\235\347\246\273 (m):", nullptr));
        stopTriggerDistanceLabel->setText(QCoreApplication::translate("MainWindow", "\344\270\255\346\226\255\350\247\246\345\217\221\350\267\235\347\246\273 (m):", nullptr));
        pathPlanningGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\350\267\257\345\276\204\350\247\204\345\210\222\345\217\202\346\225\260", nullptr));
        oPointLatitudeLabel->setText(QCoreApplication::translate("MainWindow", "O\347\202\271\347\272\254\345\272\246:", nullptr));
        oPointLongitudeLabel->setText(QCoreApplication::translate("MainWindow", "O\347\202\271\347\273\217\345\272\246:", nullptr));
        aPointLatitudeLabel->setText(QCoreApplication::translate("MainWindow", "A\347\202\271\347\272\254\345\272\246:", nullptr));
        aPointLongitudeLabel->setText(QCoreApplication::translate("MainWindow", "A\347\202\271\347\273\217\345\272\246:", nullptr));
        speedPulseFactorLabel->setText(QCoreApplication::translate("MainWindow", "\351\200\237\345\272\246-\350\204\211\345\206\262\347\263\273\346\225\260:", nullptr));
        speedFrequencyFactorLabel->setText(QCoreApplication::translate("MainWindow", "\351\200\237\345\272\246-\351\242\221\347\216\207\347\263\273\346\225\260:", nullptr));
        generateConfigButton->setText(QCoreApplication::translate("MainWindow", "\347\224\237\346\210\220", nullptr));
        clearConfigButton->setText(QCoreApplication::translate("MainWindow", "\346\270\205\351\233\266", nullptr));
        loadConfigButton->setText(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256\344\270\272\345\216\237\347\202\271", nullptr));
        saveConfigButton->setText(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256\344\270\272A\347\202\271", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_settings), QCoreApplication::translate("MainWindow", "\351\205\215\347\275\256\347\252\227\345\217\243", nullptr));
        databaseGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\345\272\223\350\277\236\346\216\245", nullptr));
        databaseStatusLabel->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\345\272\223\347\212\266\346\200\201: \346\234\252\350\277\236\346\216\245", nullptr));
        connectDatabaseButton->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\346\225\260\346\215\256\345\272\223", nullptr));
        jobInfoGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\344\275\234\344\270\232\344\277\241\346\201\257", nullptr));
        jobIdLabel->setText(QCoreApplication::translate("MainWindow", "\344\275\234\344\270\232ID:", nullptr));
        jobIdValueLabel->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        jobDateLabel->setText(QCoreApplication::translate("MainWindow", "\346\227\245\346\234\237:", nullptr));
        jobDateValueLabel->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        jobAreaLabel->setText(QCoreApplication::translate("MainWindow", "\351\235\242\347\247\257:", nullptr));
        jobAreaValueLabel->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        jobSeedlingsLabel->setText(QCoreApplication::translate("MainWindow", "\346\222\255\347\247\215\346\225\260:", nullptr));
        jobSeedlingsValueLabel->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        operationControlGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\344\275\234\344\270\232\346\216\247\345\210\266", nullptr));
        seedingStatusLabel->setText(QCoreApplication::translate("MainWindow", "\346\222\255\347\247\215\347\212\266\346\200\201:", nullptr));
        seedingStatusIndicator->setText(QCoreApplication::translate("MainWindow", "\360\237\224\264 \346\234\252\346\222\255\347\247\215", nullptr));
        pathPlanningLabel->setText(QCoreApplication::translate("MainWindow", "\350\267\257\345\276\204\350\247\204\345\210\222:", nullptr));
        currentSectionLabel->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\345\214\272\351\227\264: 0", nullptr));
        targetPointLabel->setText(QCoreApplication::translate("MainWindow", "\347\233\256\346\240\207\347\202\271: (0.00, 0.00)", nullptr));
        triggerPointLabel->setText(QCoreApplication::translate("MainWindow", "\350\247\246\345\217\221\347\202\271: (0.00, 0.00)", nullptr));
        operationDataLabel->setText(QCoreApplication::translate("MainWindow", "\344\275\234\344\270\232\346\225\260\346\215\256:", nullptr));
        seedlingsCountLabel->setText(QCoreApplication::translate("MainWindow", "Seedlings: 0", nullptr));
        operationProgressLabel->setText(QCoreApplication::translate("MainWindow", "\350\277\233\345\272\246: 0%", nullptr));
        startButton->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213", nullptr));
        stopButton->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242", nullptr));
        configButton->setText(QCoreApplication::translate("MainWindow", "\351\205\215\347\275\256", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_data), QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\347\252\227\345\217\243", nullptr));
        timeAndSensorGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\346\246\202\350\246\201\344\277\241\346\201\257", nullptr));
        timeLabel->setText(QCoreApplication::translate("MainWindow", "\346\234\254\345\234\260\346\227\266\351\227\264:", nullptr));
        localTimeLabel->setText(QCoreApplication::translate("MainWindow", "2024-01-01 00:00:00", nullptr));
        gpsTimeLabel->setText(QCoreApplication::translate("MainWindow", "\345\215\253\346\230\237\346\227\266\351\227\264:", nullptr));
        gpsTimeValueLabel->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        imuAccelerationLabel->setText(QCoreApplication::translate("MainWindow", "\345\212\240\351\200\237\345\272\246: ", nullptr));
        gpsPositionLabel->setText(QCoreApplication::translate("MainWindow", "\347\273\217\347\272\254\345\272\246/\351\253\230\345\272\246: (0.000000, 0.000000, 0.00 m)", nullptr));
        imuDthetaLabel->setText(QCoreApplication::translate("MainWindow", "\350\247\222\351\200\237\345\272\246\345\242\236\351\207\217: ", nullptr));
        speedLabel->setText(QCoreApplication::translate("MainWindow", "\351\200\237\345\272\246: 0.00 m/s", nullptr));
        imuOutputMagneticHeadingLabel->setText(QCoreApplication::translate("MainWindow", "IMU\347\243\201\350\210\252\345\220\221: ", nullptr));
        magneticHeadingLabel->setText(QCoreApplication::translate("MainWindow", "\347\243\201\346\226\271\344\275\215\350\247\222: 0.00 \302\260", nullptr));
        imuAngleDisplayLabel->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\350\247\222\345\272\246: (0.00, 0.00, 0.00) \302\260", nullptr));
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
