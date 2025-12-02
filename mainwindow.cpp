#include "MainWindow.h"

#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_button(new QPushButton("Нажми меня")),
    m_label(new QLabel("Привет, Qt!"))
{
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    layout->addWidget(m_label);
    layout->addWidget(m_button);

    central->setLayout(layout);
    setCentralWidget(central);

    connect(m_button, &QPushButton::clicked,
            this, &MainWindow::onButtonClicked);
}

void MainWindow::onButtonClicked()
{
    m_label->setText("Кнопка нажата!");
}
