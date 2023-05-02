#include "guile_thread.hpp"

#include <iostream>
#include <unordered_map>

#include <QDebug>

std::unordered_map<std::thread::id, QDebug*> guile_logs;
std::unordered_map<std::thread::id, QString> guile_last_messages;

const char* guile_init_code =
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

    while (true) {
        std::function<void()> f = *mQueue.getPopElement();
        if (mQueue.isDestroyed()) {
            break;
        }
        mQueue.notifyPop();

        mGuileNoThread.addToLaunchQueue(f, "guile_thread::run");
    }

    return;
}

namespace texmacs {
    void initialize_smobs();
}

void texmacs::guile_thread::addToLaunchQueue(std::function<void()> f, std::string debugInfo) {

    qDebug() << "Adding command to Guile Launche Queue : '" << QString::fromStdString(debugInfo) << "'";

    if (!mIsInitialized) {
        //scm_use_embedded_ice9();
        //scm_set_log_function(guile_log_function);
        //set_error_callback(guile_error);

        start();
        mIsInitialized = true;

        //addToLaunchQueue([]() {
        //    initialize_smobs();
        //}, "initialize_smobs");
    }

    // check if we are in the guile thread with pthread
    if (QThread::currentThread() == this) {
        f();
        return;
    }

    *mQueue.getPushElement() = f;
    mQueue.notifyPush();
}

void* texmacs::guile_no_thread::c_guile_run_function(void *data) {
    std::function<void()> *f = (std::function<void()>*)data;
    (*f)();
    return nullptr;
}

SCM
TeXmacs_catcher  (void *data, SCM tag, SCM throw_args) {
    (void) data;
    return scm_cons (tag, throw_args);
}


void* texmacs::guile_no_thread::c_guile_run_function_with_lazy_catch(void *function) {
    return scm_internal_lazy_catch (SCM_BOOL_T,
                                    (scm_t_catch_body) c_guile_run_function, function,
                                    (scm_t_catch_handler) TeXmacs_catcher , function);
}

void texmacs::guile_no_thread::addToLaunchQueue(std::function<void()> f, std::string debugInfo) {
    if (!mIsInitialized) {
        scm_use_embedded_ice9();
        scm_set_log_function(guile_log_function);
        set_error_callback(guile_error);

        mIsInitialized = true;
        addToLaunchQueue([this]() {
            initialize_smobs();
            scm_c_eval_string (guile_init_code);
        }, "initialize_smobs");
    }

    scm_with_guile(c_guile_run_function_with_lazy_catch, (void*)&f);

}