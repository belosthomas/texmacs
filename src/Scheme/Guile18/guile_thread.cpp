#include "guile_thread.hpp"

#include <iostream>
#include <unordered_map>

#include <QDebug>

std::unordered_map<std::thread::id, QDebug*> guile_logs;

void guile_error(const char *message) {
    throw std::runtime_error(message);
}

void guile_log_function(const char *cmsg, int len) {
    auto thread_id = std::this_thread::get_id();
    if (guile_logs.find(std::this_thread::get_id()) == guile_logs.end()) {
        guile_logs[thread_id] = new QDebug(qInfo().noquote().nospace());
    }
    std::string msg(cmsg, len);
    QString str = QString::fromStdString(msg);
    if (!str.contains("\n")) {
        *guile_logs[thread_id] << str;
        return;
    }

    qsizetype last = str.lastIndexOf("\n");
    *guile_logs[thread_id] << str.left(last);
    delete guile_logs[thread_id];
    guile_logs[thread_id] = new QDebug(qInfo().noquote().nospace());
    *guile_logs[thread_id] << str.right(str.length() - last - 1);
}

void texmacs::guile_thread::init() {
    // compute the stack base and size of the current thread with pthread and mThreadAttr
    int64_t stack_base;
    run(&stack_base);

}

void *texmacs::guile_thread::run(int64_t *stack_base_manual) {

    void *stack_base;
    size_t stack_size;
    pthread_attr_getstack(&mThreadAttr, &stack_base, &stack_size);

    scm_init_guile((int64_t*)stack_base_manual, stack_size);

    while (true) {
        std::function<void()> f = *mQueue.getPopElement();
        if (mQueue.isDestroyed()) {
            break;
        }
        mQueue.notifyPop();
        f();
    }
    return nullptr;
}

void *texmacs::guile_thread::c_init(void *data) {
    guile_thread *thread = static_cast<guile_thread*>(data);
    thread->init();
    return nullptr;
}

void texmacs::guile_thread::run(std::function<void()> f) {
    if (!mIsInitialized) {
        scm_use_embedded_ice9();
        scm_set_log_function(guile_log_function);
        set_error_callback(guile_error);

        pthread_attr_init(&mThreadAttr);
        // set the stack size to 100 MB
        pthread_attr_setstacksize(&mThreadAttr, 100 * 1024 * 1024);
        pthread_create(&mThreadID, &mThreadAttr, &guile_thread::c_init, this);
        mIsInitialized = true;
    }
    if (mThread.get_id() == std::this_thread::get_id()) {
        f();
        return;
    }
    *mQueue.getPushElement() = std::move(f);
    mQueue.notifyPush();
}