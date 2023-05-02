#ifndef TEXMACS_GUI_WELCOME_WIDGET_HPP
#define TEXMACS_GUI_WELCOME_WIDGET_HPP

#include <QApplication>
#include <QScreen>
#include <QWidget>
#include <QComboBox>
#include <QProgressBar>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QSplashScreen>

namespace texmacs {

    class WelcomeWidget : public QWidget {

        Q_OBJECT

    public:
        inline WelcomeWidget(QWidget *parent = 0) {

            mSplashPixmap = QPixmap(":/TeXmacs/misc/images/splash.png");

#if ANDROID
            mBanner.setPixmap(mSplashPixmap.scaledToWidth(QApplication::primaryScreen()->size().width(), Qt::SmoothTransformation));
#else
            mBanner.setPixmap(mSplashPixmap.scaledToWidth(QApplication::primaryScreen()->size().width() / 4, Qt::SmoothTransformation));
#endif


            mLayout.addWidget(&mBanner, 0, 0, 1, 2);
            mLayout.addWidget(&mComboBox, 1, 0);
            mLayout.addWidget(&mLaunchButton, 1, 1);
            mLayout.addWidget(&mProgressBar, 2, 0, 1, 2);
            mLayout.addWidget(&mStatus, 3, 0, 1, 2);

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
            if (mComboBox.count() == 1) {
                emit launch(mComboBox.currentText());
            } else {
                mLaunchButton.setEnabled(true);
            }
        }

        void addVersion(QString version) {
            mComboBox.addItem(version);
        }

        void setStatus(const QString &status) {
            mStatus.setText(status);
        }

    signals:
        void launch(QString version);

    private:
        QPixmap mSplashPixmap;
        QGridLayout mLayout;

        QLabel mBanner;
        QComboBox mComboBox;
        QPushButton mLaunchButton;
        QProgressBar mProgressBar;
        QLabel mStatus;

    };

}

#endif