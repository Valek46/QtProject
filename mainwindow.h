#pragma once

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QSslError>

class QPushButton;
class QLabel;
class QLineEdit;
class QProgressBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QLineEdit    *m_urlEdit;
    QPushButton  *m_downloadButton;
    QPushButton  *m_cancelButton;
    QProgressBar *m_progress;
    QLabel       *m_statusLabel;

    QNetworkAccessManager m_networkManager;
    QNetworkReply        *m_reply = nullptr;
    QFile                 m_outputFile;

private slots:
    void onDownloadClicked();
    void onDownloadProgress(qint64 received, qint64 total);
    void onDownloadReadyRead();
    void onDownloadFinished();
    void onCancelClicked();
};
