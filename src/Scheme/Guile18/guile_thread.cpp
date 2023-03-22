#include "guile_thread.hpp"

#include <iostream>
#include <unordered_map>

#include <QDebug>

texmacs::guile_thread *guile_init_self;
std::mutex guile_init_mutex;

void* texmacs::guile_thread::c_run(void* data) {
    return guile_init_self->run();
}

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
    guile_init_mutex.lock();
    try {
        guile_init_self = this;
        scm_use_embedded_ice9();
        scm_set_log_function(guile_log_function);
        scm_with_guile(guile_thread::c_run, nullptr);
    } catch (...) {
        mIsInitializedPromise.set_exception(std::current_exception());
    }
}

void *texmacs::guile_thread::run() {
    mIsInitializedPromise.set_value(true);
    guile_init_mutex.unlock();
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

void texmacs::guile_thread::run(std::function<void()> f) {
    if (!mIsInitialized) {
        mThread = std::thread(&guile_thread::init, this);
        mIsInitialized = mIsInitializedPromise.get_future().get();
    }
    if (mThread.get_id() == std::this_thread::get_id()) {
        f();
        return;
    }
    *mQueue.getPushElement() = std::move(f);
    mQueue.notifyPush();
}