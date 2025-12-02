#include "MainWindow.h"

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QUrl>
#include <QFileInfo>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_urlEdit(new QLineEdit(this)),
    m_downloadButton(new QPushButton("Скачать", this)),
    m_cancelButton(new QPushButton("Отмена", this)),
    m_progress(new QProgressBar(this)),
    m_statusLabel(new QLabel("Ожидание URL...", this))
{
    setWindowTitle("Простой загрузчик файлов");

    m_progress->setRange(0, 100);
    m_progress->setValue(0);

    auto *urlLabel = new QLabel("Введите URL:", this);

    auto *urlLayout = new QHBoxLayout();
    urlLayout->addWidget(urlLabel);
    urlLayout->addWidget(m_urlEdit);

    auto *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(m_downloadButton);
    buttonsLayout->addWidget(m_cancelButton);

    auto *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(urlLayout);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addWidget(m_progress);
    mainLayout->addWidget(m_statusLabel);

    auto *central = new QWidget(this);
    central->setLayout(mainLayout);
    setCentralWidget(central);

    connect(m_downloadButton, &QPushButton::clicked,
            this, &MainWindow::onDownloadClicked);

    connect(m_cancelButton, &QPushButton::clicked,
            this, &MainWindow::onCancelClicked);
}

void MainWindow::onDownloadClicked()
{
    const QString urlText = m_urlEdit->text().trimmed();

    if (urlText.isEmpty()) {
        m_statusLabel->setText("Пожалуйста, введите URL.");
        return;
    }

    if (m_reply) {
        m_statusLabel->setText("Загрузка уже выполняется...");
        return;
    }

    QUrl url(urlText);
    if (!url.isValid()) {
        m_statusLabel->setText("Некорректный URL.");
        return;
    }

    QString suggestedName = QFileInfo(url.path()).fileName();
    if (suggestedName.isEmpty())
        suggestedName = "download.bin";

    QString savePath = QFileDialog::getSaveFileName(
        this,
        "Сохранить файл как...",
        suggestedName
        );

    if (savePath.isEmpty()) {
        m_statusLabel->setText("Сохранение отменено пользователем.");
        return;
    }

    m_outputFile.setFileName(savePath);
    if (!m_outputFile.open(QIODevice::WriteOnly)) {
        m_statusLabel->setText("Не удалось открыть файл для записи: " + savePath);
        return;
    }

    m_statusLabel->setText("Подключаемся к: " + url.toString());
    m_progress->setValue(0);

    QNetworkRequest request(url);
    m_reply = m_networkManager.get(request);

    connect(m_reply, &QNetworkReply::downloadProgress,
            this, &MainWindow::onDownloadProgress);
    connect(m_reply, &QIODevice::readyRead,
            this, &MainWindow::onDownloadReadyRead);
    connect(m_reply, &QNetworkReply::finished,
            this, &MainWindow::onDownloadFinished);
}

void MainWindow::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal <= 0) {
        m_progress->setRange(0, 0);
        return;
    }

    m_progress->setRange(0, 100);
    int percent = static_cast<int>((bytesReceived * 100) / bytesTotal);
    m_progress->setValue(percent);

    m_statusLabel->setText(
        QString("Загружено %1 / %2 байт")
            .arg(bytesReceived)
            .arg(bytesTotal)
        );
}

void MainWindow::onDownloadReadyRead()
{
    if (!m_reply)
        return;

    m_outputFile.write(m_reply->readAll());
}

void MainWindow::onDownloadFinished()
{
    if (!m_reply)
        return;

    m_outputFile.close();

    if (m_reply->error() == QNetworkReply::NoError) {
        m_statusLabel->setText("Загрузка завершена: " + m_outputFile.fileName());
    } else if (m_reply->error() == QNetworkReply::OperationCanceledError) {
        m_statusLabel->setText("Загрузка отменена.");
    } else {
        m_statusLabel->setText("Ошибка: " + m_reply->errorString());
    }

    m_reply->deleteLater();
    m_reply = nullptr;

    m_progress->setValue(0);
}

void MainWindow::onCancelClicked()
{
    if (!m_reply) {
        m_statusLabel->setText("Нет активной загрузки.");
        return;
    }

    m_statusLabel->setText("Отмена загрузки...");
    m_reply->abort();
}
