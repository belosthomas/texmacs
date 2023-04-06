//
// Created by lizab on 08/02/2023.
//

#include "DocumentWidget.hpp"

#include <QKeySequence>

#include "edit_interface.hpp"
#include "Plugins/Qt/qt_renderer.hpp"
#include "Plugins/Qt/qt_simple_widget.hpp"

texmacs::DocumentWidget::DocumentWidget(qt_simple_widget_rep *drawer, edit_interface_rep *editor,
                                        ThingyTabInnerWindow *parent) : QWidget(parent), mParent(parent),
                                                                        mDrawer(drawer), mEditor(editor) {
    assert(drawer != nullptr);
    assert(editor != nullptr);

    drawer->associatedDocumentWidget = this;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(16);

    // when this widget is focused, show the keyboard
    connect(this, &DocumentWidget::focusInEvent, this, &DocumentWidget::showKeyboard);
    // when this widget is not focused, hide the keyboard
    connect(this, &DocumentWidget::focusOutEvent, this, &DocumentWidget::hideKeyboard);

    setAttribute(Qt::WA_InputMethodEnabled, true);
    setInputMethodHints(Qt::ImhNoAutoUppercase | Qt::ImhPreferNumbers);

    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_StaticContents);
    //p_surface->setAttribute(Qt::WA_MacNoClickThrough);
    setAutoFillBackground(false);
    setBackgroundRole(QPalette::NoRole);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(&mTextEdit, &QPlainTextEdit::textChanged, this, &DocumentWidget::updateText);

}

void texmacs::DocumentWidget::paint(QPainter &painter) {

// Enable antialiasing
    painter.setRenderHint(QPainter::Antialiasing, true);

// Fix blurry images on high DPI screens
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);


    qt_renderer_rep ren(&painter, painter.device()->width(), painter.device()->height());
    mDrawer->invalidate_all();
    mDrawer->repaint_invalid_regions(&ren);

}

void keyEventToString(QKeyEvent *event) {
    QString nss = event->text();
    unsigned int kc = event->nativeVirtualKey();
    unsigned short unic = nss.data()[0].unicode();


}

void texmacs::DocumentWidget::keyPressEvent(QKeyEvent *event) {

    qDebug() << "key press";

    // send the key to mTextEdit
    #define TMKEYMAP(filter, str) if (event->key() == filter ) return mEditor->handle_keypress(str, texmacs_time());
    TMKEYMAP(Qt::Key_Space, "space");
    TMKEYMAP(Qt::Key_Tab, "tab");
    TMKEYMAP(Qt::Key_Backtab, "tab");
    TMKEYMAP(Qt::Key_Return, "return");
    TMKEYMAP(Qt::Key_Enter, "enter");
    TMKEYMAP(Qt::Key_Escape, "escape");
    TMKEYMAP(Qt::Key_Backspace, "backspace");
    TMKEYMAP(Qt::Key_Up, "up");
    TMKEYMAP(Qt::Key_Down, "down");
    TMKEYMAP(Qt::Key_Left, "left");
    TMKEYMAP(Qt::Key_Right, "right");
    TMKEYMAP(Qt::Key_F1, "F1");
    TMKEYMAP(Qt::Key_F2, "F2");
    TMKEYMAP(Qt::Key_F3, "F3");
    TMKEYMAP(Qt::Key_F4, "F4");
    TMKEYMAP(Qt::Key_F5, "F5");
    TMKEYMAP(Qt::Key_F6, "F6");
    TMKEYMAP(Qt::Key_F7, "F7");
    TMKEYMAP(Qt::Key_F8, "F8");
    TMKEYMAP(Qt::Key_F9, "F9");
    TMKEYMAP(Qt::Key_F10, "F10");
    TMKEYMAP(Qt::Key_F11, "F11");
    TMKEYMAP(Qt::Key_F12, "F12");
    TMKEYMAP(Qt::Key_F13, "F13");
    TMKEYMAP(Qt::Key_F14, "F14");
    TMKEYMAP(Qt::Key_F15, "F15");
    TMKEYMAP(Qt::Key_F16, "F16");
    TMKEYMAP(Qt::Key_F17, "F17");
    TMKEYMAP(Qt::Key_F18, "F18");
    TMKEYMAP(Qt::Key_F19, "F19");
    TMKEYMAP(Qt::Key_F20, "F20");
    TMKEYMAP(Qt::Key_F21, "F21");
    TMKEYMAP(Qt::Key_F22, "F22");
    TMKEYMAP(Qt::Key_F23, "F23");
    TMKEYMAP(Qt::Key_F24, "F24");
    TMKEYMAP(Qt::Key_F25, "F25");
    TMKEYMAP(Qt::Key_F26, "F26");
    TMKEYMAP(Qt::Key_F27, "F27");
    TMKEYMAP(Qt::Key_F28, "F28");
    TMKEYMAP(Qt::Key_F29, "F29");
    TMKEYMAP(Qt::Key_F30, "F30");
    TMKEYMAP(Qt::Key_F31, "F31");
    TMKEYMAP(Qt::Key_F32, "F32");
    TMKEYMAP(Qt::Key_F33, "F33");
    TMKEYMAP(Qt::Key_F34, "F34");
    TMKEYMAP(Qt::Key_F35, "F35");
    TMKEYMAP(Qt::Key_Insert, "insert");
    TMKEYMAP(Qt::Key_Delete, "delete");
    TMKEYMAP(Qt::Key_Home, "home");
    TMKEYMAP(Qt::Key_End, "end");
    TMKEYMAP(Qt::Key_PageUp, "pageup");
    TMKEYMAP(Qt::Key_PageDown, "pagedown");
    TMKEYMAP(Qt::Key_ScrollLock, "scrolllock");
    TMKEYMAP(Qt::Key_Pause, "pause");
    TMKEYMAP(Qt::Key_SysReq, "sysreq");
    TMKEYMAP(Qt::Key_Stop, "stop");
    TMKEYMAP(Qt::Key_Menu, "menu");
    TMKEYMAP(Qt::Key_Print, "print");
    TMKEYMAP(Qt::Key_Select, "select");
    TMKEYMAP(Qt::Key_Execute, "execute");
    TMKEYMAP(Qt::Key_Help, "help");
    TMKEYMAP(Qt::Key_section, "section");
    #undef TMKEYMAP
    
    qApp->sendEvent(&mTextEdit, event);
}

static unsigned int
mouse_state(QMouseEvent *event, bool flag) {
    unsigned int i = 0;
    Qt::MouseButtons bstate = event->buttons();
    Qt::MouseButton tstate = event->button();
    Qt::KeyboardModifiers kstate = event->modifiers();
    if (flag) bstate = bstate | tstate;
    if ((bstate & Qt::LeftButton) != 0) i += 1;
    if ((bstate & Qt::MiddleButton) != 0) i += 2;
    if ((bstate & Qt::RightButton) != 0) i += 4;
    if ((bstate & Qt::XButton1) != 0) i += 8;
    if ((bstate & Qt::XButton2) != 0) i += 16;
#ifdef Q_OS_MAC
    // We emulate right and middle clicks with ctrl and option, but we pass the
    // modifiers anyway: old code continues to work and new one can use them.
  if ((kstate & Qt::MetaModifier   ) != 0) i = 1024+4; // control key
  if ((kstate & Qt::AltModifier    ) != 0) i = 2048+2; // option key
  if ((kstate & Qt::ShiftModifier  ) != 0) i += 256;
  if ((kstate & Qt::ControlModifier) != 0) i += 4096;   // cmd key
#else
    if ((kstate & Qt::ShiftModifier) != 0) i += 256;
    if ((kstate & Qt::ControlModifier) != 0) i += 1024;
    if ((kstate & Qt::AltModifier) != 0) i += 2048;
    if ((kstate & Qt::MetaModifier) != 0) i += 4096;
#endif
    return i;
}

static string
mouse_decode(unsigned int mstate) {
    if (mstate & 2) return "middle";
    else if (mstate & 4) return "right";
        // we check for left clicks after the others for macos (see ifdef in mouse_state)
    else if (mstate & 1) return "left";
    else if (mstate & 8) return "up";
    else if (mstate & 16) return "down";
    return "unknown";
}


void texmacs::DocumentWidget::mousePressEvent(QMouseEvent *event) {
    QPoint point = event->pos() + origin();
    coord2 pt = from_qpoint(point);
    unsigned int mstate = mouse_state(event, false);

    string button = "unknown";
    switch (event->button()) {
        case Qt::MiddleButton:
            button = "middle";
            break;
        case Qt::RightButton:
            button = "right";
            break;
        case Qt::LeftButton:
            button = "left";
            break;
        case Qt::XButton1:
            button = "up";
            break;
        case Qt::XButton2:
            button = "down";
            break;
        default:
            break;
    }
    string s = "press-" * button;

    mEditor->handle_mouse(s, pt.x1, pt.x2, mstate, texmacs_time(), array<double>());
    event->accept();
}

void texmacs::DocumentWidget::mouseReleaseEvent(QMouseEvent *event) {
    QPoint point = event->pos() + origin();
    coord2 pt = from_qpoint(point);
    unsigned int mstate = mouse_state(event, false);

    string button = "unknown";
    switch (event->button()) {
        case Qt::MiddleButton:
            button = "middle";
            break;
        case Qt::RightButton:
            button = "right";
            break;
        case Qt::LeftButton:
            button = "left";
            break;
        case Qt::XButton1:
            button = "up";
            break;
        case Qt::XButton2:
            button = "down";
            break;
        default:
            break;
    }
    string s = "release-" * button;

    mEditor->handle_mouse(s, pt.x1, pt.x2, mstate, texmacs_time(), array<double>());
    event->accept();
}

void texmacs::DocumentWidget::showKeyboard() {
    QInputMethod *inputMethod = QGuiApplication::inputMethod();
    if (inputMethod != nullptr) {
        inputMethod->show();
    }
}

void texmacs::DocumentWidget::hideKeyboard() {
    QInputMethod *inputMethod = QGuiApplication::inputMethod();
    if (inputMethod != nullptr) {
        inputMethod->hide();
    }
}

void texmacs::DocumentWidget::updateText() {
    if (mTextEdit.toPlainText().isEmpty()) {
        return;
    }
    QString text = mTextEdit.toPlainText();
    qDebug() << "TEXT : " << text;
    mTextEdit.clear();
    mEditor->handle_keypress(utf8_to_cork(string(text.toUtf8().data(), text.toUtf8().size())), texmacs_time());
}

void texmacs::DocumentWidget::inputMethodEvent (QInputMethodEvent* event) {
    QString const & preedit_string = event->preeditString();
    QString const & commit_string = event->commitString();

    if (!commit_string.isEmpty()) {
        debug_qt << "IM commit :" << commit_string.toUtf8().data() << LF;
        mEditor->handle_keypress(utf8_to_cork(string(commit_string.toUtf8().data(), commit_string.toUtf8().size())), texmacs_time());
    }

    if (DEBUG_QT)
        debug_qt << "IM preediting :" << preedit_string.toUtf8().data() << LF;

    string r = "pre-edit:";
    if (!preedit_string.isEmpty())
    {

        // find cursor position in the preedit string
        QList<QInputMethodEvent::Attribute>  const & attrs = event->attributes();
        //    int pos = preedit_string.count();
        int pos = 0;
        bool visible_cur = false;
        for (int i=0; i< attrs.count(); i++)
            if (attrs[i].type == QInputMethodEvent::Cursor) {
                pos = attrs[i].start;
                visible_cur = (attrs[i].length != 0);
            }

        // find selection in the preedit string
        int sel_start = 0;
        int sel_length = 0;
        if (pos <  preedit_string.count()) {
            for (int i=0; i< attrs.count(); i++)
                if ((attrs[i].type == QInputMethodEvent::TextFormat) &&
                    (attrs[i].start <= pos) &&
                    (pos < attrs[i].start + attrs[i].length)) {
                    sel_start = attrs[i].start;
                    sel_length =  attrs[i].length;
                    if (!visible_cur) pos += attrs[i].length;
                }
        } else {
            sel_start = pos;
            sel_length = 0;
        }
        (void) sel_start; (void) sel_length;

        r = r * as_string (pos) * ":" * from_qstring (preedit_string);
    }

    mEditor->handle_keypress(r, texmacs_time());

    event->accept();
}

QVariant texmacs::DocumentWidget::inputMethodQuery (Qt::InputMethodQuery query) const {
    qDebug() << "inputMethodQuery " << query;
    switch (query) {
        case Qt::ImEnabled:
            return QVariant(true);
        case Qt::ImHints:
            return QVariant(Qt::ImhNoAutoUppercase | Qt::ImhPreferNumbers);
        case Qt::ImCursorRectangle:
            return QVariant(QRect(0, 0, 0, 0));
        default:
            return QVariant();
    }
/*
    switch (query) {
           case Qt::ImMicroFocus : {
             const QPoint &topleft= cursor_pos - tm_widget()->backing_pos + surface()->geometry().topLeft();
             return QVariant (QRect (topleft, QSize (5, 5)));
           }
           default:
        return QWidget::inputMethodQuery (query);
    }
    */
    return QWidget::inputMethodQuery (query);
}
