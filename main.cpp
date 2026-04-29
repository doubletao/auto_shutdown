#include <QApplication>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include <QSettings>
#include <QMessageBox>
#include <QProcess>
#include <QFileInfo>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QDateTime>
#include <QCloseEvent>
#include <QStyle>
#include <windows.h>

class ShutdownDialog : public QDialog
{
    Q_OBJECT

public:
    ShutdownDialog(int countdownSeconds, QWidget *parent = nullptr)
        : QDialog(parent), m_seconds(countdownSeconds)
    {
        setWindowTitle("Power Warning");
        setFixedSize(400, 200);
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

        QVBoxLayout *layout = new QVBoxLayout(this);

        SYSTEM_POWER_STATUS sps;
        GetSystemPowerStatus(&sps);

        QLabel *titleLabel = new QLabel("Running on battery power!", this);
        titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #ff4444;");
        titleLabel->setAlignment(Qt::AlignCenter);

        m_countdownLabel = new QLabel(QString("System will shutdown in %1 seconds").arg(m_seconds), this);
        m_countdownLabel->setStyleSheet("font-size: 14px;");
        m_countdownLabel->setAlignment(Qt::AlignCenter);

        QLabel *batteryLabel = new QLabel(QString("Battery level: %1%").arg(sps.BatteryLifePercent), this);
        batteryLabel->setStyleSheet("font-size: 12px; color: #666;");
        batteryLabel->setAlignment(Qt::AlignCenter);

        m_cancelBtn = new QPushButton("Cancel Shutdown", this);
        m_cancelBtn->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; background-color: #4CAF50; color: white; border: none; border-radius: 5px; } QPushButton:hover { background-color: #45a049; }");
        connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

        layout->addStretch();
        layout->addWidget(titleLabel);
        layout->addSpacing(10);
        layout->addWidget(m_countdownLabel);
        layout->addSpacing(5);
        layout->addWidget(batteryLabel);
        layout->addSpacing(20);
        layout->addWidget(m_cancelBtn);
        layout->addStretch();

        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &ShutdownDialog::updateCountdown);
        m_timer->start(1000);
    }

private slots:
    void updateCountdown()
    {
        m_seconds--;
        m_countdownLabel->setText(QString("System will shutdown in %1 seconds").arg(m_seconds));

        if (m_seconds <= 0)
        {
            m_timer->stop();
            accept();
        }
    }

private:
    QLabel *m_countdownLabel;
    QPushButton *m_cancelBtn;
    QTimer *m_timer;
    int m_seconds;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        loadConfig();
        setupUI();
        setupTrayIcon();
        setupTimers();

        appendLog("Auto Shutdown started");
        appendLog(QString("Detection interval: %1 seconds").arg(m_detectInterval));
        appendLog(QString("Shutdown countdown: %1 seconds").arg(m_shutdownCountdown));
    }

protected:
    void closeEvent(QCloseEvent *event) override
    {
        if (m_trayIcon->isVisible())
        {
            hide();
            event->ignore();
        }
        else
        {
            QMainWindow::closeEvent(event);
        }
    }

private slots:
    void checkPowerStatus()
    {
        SYSTEM_POWER_STATUS sps;
        if (!GetSystemPowerStatus(&sps))
        {
            appendLog("Power status: FAILED");
            return;
        }

        if (sps.ACLineStatus == 1)
        {
            appendLog(QString("Power status: AC Power (battery %1%)").arg(sps.BatteryLifePercent));
        }
        else if (sps.ACLineStatus == 0)
        {
            appendLog(QString("Power status: BATTERY (battery %1%) - showing shutdown dialog").arg(sps.BatteryLifePercent));

            m_detectTimer->stop();

            ShutdownDialog dialog(m_shutdownCountdown);
            int result = dialog.exec();

            if (result == QDialog::Accepted)
            {
                appendLog("Performing system shutdown");
                performShutdown();
            }
            else
            {
                appendLog("Shutdown cancelled by user");
                m_detectTimer->start(m_detectInterval * 1000);
            }
        }
        else
        {
            appendLog("Power status: UNKNOWN");
        }
    }

    void openConfigFile()
    {
        QString configPath = QApplication::applicationDirPath() + "/config.ini";
        QProcess::startDetached("notepad.exe", QStringList() << configPath);
    }

    void showMainWindow()
    {
        showNormal();
        activateWindow();
    }

    void quitApplication()
    {
        m_trayIcon->hide();
        QApplication::quit();
    }

private:
    void loadConfig()
    {
        QString configPath = QApplication::applicationDirPath() + "/config.ini";
        QSettings settings(configPath, QSettings::IniFormat);

        m_detectInterval = settings.value("General/DetectInterval", 300).toInt();
        m_shutdownCountdown = settings.value("General/ShutdownCountdown", 60).toInt();

        if (m_detectInterval < 1)
            m_detectInterval = 1;
        if (m_shutdownCountdown < 1)
            m_shutdownCountdown = 1;
    }

    void setupUI()
    {
        setWindowTitle("Auto Shutdown");
        setMinimumSize(600, 400);

        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QVBoxLayout *layout = new QVBoxLayout(centralWidget);

        m_logText = new QPlainTextEdit(this);
        m_logText->setReadOnly(true);
        m_logText->setMaximumBlockCount(50);
        m_logText->setStyleSheet("QPlainTextEdit { font-family: Consolas, monospace; font-size: 12px; }");

        layout->addWidget(m_logText);
    }

    void setupTrayIcon()
    {
        QMenu *trayMenu = new QMenu(this);

        QAction *showAction = trayMenu->addAction("Show Window");
        connect(showAction, &QAction::triggered, this, &MainWindow::showMainWindow);

        QAction *detectAction = trayMenu->addAction("Check Now");
        connect(detectAction, &QAction::triggered, this, &MainWindow::checkPowerStatus);

        QAction *configAction = trayMenu->addAction("Open Config");
        connect(configAction, &QAction::triggered, this, &MainWindow::openConfigFile);

        trayMenu->addSeparator();

        QAction *quitAction = trayMenu->addAction("Exit");
        connect(quitAction, &QAction::triggered, this, &MainWindow::quitApplication);

        m_trayIcon = new QSystemTrayIcon(this);
        m_trayIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
        m_trayIcon->setContextMenu(trayMenu);
        m_trayIcon->setToolTip("Auto Shutdown");
        m_trayIcon->show();

        connect(m_trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::DoubleClick)
                showMainWindow();
        });
    }

    void setupTimers()
    {
        m_detectTimer = new QTimer(this);
        connect(m_detectTimer, &QTimer::timeout, this, &MainWindow::checkPowerStatus);
        m_detectTimer->start(m_detectInterval * 1000);
    }

    void appendLog(const QString &message)
    {
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        m_logText->appendPlainText(QString("[%1] %2").arg(timestamp, message));
    }

    void performShutdown()
    {
        HANDLE hToken;
        TOKEN_PRIVILEGES tkp;

        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
            return;

        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

        if (GetLastError() == ERROR_SUCCESS)
            ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCEIFHUNG, SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER);

        CloseHandle(hToken);
    }

    QPlainTextEdit *m_logText;
    QSystemTrayIcon *m_trayIcon;
    QTimer *m_detectTimer;
    int m_detectInterval;
    int m_shutdownCountdown;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);

    MainWindow window;
    window.show();
    return app.exec();
}

#include "main.moc"
