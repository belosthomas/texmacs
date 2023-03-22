/*
 * Code by Liza Belos
 */

#ifndef TEXMACS_UTILS_THREADSAFEQUEUE_HPP
#define TEXMACS_UTILS_THREADSAFEQUEUE_HPP

#include <mutex>
#include <atomic>
#include <condition_variable>

namespace texmacs {

    template <typename T, int C> class thread_safe_queue
    {
    public:
        thread_safe_queue() {
            mSize = 0;
            mPushIndex = 0;
            mPopIndex = 0;
        }

        inline T *getPushElement() {
            if (mIsDestroyed) {
                return &mElements[mPushIndex];
            }
            std::unique_lock<std::mutex> lock(mMutex);
            while (mSize == C) {
                mConditionVariable.wait_for(lock, std::chrono::milliseconds(10));
                if (mIsDestroyed) {
                    return &mElements[mPushIndex];
                }
            }
            T *element = &mElements[mPushIndex];
            return element;
        }

        inline void notifyPush() {
            mPushIndex = (mPushIndex + 1) % C;
            mSize++;
            mConditionVariable.notify_all();
        }

        inline T *getPopElement()
        {
            if (mIsDestroyed) {
                mElements[mPopIndex] = T();
                return &mElements[mPopIndex];
            }
            std::unique_lock<std::mutex> lock(mMutex);
            while (mSize == 0) {
                mConditionVariable.wait_for(lock, std::chrono::milliseconds(10));
                if (mIsDestroyed) {
                    mElements[mPopIndex] = T();
                    return &mElements[mPopIndex];
                }
            }
            T *element = &mElements[mPopIndex];
            return element;
        }

        inline void notifyPop() {
            mPopIndex = (mPopIndex + 1) % C;
            mSize--;
            mConditionVariable.notify_all();
        }

        inline void destroy() {
            mIsDestroyed = true;
            mConditionVariable.notify_all();
        }

        inline bool isDestroyed() {
            return mIsDestroyed;
        }

        inline int getCurrentSize() {
            return mSize;
        }

    private:
        T mElements[C];
        std::atomic<int> mSize;
        int mPushIndex, mPopIndex;
        std::mutex mMutex;
        std::condition_variable mConditionVariable;
        std::atomic<bool> mIsDestroyed = false;
    };

}

#endif // TEXMACS_UTILS_THREADSAFEQUEUE_HPP