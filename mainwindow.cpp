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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_urlEdit(new QLineEdit(this)),
    m_downloadButton(new QPushButton("Скачать", this)),
    m_progress(new QProgressBar(this)),
    m_statusLabel(new QLabel("Ожидание URL...", this))
{
    setWindowTitle("Простой загрузчик файлов");

    // Настройки прогресс-бара
    m_progress->setRange(0, 100);
    m_progress->setValue(0);

    // Подпись к полю ввода
    auto *urlLabel = new QLabel("Введите URL:", this);

    // Горизонтальный layout для URL: [Label][LineEdit]
    auto *urlLayout = new QHBoxLayout();
    urlLayout->addWidget(urlLabel);
    urlLayout->addWidget(m_urlEdit);

    // Основной вертикальный layout
    auto *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(urlLayout);
    mainLayout->addWidget(m_downloadButton);
    mainLayout->addWidget(m_progress);
    mainLayout->addWidget(m_statusLabel);

    // Центральный виджет
    auto *central = new QWidget(this);
    central->setLayout(mainLayout);
    setCentralWidget(central);

    // Связываем кнопку "Скачать" со слотом
    connect(m_downloadButton, &QPushButton::clicked,
            this, &MainWindow::onDownloadClicked);
}

void MainWindow::onDownloadClicked()
{
    const QString urlText = m_urlEdit->text().trimmed();

    if (urlText.isEmpty()) {
        m_statusLabel->setText("Пожалуйста, введите URL.");
        return;
    }

    // Если уже идёт загрузка — не даём запустить ещё одну
    if (m_reply) {
        m_statusLabel->setText("Загрузка уже выполняется...");
        return;
    }

    QUrl url(urlText);
    if (!url.isValid() || url.scheme().isEmpty()) {
        m_statusLabel->setText("Некорректный URL.");
        return;
    }

    // Определяем имя файла по URL
    QString fileName = QFileInfo(url.path()).fileName();
    if (fileName.isEmpty()) {
        fileName = "download.bin";
    }

    // Открываем файл на запись в той же папке, где exe
    m_outputFile.setFileName(fileName);
    if (!m_outputFile.open(QIODevice::WriteOnly)) {
        m_statusLabel->setText("Не удалось открыть файл для записи: " + fileName);
        return;
    }

    m_statusLabel->setText("Подключаемся к: " + url.toString());
    m_progress->setValue(0);

    // Запускаем HTTP GET запрос
    QNetworkRequest request(url);
    m_reply = m_networkManager.get(request);

    // Подключаем сигналы к слотам
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
        m_progress->setRange(0, 0); // "неопределённый" прогресс (анимация)
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

    // Читаем доступные данные и пишем в файл
    const QByteArray data = m_reply->readAll();
    m_outputFile.write(data);
}

void MainWindow::onDownloadFinished()
{
    if (!m_reply)
        return;

    m_outputFile.close();

    if (m_reply->error() == QNetworkReply::NoError) {
        m_statusLabel->setText("Загрузка завершена: " + m_outputFile.fileName());
    } else {
        m_statusLabel->setText("Ошибка: " + m_reply->errorString());
        // При ошибке можно удалить недокачанный файл:
        // m_outputFile.remove();
    }

    m_reply->deleteLater();
    m_reply = nullptr;

    m_progress->setRange(0, 100);
    m_progress->setValue(0);
}
