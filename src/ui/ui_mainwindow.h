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
    QWidget *mapWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *mapTypeLabel;
    QComboBox *mapTypeComboBox;
    QLabel *dataSourceLabel;
    QComboBox *dataSourceComboBox;
    QPushButton *calibrateDataButton;
    QHBoxLayout *timeLayout;
    QLabel *timeLabel;
    QLabel *localTimeLabel;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *sensorDataGroupBox;
    QGridLayout *sensorDataGridLayout;
    QLabel *imuDataLabel;
    QLabel *imuTimeLabel;
    QLabel *imuAccLabel;
    QLabel *imuGyroLabel;
    QLabel *imuAngleLabel;
    QLabel *imuMagneticHeadingLabel;
    QLabel *gpsDataLabel;
    QLabel *gpsTimeLabel;
    QLabel *gpsPositionLabel;
    QLabel *gpsAltSpeedLabel;
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
    QHBoxLayout *serialOutputLayout;
    QGroupBox *imuOutputGroupBox;
    QGridLayout *imuOutputLayout;
    QLabel *imuTimeLabel1;
    QLabel *imuTimeValueLabel;
    QLabel *imuDtLabel;
    QLabel *imuDtValueLabel;
    QLabel *imuDthetaLabel;
    QLabel *imuDthetaValueLabel;
    QLabel *imuDvelLabel;
    QLabel *imuDvelValueLabel;
    QLabel *imuOdovelLabel;
    QLabel *imuOdovelValueLabel;
    QLabel *imuMagneticFieldLabel;
    QLabel *imuMagneticFieldValueLabel;
    QLabel *imuTemperatureLabel;
    QLabel *imuTemperatureValueLabel;
    QLabel *imuOutputMagneticHeadingLabel;
    QLabel *imuOutputMagneticHeadingValueLabel;
    QLabel *imuTrueHeadingLabel;
    QLabel *imuTrueHeadingValueLabel;
    QGroupBox *gpsOutputGroupBox;
    QGridLayout *gpsOutputLayout;
    QLabel *gnssTimeLabel;
    QLabel *gnssTimeValueLabel;
    QLabel *gnssPositionLabel;
    QLabel *gnssPositionValueLabel;
    QLabel *gnssVelocityLabel;
    QLabel *gnssVelocityValueLabel;
    QLabel *gnssStdLabel;
    QLabel *gnssStdValueLabel;
    QLabel *gnssValidLabel;
    QLabel *gnssValidValueLabel;
    QLabel *gnssQualityLabel;
    QLabel *gnssQualityValueLabel;
    QLabel *gnssFixModeLabel;
    QLabel *gnssFixModeValueLabel;
    QLabel *gnssHdopLabel;
    QLabel *gnssHdopValueLabel;
    QLabel *gnssVdopLabel;
    QLabel *gnssVdopValueLabel;
    QLabel *gnssPdopLabel;
    QLabel *gnssPdopValueLabel;
    QLabel *gnssVisibleSvLabel;
    QLabel *gnssVisibleSvValueLabel;
    QLabel *gnssUsedSvLabel;
    QLabel *gnssUsedSvValueLabel;
    QLabel *gnssGroundSpeedLabel;
    QLabel *gnssGroundSpeedValueLabel;
    QLabel *gnssTrueHeadingLabel;
    QLabel *gnssTrueHeadingValueLabel;
    QLabel *gnssMagneticHeadingLabel;
    QLabel *gnssMagneticHeadingValueLabel;
    QLabel *gnssSigmaLatLabel;
    QLabel *gnssSigmaLatValueLabel;
    QLabel *gnssSigmaLonLabel;
    QLabel *gnssSigmaLonValueLabel;
    QLabel *gnssSigmaAltLabel;
    QLabel *gnssSigmaAltValueLabel;
    QLabel *gnssSigmaRangeLabel;
    QLabel *gnssSigmaRangeValueLabel;
    QGroupBox *logDisplayGroupBox;
    QVBoxLayout *logDisplayLayout;
    QTextEdit *logDisplayTextEdit;
    QHBoxLayout *logLevelLayout;
    QLabel *logLevelLabel;
    QComboBox *logLevelComboBox;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *verticalSpacer;
    QWidget *tab_data;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label;
    QDoubleSpinBox *rowSpacingSpinBox;
    QLabel *label_2;
    QDoubleSpinBox *headlandWidthSpinBox;
    QLabel *label_3;
    QSpinBox *rowsPerBlockSpinBox;
    QGroupBox *pathPlanningGroupBox;
    QFormLayout *pathPlanningFormLayout;
    QLabel *triggerDistanceLabel;
    QDoubleSpinBox *triggerDistanceSpinBox;
    QLabel *safetyMarginLabel;
    QDoubleSpinBox *safetyMarginSpinBox;
    QLabel *fieldLengthLabel;
    QDoubleSpinBox *fieldLengthSpinBox;
    QLabel *advanceDistanceLabel;
    QDoubleSpinBox *advanceDistanceSpinBox;
    QGroupBox *groupBox_2;
    QFormLayout *formLayout_2;
    QLabel *label_4;
    QDoubleSpinBox *delaySpinBox;
    QLabel *label_5;
    QDoubleSpinBox *hysteresisSpinBox;
    QHBoxLayout *configFileLayout;
    QPushButton *loadConfigButton;
    QPushButton *saveConfigButton;
    QGroupBox *databaseGroupBox;
    QVBoxLayout *databaseLayout;
    QLabel *databaseStatusLabel;
    QPushButton *connectDatabaseButton;
    QGroupBox *historyGroupBox;
    QVBoxLayout *historyLayout;
    QListWidget *historyListWidget;
    QGroupBox *jobInfoGroupBox;
    QFormLayout *jobInfoLayout;
    QLabel *jobIdLabel;
    QLabel *jobIdValueLabel;
    QLabel *jobDateLabel;
    QLabel *jobDateValueLabel;
    QLabel *jobAreaLabel;
    QLabel *jobAreaValueLabel;
    QLabel *jobSeedlingsLabel;
    QLabel *jobSeedlingsValueLabel;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1024, 1114);
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
        mapWidget = new QWidget(tab_navigation);
        mapWidget->setObjectName("mapWidget");
        mapWidget->setMinimumSize(QSize(0, 400));

        verticalLayout_2->addWidget(mapWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        mapTypeLabel = new QLabel(tab_navigation);
        mapTypeLabel->setObjectName("mapTypeLabel");

        horizontalLayout->addWidget(mapTypeLabel);

        mapTypeComboBox = new QComboBox(tab_navigation);
        mapTypeComboBox->setObjectName("mapTypeComboBox");

        horizontalLayout->addWidget(mapTypeComboBox);

        dataSourceLabel = new QLabel(tab_navigation);
        dataSourceLabel->setObjectName("dataSourceLabel");

        horizontalLayout->addWidget(dataSourceLabel);

        dataSourceComboBox = new QComboBox(tab_navigation);
        dataSourceComboBox->setObjectName("dataSourceComboBox");

        horizontalLayout->addWidget(dataSourceComboBox);

        calibrateDataButton = new QPushButton(tab_navigation);
        calibrateDataButton->setObjectName("calibrateDataButton");

        horizontalLayout->addWidget(calibrateDataButton);


        verticalLayout_2->addLayout(horizontalLayout);

        timeLayout = new QHBoxLayout();
        timeLayout->setObjectName("timeLayout");
        timeLabel = new QLabel(tab_navigation);
        timeLabel->setObjectName("timeLabel");

        timeLayout->addWidget(timeLabel);

        localTimeLabel = new QLabel(tab_navigation);
        localTimeLabel->setObjectName("localTimeLabel");

        timeLayout->addWidget(localTimeLabel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        timeLayout->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(timeLayout);

        sensorDataGroupBox = new QGroupBox(tab_navigation);
        sensorDataGroupBox->setObjectName("sensorDataGroupBox");
        sensorDataGridLayout = new QGridLayout(sensorDataGroupBox);
        sensorDataGridLayout->setObjectName("sensorDataGridLayout");
        imuDataLabel = new QLabel(sensorDataGroupBox);
        imuDataLabel->setObjectName("imuDataLabel");

        sensorDataGridLayout->addWidget(imuDataLabel, 0, 0, 1, 1);

        imuTimeLabel = new QLabel(sensorDataGroupBox);
        imuTimeLabel->setObjectName("imuTimeLabel");

        sensorDataGridLayout->addWidget(imuTimeLabel, 0, 1, 1, 1);

        imuAccLabel = new QLabel(sensorDataGroupBox);
        imuAccLabel->setObjectName("imuAccLabel");

        sensorDataGridLayout->addWidget(imuAccLabel, 1, 1, 1, 1);

        imuGyroLabel = new QLabel(sensorDataGroupBox);
        imuGyroLabel->setObjectName("imuGyroLabel");

        sensorDataGridLayout->addWidget(imuGyroLabel, 2, 1, 1, 1);

        imuAngleLabel = new QLabel(sensorDataGroupBox);
        imuAngleLabel->setObjectName("imuAngleLabel");

        sensorDataGridLayout->addWidget(imuAngleLabel, 3, 1, 1, 1);

        imuMagneticHeadingLabel = new QLabel(sensorDataGroupBox);
        imuMagneticHeadingLabel->setObjectName("imuMagneticHeadingLabel");

        sensorDataGridLayout->addWidget(imuMagneticHeadingLabel, 4, 1, 1, 1);

        gpsDataLabel = new QLabel(sensorDataGroupBox);
        gpsDataLabel->setObjectName("gpsDataLabel");

        sensorDataGridLayout->addWidget(gpsDataLabel, 0, 2, 1, 1);

        gpsTimeLabel = new QLabel(sensorDataGroupBox);
        gpsTimeLabel->setObjectName("gpsTimeLabel");

        sensorDataGridLayout->addWidget(gpsTimeLabel, 1, 2, 1, 1);

        gpsPositionLabel = new QLabel(sensorDataGroupBox);
        gpsPositionLabel->setObjectName("gpsPositionLabel");

        sensorDataGridLayout->addWidget(gpsPositionLabel, 2, 2, 1, 1);

        gpsAltSpeedLabel = new QLabel(sensorDataGroupBox);
        gpsAltSpeedLabel->setObjectName("gpsAltSpeedLabel");

        sensorDataGridLayout->addWidget(gpsAltSpeedLabel, 3, 2, 1, 1);


        verticalLayout_2->addWidget(sensorDataGroupBox);

        operationControlGroupBox = new QGroupBox(tab_navigation);
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

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        seedingStatusLayout->addItem(horizontalSpacer);


        operationControlLayout->addLayout(seedingStatusLayout);

        fieldListWidget = new QListWidget(operationControlGroupBox);
        fieldListWidget->setObjectName("fieldListWidget");
        fieldListWidget->setMinimumSize(QSize(0, 100));

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


        verticalLayout_2->addWidget(operationControlGroupBox);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        startButton = new QPushButton(tab_navigation);
        startButton->setObjectName("startButton");

        horizontalLayout_2->addWidget(startButton);

        stopButton = new QPushButton(tab_navigation);
        stopButton->setObjectName("stopButton");

        horizontalLayout_2->addWidget(stopButton);

        configButton = new QPushButton(tab_navigation);
        configButton->setObjectName("configButton");

        horizontalLayout_2->addWidget(configButton);


        verticalLayout_2->addLayout(horizontalLayout_2);

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


        verticalLayout_3->addLayout(sensorSettingsLayout);

        serialOutputLayout = new QHBoxLayout();
        serialOutputLayout->setObjectName("serialOutputLayout");
        imuOutputGroupBox = new QGroupBox(tab_settings);
        imuOutputGroupBox->setObjectName("imuOutputGroupBox");
        imuOutputLayout = new QGridLayout(imuOutputGroupBox);
        imuOutputLayout->setObjectName("imuOutputLayout");
        imuTimeLabel1 = new QLabel(imuOutputGroupBox);
        imuTimeLabel1->setObjectName("imuTimeLabel1");

        imuOutputLayout->addWidget(imuTimeLabel1, 0, 0, 1, 1);

        imuTimeValueLabel = new QLabel(imuOutputGroupBox);
        imuTimeValueLabel->setObjectName("imuTimeValueLabel");

        imuOutputLayout->addWidget(imuTimeValueLabel, 0, 1, 1, 1);

        imuDtLabel = new QLabel(imuOutputGroupBox);
        imuDtLabel->setObjectName("imuDtLabel");

        imuOutputLayout->addWidget(imuDtLabel, 1, 0, 1, 1);

        imuDtValueLabel = new QLabel(imuOutputGroupBox);
        imuDtValueLabel->setObjectName("imuDtValueLabel");

        imuOutputLayout->addWidget(imuDtValueLabel, 1, 1, 1, 1);

        imuDthetaLabel = new QLabel(imuOutputGroupBox);
        imuDthetaLabel->setObjectName("imuDthetaLabel");

        imuOutputLayout->addWidget(imuDthetaLabel, 2, 0, 1, 1);

        imuDthetaValueLabel = new QLabel(imuOutputGroupBox);
        imuDthetaValueLabel->setObjectName("imuDthetaValueLabel");

        imuOutputLayout->addWidget(imuDthetaValueLabel, 2, 1, 1, 1);

        imuDvelLabel = new QLabel(imuOutputGroupBox);
        imuDvelLabel->setObjectName("imuDvelLabel");

        imuOutputLayout->addWidget(imuDvelLabel, 3, 0, 1, 1);

        imuDvelValueLabel = new QLabel(imuOutputGroupBox);
        imuDvelValueLabel->setObjectName("imuDvelValueLabel");

        imuOutputLayout->addWidget(imuDvelValueLabel, 3, 1, 1, 1);

        imuOdovelLabel = new QLabel(imuOutputGroupBox);
        imuOdovelLabel->setObjectName("imuOdovelLabel");

        imuOutputLayout->addWidget(imuOdovelLabel, 4, 0, 1, 1);

        imuOdovelValueLabel = new QLabel(imuOutputGroupBox);
        imuOdovelValueLabel->setObjectName("imuOdovelValueLabel");

        imuOutputLayout->addWidget(imuOdovelValueLabel, 4, 1, 1, 1);

        imuMagneticFieldLabel = new QLabel(imuOutputGroupBox);
        imuMagneticFieldLabel->setObjectName("imuMagneticFieldLabel");

        imuOutputLayout->addWidget(imuMagneticFieldLabel, 5, 0, 1, 1);

        imuMagneticFieldValueLabel = new QLabel(imuOutputGroupBox);
        imuMagneticFieldValueLabel->setObjectName("imuMagneticFieldValueLabel");

        imuOutputLayout->addWidget(imuMagneticFieldValueLabel, 5, 1, 1, 1);

        imuTemperatureLabel = new QLabel(imuOutputGroupBox);
        imuTemperatureLabel->setObjectName("imuTemperatureLabel");

        imuOutputLayout->addWidget(imuTemperatureLabel, 6, 0, 1, 1);

        imuTemperatureValueLabel = new QLabel(imuOutputGroupBox);
        imuTemperatureValueLabel->setObjectName("imuTemperatureValueLabel");

        imuOutputLayout->addWidget(imuTemperatureValueLabel, 6, 1, 1, 1);

        imuOutputMagneticHeadingLabel = new QLabel(imuOutputGroupBox);
        imuOutputMagneticHeadingLabel->setObjectName("imuOutputMagneticHeadingLabel");

        imuOutputLayout->addWidget(imuOutputMagneticHeadingLabel, 7, 0, 1, 1);

        imuOutputMagneticHeadingValueLabel = new QLabel(imuOutputGroupBox);
        imuOutputMagneticHeadingValueLabel->setObjectName("imuOutputMagneticHeadingValueLabel");

        imuOutputLayout->addWidget(imuOutputMagneticHeadingValueLabel, 7, 1, 1, 1);

        imuTrueHeadingLabel = new QLabel(imuOutputGroupBox);
        imuTrueHeadingLabel->setObjectName("imuTrueHeadingLabel");

        imuOutputLayout->addWidget(imuTrueHeadingLabel, 8, 0, 1, 1);

        imuTrueHeadingValueLabel = new QLabel(imuOutputGroupBox);
        imuTrueHeadingValueLabel->setObjectName("imuTrueHeadingValueLabel");

        imuOutputLayout->addWidget(imuTrueHeadingValueLabel, 8, 1, 1, 1);


        serialOutputLayout->addWidget(imuOutputGroupBox);

        gpsOutputGroupBox = new QGroupBox(tab_settings);
        gpsOutputGroupBox->setObjectName("gpsOutputGroupBox");
        gpsOutputLayout = new QGridLayout(gpsOutputGroupBox);
        gpsOutputLayout->setObjectName("gpsOutputLayout");
        gnssTimeLabel = new QLabel(gpsOutputGroupBox);
        gnssTimeLabel->setObjectName("gnssTimeLabel");

        gpsOutputLayout->addWidget(gnssTimeLabel, 0, 0, 1, 1);

        gnssTimeValueLabel = new QLabel(gpsOutputGroupBox);
        gnssTimeValueLabel->setObjectName("gnssTimeValueLabel");

        gpsOutputLayout->addWidget(gnssTimeValueLabel, 0, 1, 1, 1);

        gnssPositionLabel = new QLabel(gpsOutputGroupBox);
        gnssPositionLabel->setObjectName("gnssPositionLabel");

        gpsOutputLayout->addWidget(gnssPositionLabel, 1, 0, 1, 1);

        gnssPositionValueLabel = new QLabel(gpsOutputGroupBox);
        gnssPositionValueLabel->setObjectName("gnssPositionValueLabel");

        gpsOutputLayout->addWidget(gnssPositionValueLabel, 1, 1, 1, 1);

        gnssVelocityLabel = new QLabel(gpsOutputGroupBox);
        gnssVelocityLabel->setObjectName("gnssVelocityLabel");

        gpsOutputLayout->addWidget(gnssVelocityLabel, 2, 0, 1, 1);

        gnssVelocityValueLabel = new QLabel(gpsOutputGroupBox);
        gnssVelocityValueLabel->setObjectName("gnssVelocityValueLabel");

        gpsOutputLayout->addWidget(gnssVelocityValueLabel, 2, 1, 1, 1);

        gnssStdLabel = new QLabel(gpsOutputGroupBox);
        gnssStdLabel->setObjectName("gnssStdLabel");

        gpsOutputLayout->addWidget(gnssStdLabel, 3, 0, 1, 1);

        gnssStdValueLabel = new QLabel(gpsOutputGroupBox);
        gnssStdValueLabel->setObjectName("gnssStdValueLabel");

        gpsOutputLayout->addWidget(gnssStdValueLabel, 3, 1, 1, 1);

        gnssValidLabel = new QLabel(gpsOutputGroupBox);
        gnssValidLabel->setObjectName("gnssValidLabel");

        gpsOutputLayout->addWidget(gnssValidLabel, 4, 0, 1, 1);

        gnssValidValueLabel = new QLabel(gpsOutputGroupBox);
        gnssValidValueLabel->setObjectName("gnssValidValueLabel");

        gpsOutputLayout->addWidget(gnssValidValueLabel, 4, 1, 1, 1);

        gnssQualityLabel = new QLabel(gpsOutputGroupBox);
        gnssQualityLabel->setObjectName("gnssQualityLabel");

        gpsOutputLayout->addWidget(gnssQualityLabel, 5, 0, 1, 1);

        gnssQualityValueLabel = new QLabel(gpsOutputGroupBox);
        gnssQualityValueLabel->setObjectName("gnssQualityValueLabel");

        gpsOutputLayout->addWidget(gnssQualityValueLabel, 5, 1, 1, 1);

        gnssFixModeLabel = new QLabel(gpsOutputGroupBox);
        gnssFixModeLabel->setObjectName("gnssFixModeLabel");

        gpsOutputLayout->addWidget(gnssFixModeLabel, 6, 0, 1, 1);

        gnssFixModeValueLabel = new QLabel(gpsOutputGroupBox);
        gnssFixModeValueLabel->setObjectName("gnssFixModeValueLabel");

        gpsOutputLayout->addWidget(gnssFixModeValueLabel, 6, 1, 1, 1);

        gnssHdopLabel = new QLabel(gpsOutputGroupBox);
        gnssHdopLabel->setObjectName("gnssHdopLabel");

        gpsOutputLayout->addWidget(gnssHdopLabel, 7, 0, 1, 1);

        gnssHdopValueLabel = new QLabel(gpsOutputGroupBox);
        gnssHdopValueLabel->setObjectName("gnssHdopValueLabel");

        gpsOutputLayout->addWidget(gnssHdopValueLabel, 7, 1, 1, 1);

        gnssVdopLabel = new QLabel(gpsOutputGroupBox);
        gnssVdopLabel->setObjectName("gnssVdopLabel");

        gpsOutputLayout->addWidget(gnssVdopLabel, 8, 0, 1, 1);

        gnssVdopValueLabel = new QLabel(gpsOutputGroupBox);
        gnssVdopValueLabel->setObjectName("gnssVdopValueLabel");

        gpsOutputLayout->addWidget(gnssVdopValueLabel, 8, 1, 1, 1);

        gnssPdopLabel = new QLabel(gpsOutputGroupBox);
        gnssPdopLabel->setObjectName("gnssPdopLabel");

        gpsOutputLayout->addWidget(gnssPdopLabel, 9, 0, 1, 1);

        gnssPdopValueLabel = new QLabel(gpsOutputGroupBox);
        gnssPdopValueLabel->setObjectName("gnssPdopValueLabel");

        gpsOutputLayout->addWidget(gnssPdopValueLabel, 9, 1, 1, 1);

        gnssVisibleSvLabel = new QLabel(gpsOutputGroupBox);
        gnssVisibleSvLabel->setObjectName("gnssVisibleSvLabel");

        gpsOutputLayout->addWidget(gnssVisibleSvLabel, 10, 0, 1, 1);

        gnssVisibleSvValueLabel = new QLabel(gpsOutputGroupBox);
        gnssVisibleSvValueLabel->setObjectName("gnssVisibleSvValueLabel");

        gpsOutputLayout->addWidget(gnssVisibleSvValueLabel, 10, 1, 1, 1);

        gnssUsedSvLabel = new QLabel(gpsOutputGroupBox);
        gnssUsedSvLabel->setObjectName("gnssUsedSvLabel");

        gpsOutputLayout->addWidget(gnssUsedSvLabel, 11, 0, 1, 1);

        gnssUsedSvValueLabel = new QLabel(gpsOutputGroupBox);
        gnssUsedSvValueLabel->setObjectName("gnssUsedSvValueLabel");

        gpsOutputLayout->addWidget(gnssUsedSvValueLabel, 11, 1, 1, 1);

        gnssGroundSpeedLabel = new QLabel(gpsOutputGroupBox);
        gnssGroundSpeedLabel->setObjectName("gnssGroundSpeedLabel");

        gpsOutputLayout->addWidget(gnssGroundSpeedLabel, 12, 0, 1, 1);

        gnssGroundSpeedValueLabel = new QLabel(gpsOutputGroupBox);
        gnssGroundSpeedValueLabel->setObjectName("gnssGroundSpeedValueLabel");

        gpsOutputLayout->addWidget(gnssGroundSpeedValueLabel, 12, 1, 1, 1);

        gnssTrueHeadingLabel = new QLabel(gpsOutputGroupBox);
        gnssTrueHeadingLabel->setObjectName("gnssTrueHeadingLabel");

        gpsOutputLayout->addWidget(gnssTrueHeadingLabel, 13, 0, 1, 1);

        gnssTrueHeadingValueLabel = new QLabel(gpsOutputGroupBox);
        gnssTrueHeadingValueLabel->setObjectName("gnssTrueHeadingValueLabel");

        gpsOutputLayout->addWidget(gnssTrueHeadingValueLabel, 13, 1, 1, 1);

        gnssMagneticHeadingLabel = new QLabel(gpsOutputGroupBox);
        gnssMagneticHeadingLabel->setObjectName("gnssMagneticHeadingLabel");

        gpsOutputLayout->addWidget(gnssMagneticHeadingLabel, 14, 0, 1, 1);

        gnssMagneticHeadingValueLabel = new QLabel(gpsOutputGroupBox);
        gnssMagneticHeadingValueLabel->setObjectName("gnssMagneticHeadingValueLabel");

        gpsOutputLayout->addWidget(gnssMagneticHeadingValueLabel, 14, 1, 1, 1);

        gnssSigmaLatLabel = new QLabel(gpsOutputGroupBox);
        gnssSigmaLatLabel->setObjectName("gnssSigmaLatLabel");

        gpsOutputLayout->addWidget(gnssSigmaLatLabel, 15, 0, 1, 1);

        gnssSigmaLatValueLabel = new QLabel(gpsOutputGroupBox);
        gnssSigmaLatValueLabel->setObjectName("gnssSigmaLatValueLabel");

        gpsOutputLayout->addWidget(gnssSigmaLatValueLabel, 15, 1, 1, 1);

        gnssSigmaLonLabel = new QLabel(gpsOutputGroupBox);
        gnssSigmaLonLabel->setObjectName("gnssSigmaLonLabel");

        gpsOutputLayout->addWidget(gnssSigmaLonLabel, 16, 0, 1, 1);

        gnssSigmaLonValueLabel = new QLabel(gpsOutputGroupBox);
        gnssSigmaLonValueLabel->setObjectName("gnssSigmaLonValueLabel");

        gpsOutputLayout->addWidget(gnssSigmaLonValueLabel, 16, 1, 1, 1);

        gnssSigmaAltLabel = new QLabel(gpsOutputGroupBox);
        gnssSigmaAltLabel->setObjectName("gnssSigmaAltLabel");

        gpsOutputLayout->addWidget(gnssSigmaAltLabel, 17, 0, 1, 1);

        gnssSigmaAltValueLabel = new QLabel(gpsOutputGroupBox);
        gnssSigmaAltValueLabel->setObjectName("gnssSigmaAltValueLabel");

        gpsOutputLayout->addWidget(gnssSigmaAltValueLabel, 17, 1, 1, 1);

        gnssSigmaRangeLabel = new QLabel(gpsOutputGroupBox);
        gnssSigmaRangeLabel->setObjectName("gnssSigmaRangeLabel");

        gpsOutputLayout->addWidget(gnssSigmaRangeLabel, 18, 0, 1, 1);

        gnssSigmaRangeValueLabel = new QLabel(gpsOutputGroupBox);
        gnssSigmaRangeValueLabel->setObjectName("gnssSigmaRangeValueLabel");

        gpsOutputLayout->addWidget(gnssSigmaRangeValueLabel, 18, 1, 1, 1);


        serialOutputLayout->addWidget(gpsOutputGroupBox);


        verticalLayout_3->addLayout(serialOutputLayout);

        logDisplayGroupBox = new QGroupBox(tab_settings);
        logDisplayGroupBox->setObjectName("logDisplayGroupBox");
        logDisplayLayout = new QVBoxLayout(logDisplayGroupBox);
        logDisplayLayout->setObjectName("logDisplayLayout");
        logDisplayTextEdit = new QTextEdit(logDisplayGroupBox);
        logDisplayTextEdit->setObjectName("logDisplayTextEdit");
        logDisplayTextEdit->setMinimumSize(QSize(0, 300));
        logDisplayTextEdit->setReadOnly(true);

        logDisplayLayout->addWidget(logDisplayTextEdit);


        verticalLayout_3->addWidget(logDisplayGroupBox);

        logLevelLayout = new QHBoxLayout();
        logLevelLayout->setObjectName("logLevelLayout");
        logLevelLabel = new QLabel(tab_settings);
        logLevelLabel->setObjectName("logLevelLabel");

        logLevelLayout->addWidget(logLevelLabel);

        logLevelComboBox = new QComboBox(tab_settings);
        logLevelComboBox->addItem(QString());
        logLevelComboBox->addItem(QString());
        logLevelComboBox->addItem(QString());
        logLevelComboBox->addItem(QString());
        logLevelComboBox->setObjectName("logLevelComboBox");

        logLevelLayout->addWidget(logLevelComboBox);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        logLevelLayout->addItem(horizontalSpacer_3);


        verticalLayout_3->addLayout(logLevelLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        tabWidget->addTab(tab_settings, QString());
        tab_data = new QWidget();
        tab_data->setObjectName("tab_data");
        verticalLayout_4 = new QVBoxLayout(tab_data);
        verticalLayout_4->setObjectName("verticalLayout_4");
        groupBox = new QGroupBox(tab_data);
        groupBox->setObjectName("groupBox");
        formLayout = new QFormLayout(groupBox);
        formLayout->setObjectName("formLayout");
        label = new QLabel(groupBox);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, label);

        rowSpacingSpinBox = new QDoubleSpinBox(groupBox);
        rowSpacingSpinBox->setObjectName("rowSpacingSpinBox");
        rowSpacingSpinBox->setDecimals(1);
        rowSpacingSpinBox->setMinimum(0.100000000000000);
        rowSpacingSpinBox->setMaximum(2.000000000000000);
        rowSpacingSpinBox->setSingleStep(0.100000000000000);
        rowSpacingSpinBox->setValue(0.300000000000000);

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, rowSpacingSpinBox);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, label_2);

        headlandWidthSpinBox = new QDoubleSpinBox(groupBox);
        headlandWidthSpinBox->setObjectName("headlandWidthSpinBox");
        headlandWidthSpinBox->setDecimals(1);
        headlandWidthSpinBox->setMinimum(1.000000000000000);
        headlandWidthSpinBox->setMaximum(5.000000000000000);
        headlandWidthSpinBox->setSingleStep(0.100000000000000);
        headlandWidthSpinBox->setValue(1.000000000000000);

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, headlandWidthSpinBox);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, label_3);

        rowsPerBlockSpinBox = new QSpinBox(groupBox);
        rowsPerBlockSpinBox->setObjectName("rowsPerBlockSpinBox");
        rowsPerBlockSpinBox->setMinimum(1);
        rowsPerBlockSpinBox->setMaximum(50);
        rowsPerBlockSpinBox->setValue(10);

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, rowsPerBlockSpinBox);


        verticalLayout_4->addWidget(groupBox);

        pathPlanningGroupBox = new QGroupBox(tab_data);
        pathPlanningGroupBox->setObjectName("pathPlanningGroupBox");
        pathPlanningFormLayout = new QFormLayout(pathPlanningGroupBox);
        pathPlanningFormLayout->setObjectName("pathPlanningFormLayout");
        triggerDistanceLabel = new QLabel(pathPlanningGroupBox);
        triggerDistanceLabel->setObjectName("triggerDistanceLabel");

        pathPlanningFormLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, triggerDistanceLabel);

        triggerDistanceSpinBox = new QDoubleSpinBox(pathPlanningGroupBox);
        triggerDistanceSpinBox->setObjectName("triggerDistanceSpinBox");
        triggerDistanceSpinBox->setDecimals(1);
        triggerDistanceSpinBox->setMinimum(0.100000000000000);
        triggerDistanceSpinBox->setMaximum(5.000000000000000);
        triggerDistanceSpinBox->setSingleStep(0.100000000000000);
        triggerDistanceSpinBox->setValue(1.000000000000000);

        pathPlanningFormLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, triggerDistanceSpinBox);

        safetyMarginLabel = new QLabel(pathPlanningGroupBox);
        safetyMarginLabel->setObjectName("safetyMarginLabel");

        pathPlanningFormLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, safetyMarginLabel);

        safetyMarginSpinBox = new QDoubleSpinBox(pathPlanningGroupBox);
        safetyMarginSpinBox->setObjectName("safetyMarginSpinBox");
        safetyMarginSpinBox->setDecimals(1);
        safetyMarginSpinBox->setMinimum(0.100000000000000);
        safetyMarginSpinBox->setMaximum(2.000000000000000);
        safetyMarginSpinBox->setSingleStep(0.100000000000000);
        safetyMarginSpinBox->setValue(0.500000000000000);

        pathPlanningFormLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, safetyMarginSpinBox);

        fieldLengthLabel = new QLabel(pathPlanningGroupBox);
        fieldLengthLabel->setObjectName("fieldLengthLabel");

        pathPlanningFormLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, fieldLengthLabel);

        fieldLengthSpinBox = new QDoubleSpinBox(pathPlanningGroupBox);
        fieldLengthSpinBox->setObjectName("fieldLengthSpinBox");
        fieldLengthSpinBox->setDecimals(0);
        fieldLengthSpinBox->setMinimum(10.000000000000000);
        fieldLengthSpinBox->setMaximum(1000.000000000000000);
        fieldLengthSpinBox->setSingleStep(10.000000000000000);
        fieldLengthSpinBox->setValue(100.000000000000000);

        pathPlanningFormLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, fieldLengthSpinBox);

        advanceDistanceLabel = new QLabel(pathPlanningGroupBox);
        advanceDistanceLabel->setObjectName("advanceDistanceLabel");

        pathPlanningFormLayout->setWidget(3, QFormLayout::ItemRole::LabelRole, advanceDistanceLabel);

        advanceDistanceSpinBox = new QDoubleSpinBox(pathPlanningGroupBox);
        advanceDistanceSpinBox->setObjectName("advanceDistanceSpinBox");
        advanceDistanceSpinBox->setDecimals(1);
        advanceDistanceSpinBox->setMinimum(0.000000000000000);
        advanceDistanceSpinBox->setMaximum(5.000000000000000);
        advanceDistanceSpinBox->setSingleStep(0.100000000000000);
        advanceDistanceSpinBox->setValue(0.500000000000000);

        pathPlanningFormLayout->setWidget(3, QFormLayout::ItemRole::FieldRole, advanceDistanceSpinBox);


        verticalLayout_4->addWidget(pathPlanningGroupBox);

        groupBox_2 = new QGroupBox(tab_data);
        groupBox_2->setObjectName("groupBox_2");
        formLayout_2 = new QFormLayout(groupBox_2);
        formLayout_2->setObjectName("formLayout_2");
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName("label_4");

        formLayout_2->setWidget(0, QFormLayout::ItemRole::LabelRole, label_4);

        delaySpinBox = new QDoubleSpinBox(groupBox_2);
        delaySpinBox->setObjectName("delaySpinBox");
        delaySpinBox->setDecimals(3);
        delaySpinBox->setMinimum(0.000000000000000);
        delaySpinBox->setMaximum(1.000000000000000);
        delaySpinBox->setSingleStep(0.010000000000000);
        delaySpinBox->setValue(0.300000000000000);

        formLayout_2->setWidget(0, QFormLayout::ItemRole::FieldRole, delaySpinBox);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName("label_5");

        formLayout_2->setWidget(1, QFormLayout::ItemRole::LabelRole, label_5);

        hysteresisSpinBox = new QDoubleSpinBox(groupBox_2);
        hysteresisSpinBox->setObjectName("hysteresisSpinBox");
        hysteresisSpinBox->setDecimals(2);
        hysteresisSpinBox->setMinimum(0.000000000000000);
        hysteresisSpinBox->setMaximum(0.100000000000000);
        hysteresisSpinBox->setSingleStep(0.010000000000000);
        hysteresisSpinBox->setValue(0.020000000000000);

        formLayout_2->setWidget(1, QFormLayout::ItemRole::FieldRole, hysteresisSpinBox);


        verticalLayout_4->addWidget(groupBox_2);

        configFileLayout = new QHBoxLayout();
        configFileLayout->setObjectName("configFileLayout");
        loadConfigButton = new QPushButton(tab_data);
        loadConfigButton->setObjectName("loadConfigButton");

        configFileLayout->addWidget(loadConfigButton);

        saveConfigButton = new QPushButton(tab_data);
        saveConfigButton->setObjectName("saveConfigButton");

        configFileLayout->addWidget(saveConfigButton);


        verticalLayout_4->addLayout(configFileLayout);

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

        historyGroupBox = new QGroupBox(tab_data);
        historyGroupBox->setObjectName("historyGroupBox");
        historyLayout = new QVBoxLayout(historyGroupBox);
        historyLayout->setObjectName("historyLayout");
        historyListWidget = new QListWidget(historyGroupBox);
        historyListWidget->setObjectName("historyListWidget");
        historyListWidget->setMinimumSize(QSize(0, 150));

        historyLayout->addWidget(historyListWidget);

        jobInfoGroupBox = new QGroupBox(historyGroupBox);
        jobInfoGroupBox->setObjectName("jobInfoGroupBox");
        jobInfoLayout = new QFormLayout(jobInfoGroupBox);
        jobInfoLayout->setObjectName("jobInfoLayout");
        jobIdLabel = new QLabel(jobInfoGroupBox);
        jobIdLabel->setObjectName("jobIdLabel");

        jobInfoLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, jobIdLabel);

        jobIdValueLabel = new QLabel(jobInfoGroupBox);
        jobIdValueLabel->setObjectName("jobIdValueLabel");

        jobInfoLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, jobIdValueLabel);

        jobDateLabel = new QLabel(jobInfoGroupBox);
        jobDateLabel->setObjectName("jobDateLabel");

        jobInfoLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, jobDateLabel);

        jobDateValueLabel = new QLabel(jobInfoGroupBox);
        jobDateValueLabel->setObjectName("jobDateValueLabel");

        jobInfoLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, jobDateValueLabel);

        jobAreaLabel = new QLabel(jobInfoGroupBox);
        jobAreaLabel->setObjectName("jobAreaLabel");

        jobInfoLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, jobAreaLabel);

        jobAreaValueLabel = new QLabel(jobInfoGroupBox);
        jobAreaValueLabel->setObjectName("jobAreaValueLabel");

        jobInfoLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, jobAreaValueLabel);

        jobSeedlingsLabel = new QLabel(jobInfoGroupBox);
        jobSeedlingsLabel->setObjectName("jobSeedlingsLabel");

        jobInfoLayout->setWidget(3, QFormLayout::ItemRole::LabelRole, jobSeedlingsLabel);

        jobSeedlingsValueLabel = new QLabel(jobInfoGroupBox);
        jobSeedlingsValueLabel->setObjectName("jobSeedlingsValueLabel");

        jobInfoLayout->setWidget(3, QFormLayout::ItemRole::FieldRole, jobSeedlingsValueLabel);


        historyLayout->addWidget(jobInfoGroupBox);


        verticalLayout_4->addWidget(historyGroupBox);

        tabWidget->addTab(tab_data, QString());

        verticalLayout->addWidget(tabWidget);

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
        mapTypeLabel->setText(QCoreApplication::translate("MainWindow", "\345\234\260\345\233\276\347\261\273\345\236\213:", nullptr));
        dataSourceLabel->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\346\272\220:", nullptr));
        calibrateDataButton->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\346\240\241\345\207\206", nullptr));
        timeLabel->setText(QCoreApplication::translate("MainWindow", "\346\234\254\345\234\260\346\227\266\351\227\264:", nullptr));
        localTimeLabel->setText(QCoreApplication::translate("MainWindow", "2024-01-01 00:00:00", nullptr));
        sensorDataGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\344\274\240\346\204\237\345\231\250\346\225\260\346\215\256", nullptr));
        imuDataLabel->setText(QCoreApplication::translate("MainWindow", "IMU\346\225\260\346\215\256:", nullptr));
        imuTimeLabel->setText(QCoreApplication::translate("MainWindow", "\346\227\266\351\227\264: ", nullptr));
        imuAccLabel->setText(QCoreApplication::translate("MainWindow", "\345\212\240\351\200\237\345\272\246: (0.00, 0.00, 0.00) m/s\302\262", nullptr));
        imuGyroLabel->setText(QCoreApplication::translate("MainWindow", "\350\247\222\351\200\237\345\272\246: (0.00, 0.00, 0.00) rad/s", nullptr));
        imuAngleLabel->setText(QCoreApplication::translate("MainWindow", "\350\247\222\345\272\246: (0.00, 0.00, 0.00) \302\260", nullptr));
        imuMagneticHeadingLabel->setText(QCoreApplication::translate("MainWindow", "\347\243\201\346\226\271\344\275\215\350\247\222: 0.00 \302\260", nullptr));
        gpsDataLabel->setText(QCoreApplication::translate("MainWindow", "GPS\346\225\260\346\215\256:", nullptr));
        gpsTimeLabel->setText(QCoreApplication::translate("MainWindow", "\346\227\266\351\227\264: ", nullptr));
        gpsPositionLabel->setText(QCoreApplication::translate("MainWindow", "\347\273\217\347\272\254\345\272\246: (0.000000, 0.000000)", nullptr));
        gpsAltSpeedLabel->setText(QCoreApplication::translate("MainWindow", "\351\253\230\345\272\246: 0.00 m, \351\200\237\345\272\246: 0.00 m/s", nullptr));
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
        tabWidget->setTabText(tabWidget->indexOf(tab_navigation), QCoreApplication::translate("MainWindow", "\344\270\273\347\252\227\345\217\243", nullptr));
        deviceStatusGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\350\256\276\345\244\207\347\212\266\346\200\201", nullptr));
        imuStatusLabel->setText(QCoreApplication::translate("MainWindow", "IMU\347\212\266\346\200\201: \342\232\252 \346\234\252\350\277\236\346\216\245", nullptr));
        imuConnectButton->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\220\257", nullptr));
        imuDisconnectButton->setText(QCoreApplication::translate("MainWindow", "\345\205\263\351\227\255", nullptr));
        gpsStatusLabel->setText(QCoreApplication::translate("MainWindow", "GPS\347\212\266\346\200\201: \342\232\252 \346\234\252\350\277\236\346\216\245", nullptr));
        gpsConnectButton->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\220\257", nullptr));
        gpsDisconnectButton->setText(QCoreApplication::translate("MainWindow", "\345\205\263\351\227\255", nullptr));
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

        imuConfigLabel->setText(QCoreApplication::translate("MainWindow", "IMU\351\205\215\347\275\256: \346\234\252\350\257\273\345\217\226", nullptr));
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

        gpsConfigLabel->setText(QCoreApplication::translate("MainWindow", "GPS\351\205\215\347\275\256: \346\234\252\350\257\273\345\217\226", nullptr));
        imuOutputGroupBox->setTitle(QCoreApplication::translate("MainWindow", "IMU\346\225\260\346\215\256", nullptr));
        imuTimeLabel1->setText(QCoreApplication::translate("MainWindow", "\346\227\266\351\227\264: ", nullptr));
        imuTimeValueLabel->setText(QString());
        imuDtLabel->setText(QCoreApplication::translate("MainWindow", "\346\227\266\351\227\264\351\227\264\351\232\224: ", nullptr));
        imuDtValueLabel->setText(QString());
        imuDthetaLabel->setText(QCoreApplication::translate("MainWindow", "\350\247\222\351\200\237\345\272\246\345\242\236\351\207\217: ", nullptr));
        imuDthetaValueLabel->setText(QString());
        imuDvelLabel->setText(QCoreApplication::translate("MainWindow", "\351\200\237\345\272\246\345\242\236\351\207\217: ", nullptr));
        imuDvelValueLabel->setText(QString());
        imuOdovelLabel->setText(QCoreApplication::translate("MainWindow", "\351\207\214\347\250\213\350\256\241\351\200\237\345\272\246: ", nullptr));
        imuOdovelValueLabel->setText(QString());
        imuMagneticFieldLabel->setText(QCoreApplication::translate("MainWindow", "\347\243\201\345\234\272: ", nullptr));
        imuMagneticFieldValueLabel->setText(QString());
        imuTemperatureLabel->setText(QCoreApplication::translate("MainWindow", "\346\270\251\345\272\246: ", nullptr));
        imuTemperatureValueLabel->setText(QString());
        imuOutputMagneticHeadingLabel->setText(QCoreApplication::translate("MainWindow", "\347\243\201\350\210\252\345\220\221: ", nullptr));
        imuOutputMagneticHeadingValueLabel->setText(QString());
        imuTrueHeadingLabel->setText(QCoreApplication::translate("MainWindow", "\347\234\237\350\210\252\345\220\221: ", nullptr));
        imuTrueHeadingValueLabel->setText(QString());
        gpsOutputGroupBox->setTitle(QCoreApplication::translate("MainWindow", "GNSS\346\225\260\346\215\256", nullptr));
        gnssTimeLabel->setText(QCoreApplication::translate("MainWindow", "\346\227\266\351\227\264: ", nullptr));
        gnssTimeValueLabel->setText(QString());
        gnssPositionLabel->setText(QCoreApplication::translate("MainWindow", "\344\275\215\347\275\256: ", nullptr));
        gnssPositionValueLabel->setText(QString());
        gnssVelocityLabel->setText(QCoreApplication::translate("MainWindow", "\351\200\237\345\272\246: ", nullptr));
        gnssVelocityValueLabel->setText(QString());
        gnssStdLabel->setText(QCoreApplication::translate("MainWindow", "\346\240\207\345\207\206\345\267\256: ", nullptr));
        gnssStdValueLabel->setText(QString());
        gnssValidLabel->setText(QCoreApplication::translate("MainWindow", "\346\234\211\346\225\210: ", nullptr));
        gnssValidValueLabel->setText(QString());
        gnssQualityLabel->setText(QCoreApplication::translate("MainWindow", "\350\264\250\351\207\217: ", nullptr));
        gnssQualityValueLabel->setText(QString());
        gnssFixModeLabel->setText(QCoreApplication::translate("MainWindow", "\345\256\232\344\275\215\346\250\241\345\274\217: ", nullptr));
        gnssFixModeValueLabel->setText(QString());
        gnssHdopLabel->setText(QCoreApplication::translate("MainWindow", "HDOP: ", nullptr));
        gnssHdopValueLabel->setText(QString());
        gnssVdopLabel->setText(QCoreApplication::translate("MainWindow", "VDOP: ", nullptr));
        gnssVdopValueLabel->setText(QString());
        gnssPdopLabel->setText(QCoreApplication::translate("MainWindow", "PDOP: ", nullptr));
        gnssPdopValueLabel->setText(QString());
        gnssVisibleSvLabel->setText(QCoreApplication::translate("MainWindow", "\345\217\257\350\247\201\345\215\253\346\230\237: ", nullptr));
        gnssVisibleSvValueLabel->setText(QString());
        gnssUsedSvLabel->setText(QCoreApplication::translate("MainWindow", "\344\275\277\347\224\250\345\215\253\346\230\237: ", nullptr));
        gnssUsedSvValueLabel->setText(QString());
        gnssGroundSpeedLabel->setText(QCoreApplication::translate("MainWindow", "\345\234\260\351\235\242\351\200\237\345\272\246: ", nullptr));
        gnssGroundSpeedValueLabel->setText(QString());
        gnssTrueHeadingLabel->setText(QCoreApplication::translate("MainWindow", "\347\234\237\350\210\252\345\220\221: ", nullptr));
        gnssTrueHeadingValueLabel->setText(QString());
        gnssMagneticHeadingLabel->setText(QCoreApplication::translate("MainWindow", "\347\243\201\350\210\252\345\220\221: ", nullptr));
        gnssMagneticHeadingValueLabel->setText(QString());
        gnssSigmaLatLabel->setText(QCoreApplication::translate("MainWindow", "\347\272\254\345\272\246\346\240\207\345\207\206\345\267\256: ", nullptr));
        gnssSigmaLatValueLabel->setText(QString());
        gnssSigmaLonLabel->setText(QCoreApplication::translate("MainWindow", "\347\273\217\345\272\246\346\240\207\345\207\206\345\267\256: ", nullptr));
        gnssSigmaLonValueLabel->setText(QString());
        gnssSigmaAltLabel->setText(QCoreApplication::translate("MainWindow", "\351\253\230\345\272\246\346\240\207\345\207\206\345\267\256: ", nullptr));
        gnssSigmaAltValueLabel->setText(QString());
        gnssSigmaRangeLabel->setText(QCoreApplication::translate("MainWindow", "\344\274\252\350\267\235\346\240\207\345\207\206\345\267\256: ", nullptr));
        gnssSigmaRangeValueLabel->setText(QString());
        logDisplayGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\346\227\245\345\277\227\346\230\276\347\244\272", nullptr));
        logDisplayTextEdit->setPlainText(QCoreApplication::translate("MainWindow", "\346\227\245\345\277\227\346\230\276\347\244\272\345\214\272\345\237\237", nullptr));
        logLevelLabel->setText(QCoreApplication::translate("MainWindow", "\346\227\245\345\277\227\347\272\247\345\210\253:", nullptr));
        logLevelComboBox->setItemText(0, QCoreApplication::translate("MainWindow", "DEBUG", nullptr));
        logLevelComboBox->setItemText(1, QCoreApplication::translate("MainWindow", "INFO", nullptr));
        logLevelComboBox->setItemText(2, QCoreApplication::translate("MainWindow", "WARN", nullptr));
        logLevelComboBox->setItemText(3, QCoreApplication::translate("MainWindow", "ERROR", nullptr));

        tabWidget->setTabText(tabWidget->indexOf(tab_settings), QCoreApplication::translate("MainWindow", "\351\205\215\347\275\256\347\252\227\345\217\243", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\347\224\260\345\235\227\345\217\202\346\225\260", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\350\241\214\350\267\235 (m):", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\350\277\207\351\201\223\345\256\275\345\272\246 (m):", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\346\257\217\345\235\227\350\241\214\346\225\260:", nullptr));
        pathPlanningGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\350\267\257\345\276\204\350\247\204\345\210\222\345\217\202\346\225\260", nullptr));
        triggerDistanceLabel->setText(QCoreApplication::translate("MainWindow", "\350\247\246\345\217\221\347\202\271\350\267\235\347\246\273 (m):", nullptr));
        safetyMarginLabel->setText(QCoreApplication::translate("MainWindow", "\345\256\211\345\205\250\344\275\231\351\207\217 (m):", nullptr));
        fieldLengthLabel->setText(QCoreApplication::translate("MainWindow", "\345\260\217\345\214\272\351\225\277\345\272\246 (m):", nullptr));
        advanceDistanceLabel->setText(QCoreApplication::translate("MainWindow", "\346\217\220\345\211\215\351\207\217 (m):", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\346\216\247\345\210\266\345\231\250\345\217\202\346\225\260", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\347\263\273\347\273\237\345\273\266\350\277\237 (s):", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\350\277\237\346\273\236\351\230\210\345\200\274 (m):", nullptr));
        loadConfigButton->setText(QCoreApplication::translate("MainWindow", "\345\212\240\350\275\275\351\205\215\347\275\256", nullptr));
        saveConfigButton->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\351\205\215\347\275\256", nullptr));
        databaseGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\345\272\223\350\277\236\346\216\245", nullptr));
        databaseStatusLabel->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\345\272\223\347\212\266\346\200\201: \346\234\252\350\277\236\346\216\245", nullptr));
        connectDatabaseButton->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\346\225\260\346\215\256\345\272\223", nullptr));
        historyGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\345\216\206\345\217\262\344\275\234\344\270\232\346\225\260\346\215\256", nullptr));
        jobInfoGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\344\275\234\344\270\232\344\277\241\346\201\257", nullptr));
        jobIdLabel->setText(QCoreApplication::translate("MainWindow", "\344\275\234\344\270\232ID:", nullptr));
        jobIdValueLabel->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        jobDateLabel->setText(QCoreApplication::translate("MainWindow", "\344\275\234\344\270\232\346\227\245\346\234\237:", nullptr));
        jobDateValueLabel->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        jobAreaLabel->setText(QCoreApplication::translate("MainWindow", "\344\275\234\344\270\232\351\235\242\347\247\257:", nullptr));
        jobAreaValueLabel->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        jobSeedlingsLabel->setText(QCoreApplication::translate("MainWindow", "\346\222\255\347\247\215\346\225\260\351\207\217:", nullptr));
        jobSeedlingsValueLabel->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_data), QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\347\252\227\345\217\243", nullptr));
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
