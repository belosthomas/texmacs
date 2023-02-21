/******************************************************************************
* MODULE     : Application.hpp
* DESCRIPTION: Main Application of TeXmacs
* COPYRIGHT  : (C) 2023 Liza Belos
*******************************************************************************
* This software falls under the GNU general public license version 3 or later.
* It comes WITHOUT ANY WARRANTY WHATSOEVER. For details, see the file LICENSE
* in the root directory or <http://www.gnu.org/licenses/gpl-3.0.html>.
******************************************************************************/

#ifndef TEXMACS_TEXMACSAPPLICATION_H
#define TEXMACS_TEXMACSAPPLICATION_H

#include <list>

#include <QApplication>
#include <QIcon>
#include <QSplashScreen>
#include <QTimer>
#include <QScreen>
#include <QDir>
#include <QDirIterator>
#include <QThread>

#include "Window.hpp"

#include "server.hpp"
#include "string.hpp"
#include "sys_utils.hpp"
#include "data_cache.hpp"
#include "boot.hpp"

extern url    tm_init_file;
extern url    tm_init_buffer_file;
extern string my_init_cmds;
extern string original_path;

extern int geometry_w, geometry_h;
extern int geometry_x, geometry_y;

extern tree the_et;
extern bool texmacs_started;

namespace texmacs {

    /**
     * @brief The TeXMavs Application class
     * This class is the main class of the application, responsible for the initialization of the application.
     */
    class Application : public QApplication {
        Q_OBJECT

    public:
        Application(int &argc, char **argv);

        /**
         * @brief Add a tab to the current window, or to a new window if there is no current window.
         * @param centralWidget The central widget of the tab.
         */
        void addTab(ThingyTabInnerWindow *centralWidget);

    signals:
        /**
         * @brief This signal is emitted when the application is ready to be shown.
         * It will close the splash screen and show the main window.
         */
        void initialized();

        /**
         * @brief This signal allows to change the splash screen message.
         */
        void initializationMessage(const QString& message);

    protected:
        /**
         * @brief Load and show the splash screen.
         */
        void loadSplashScreen();

        /**
         * @brief Reset TeXmacs by removing the ~/.TeXmacs directory.
         */
        void resetTeXmacs();

        /**
         * @brief Extract the resources from the application bundle.
         */
        void extractResources();

        /**
         * @brief Initialize the environment variable TEXMACS_*.
         */
        void initializeEnvironmentVariables();

        /**
         * @brief Initialize the scheme interpreter.
         */
        void initializeScheme();


    protected slots:
        /**
         * @brief This function is called when Qt has finish to initialize the application. This function initialize TeXmacs.
         */
        void onApplicationStarted();

    private:
        QSplashScreen mSlpashScreen;
        server *mServer = nullptr;
        std::list<Window> mWindows;

    };

    inline Application &getApplication() {
        QApplication *app = qApp;
        Application *texmacsApp = dynamic_cast<Application*>(app);
        assert(texmacsApp != nullptr);
        return *texmacsApp;
    }

}

#endif //TEXMACS_TEXMACSAPPLICATION_H
