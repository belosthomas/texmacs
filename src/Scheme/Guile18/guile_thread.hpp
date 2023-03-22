/*
 * Code by Liza Belos
 */

#ifndef TEXMACS_SCHEME_GUILE18_GUILE_THREAD_HPP
#define TEXMACS_SCHEME_GUILE18_GUILE_THREAD_HPP

#include <thread>
#include <future>
#include <libguile.hpp>
#include "Utils/ThreadSafeQueue.hpp"


namespace texmacs {

    /**
     * @brief This class is a guile thread pool. This class allows to isolate guile from the main thread, and to instanciate multiple guile.
     */
    class guile_thread {
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
        void run(std::function<void()> f);

        /**
         * @brief This function is a C proxy for the run function.
         */
        static void* c_run(void* data);

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
        void *run();

    private:
        std::thread::id mThreadId;
        bool mIsInitialized = false;
        std::thread mThread;
        thread_safe_queue<std::function<void()>, 10> mQueue;
        std::promise<bool> mIsInitializedPromise;
    };

}

#endif