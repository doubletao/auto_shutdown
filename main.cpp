#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <windows.h>

class ShutdownDialog : public QDialog
{
    Q_OBJECT

public:
    ShutdownDialog(QWidget *parent = nullptr) : QDialog(parent), m_seconds(60)
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

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SYSTEM_POWER_STATUS sps;
    if (!GetSystemPowerStatus(&sps))
        return 1;

    if (sps.ACLineStatus == 1)
        return 0;

    if (sps.ACLineStatus == 0)
    {
        ShutdownDialog dialog;
        int result = dialog.exec();

        if (result == QDialog::Accepted)
            performShutdown();

        return 0;
    }

    return 1;
}

#include "main.moc"
