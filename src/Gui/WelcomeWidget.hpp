#ifndef TEXMACS_GUI_WELCOME_WIDGET_HPP
#define TEXMACS_GUI_WELCOME_WIDGET_HPP

#include <QWidget>
#include <QComboBox>
#include <QProgressBar>
#include <QGridLayout>
#include <QPushButton>

namespace texmacs {

    class WelcomeWidget : public QWidget {

        Q_OBJECT

    public:
        inline WelcomeWidget(QWidget *parent = 0) {

            mLayout.addWidget(&mComboBox, 0, 0);
            mLayout.addWidget(&mLaunchButton, 0, 1);
            mLayout.addWidget(&mProgressBar, 1, 0, 1, 2);

            setLayout(&mLayout);

            mLaunchButton.setText("Launch");
            mLaunchButton.setEnabled(false);

            connect(&mLaunchButton, &QPushButton::clicked, this, [this]() {
                emit launch(mComboBox.currentText());
            });
        }

    public slots:
        void setProgress(int value) {
            mProgressBar.setValue(value);
        }

        void setReady() {
            mLaunchButton.setEnabled(true);
        }

        void addVersion(QString version) {
            mComboBox.addItem(version);
        }

    signals:
        void launch(QString version);

    private:

        QGridLayout mLayout;
        QComboBox mComboBox;
        QPushButton mLaunchButton;
        QProgressBar mProgressBar;

    };

}

#endif