/*
 * Code by Liza Belos
 */

#ifndef TEXMACS_SCHEME_GUILE18_GUILE_THREAD_HPP
#define TEXMACS_SCHEME_GUILE18_GUILE_THREAD_HPP

#include <future>
#include <QThread>
#include <libguile.hpp>
#include "Utils/ThreadSafeQueue.hpp"


namespace texmacs {

    /**
     * @brief This class is a guile thread pool. This class allows to isolate guile from the main thread, and to instanciate multiple guile.
     */
    class guile_thread : public QThread {

        Q_OBJECT

    public:
        guile_thread() = default;

        /**
         * @brief Initializes the guile thread.
         * We do NOT want to call this function from any constructor to avoid throwing exceptions which are not catchable.
         */
        void init();

        /**
         * @brief This function runs a function in the guile thread.
         * @param f The function to run.
         */
        void addToLaunchQueue(std::function<void()> f);

        /**
         * Remove copy constructor and assignment operator.
         */
        guile_thread(const guile_thread&) = delete;
        guile_thread& operator=(const guile_thread&) = delete;

        /**
         * @brief This function returns the guile object stack.
         * @return The guile object stack.
         */
        SCM *unsafe_objectstack() const {
            return mObjectStack;
        }

    protected:
        /**
         * @brief This function is the main function of the guile thread.
         * @return
         */
        void run() override;

    private:
        bool mIsInitialized = false;
        thread_safe_queue<std::function<void()>, 10> mQueue;
        std::promise<bool> mIsInitializedPromise;

        SCM *mObjectStack;
    };

}

#endif