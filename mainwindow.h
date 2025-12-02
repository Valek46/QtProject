#pragma once

#include <QMainWindow>

class QPushButton;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QPushButton *m_button;
    QLabel *m_label;

private slots:
    void onButtonClicked();
};
