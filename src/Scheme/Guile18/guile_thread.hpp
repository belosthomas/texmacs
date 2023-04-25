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

    class abstract_guile_launch_queue {

    public:
        virtual ~abstract_guile_launch_queue() = default;

        /**
         * @brief This function runs a function in the guile thread.
         * @param f The function to run.
         */
        virtual void addToLaunchQueue(std::function<void()> f, std::string debugInfo) = 0;

    };

    class guile_no_thread : public abstract_guile_launch_queue {

    public:
        /**
         * @brief This function runs a function in the guile thread.
         * @param f The function to run.
         */
        void addToLaunchQueue(std::function<void()> f, std::string debugInfo) final;

        static void* c_guile_run_function(void *function);

        static void* c_guile_run_function_with_lazy_catch(void *function);

    private:
        bool mIsInitialized = false;
    };

    /**
     * @brief This class is a guile thread pool. This class allows to isolate guile from the main thread, and to instanciate multiple guile.
     */
    class guile_thread : public QThread, public abstract_guile_launch_queue {

        Q_OBJECT

    public:
        guile_thread() = default;

        /**
         * @brief This function runs a function in the guile thread.
         * @param f The function to run.
         */
        void addToLaunchQueue(std::function<void()> f, std::string debugInfo) final;

        /**
         * Remove copy constructor and assignment operator.
         */
        guile_thread(const guile_thread&) = delete;
        guile_thread& operator=(const guile_thread&) = delete;

    protected:
        /**
         * @brief This function is the main function of the guile thread.
         * @return
         */
        void run() override;

    private:
        bool mIsInitialized = false;
        thread_safe_queue<std::function<void()>, 10> mQueue;
    };

}

#endif