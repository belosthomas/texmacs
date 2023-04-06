#include "guile_thread.hpp"

#include <iostream>
#include <unordered_map>

#include <QDebug>

std::unordered_map<std::thread::id, QDebug*> guile_logs;
std::unordered_map<std::thread::id, QString> guile_last_messages;

void guile_error() {
    auto thread_id = std::this_thread::get_id();

    // delete guile_logs[thread_id];
    if (guile_logs.find(thread_id) != guile_logs.end()) {
        delete guile_logs[thread_id];
        guile_logs.erase(thread_id);
    }

    QString message = guile_last_messages[thread_id];
    std::string stdmessage = message.toStdString();
    throw std::runtime_error(stdmessage);
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
    guile_last_messages[thread_id] = str.left(last);
    delete guile_logs[thread_id];
    guile_logs[thread_id] = new QDebug(qInfo().noquote().nospace());
    *guile_logs[thread_id] << str.right(str.length() - last - 1);
}

void texmacs::guile_thread::run() {

    size_t stack_size = 100 * 1024 * 1024;
    void *stack_base = &stack_size; // this requires the -fno-strict-aliasing compiler flag

    scm_init_guile((int64_t*)stack_base, stack_size);

    SCM object_stack;

    const char* init_prg =
            "(debug-enable 'debug)\n"
            "(debug-enable 'backtrace)\n"
            "(read-set! keywords 'prefix)\n"
            "(read-enable 'positions)\n"
            "\n"
            "(define (display-to-string obj)\n"
            "  (call-with-output-string\n"
            "    (lambda (port) (display obj port))))\n"
            "(define (object->string obj)\n"
            "  (call-with-output-string\n"
            "    (lambda (port) (write obj port))))\n"
            "\n"
            "(define (texmacs-version) \"" "Alpha Liza" "\")\n"
            "(define object-stack '(()))";

    scm_c_eval_string (init_prg);
    object_stack = scm_variable_ref (scm_c_lookup("object-stack"));

    mObjectStack = &object_stack;

    while (true) {
        (void)object_stack; // we want to keep the object_stack variable alive
        std::function<void()> f = *mQueue.getPopElement();
        if (mQueue.isDestroyed()) {
            break;
        }
        mQueue.notifyPop();
        f();
    }

    mObjectStack = nullptr;

    return;
}

void texmacs::guile_thread::addToLaunchQueue(std::function<void()> f) {
    if (!mIsInitialized) {
        scm_use_embedded_ice9();
        scm_set_log_function(guile_log_function);
        set_error_callback(guile_error);

        start();
        mIsInitialized = true;
    }

    // check if we are in the guile thread with pthread
    if (QThread::currentThread() == this) {
        f();
        return;
    }

    *mQueue.getPushElement() = std::move(f);
    mQueue.notifyPush();
}