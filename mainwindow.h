#pragma once

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

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
    QLineEdit    *m_urlEdit;        // поле для ввода URL
    QPushButton  *m_downloadButton; // кнопка "Скачать"
    QProgressBar *m_progress;       // прогресс-бар
    QLabel       *m_statusLabel;    // статус

    QNetworkAccessManager m_networkManager; // менеджер HTTP-запросов
    QNetworkReply        *m_reply = nullptr; // активный ответ
    QFile                 m_outputFile;      // файл, в который пишем данные

private slots:
    void onDownloadClicked();                       // нажата кнопка "Скачать"
    void onDownloadProgress(qint64 received, qint64 total); // обновление прогресса
    void onDownloadReadyRead();                     // пришли новые данные
    void onDownloadFinished();                      // загрузка закончена
};
