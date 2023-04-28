
/******************************************************************************
* MODULE     : QTMWidget.cpp
* DESCRIPTION: QT Texmacs widget class
* COPYRIGHT  : (C) 2008 Massimiliano Gubinelli and Joris van der Hoeven
*******************************************************************************
* This software falls under the GNU general public license version 3 or later.
* It comes WITHOUT ANY WARRANTY WHATSOEVER. For details, see the file LICENSE
* in the root directory or <http://www.gnu.org/licenses/gpl-3.0.html>.
******************************************************************************/

#include "QTMWidget.hpp"
#include "qt_gui.hpp"
#include "qt_utilities.hpp"
#include "qt_simple_widget.hpp"
#include "converter.hpp"
#include "boot.hpp"
#include "scheme.hpp"

#include "tm_config.h"

#include <QDebug>
#include <QEvent>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QPainter>
#include <QApplication>

#include <QBuffer>
#include <QMimeData>
#include <QByteArray>
#include <QImage>
#include <QUrl>
#include <QFileInfo>
#include <QRect>



#ifdef OS_MINGW
enum WindowsNativeModifiers {
    ShiftLeft            = 0x00000001,
    ControlLeft          = 0x00000002,
    AltLeft              = 0x00000004,
    MetaLeft             = 0x00000008,
    ShiftRight           = 0x00000010,
    ControlRight         = 0x00000020,
    AltRight             = 0x00000040,
    MetaRight            = 0x00000080,
    CapsLock             = 0x00000100,
    NumLock              = 0x00000200,
    ScrollLock           = 0x00000400,
    ExtendedKey          = 0x01000000,
};
#endif
static long int QTMWcounter = 0; // debugging hack

/*! Constructor.
 
  \param _parent The parent QWidget.
  \param _tmwid the TeXmacs widget who owns this object.
 */
QTMWidget::QTMWidget (QWidget* _parent, qt_widget _tmwid)
: QTMScrollView (_parent), tmwid (_tmwid),  imwidget (NULL),
  preediting (false)
{
  setObjectName (to_qstring ("QTMWidget" * as_string (QTMWcounter++)));// What is this for? (maybe only debugging?)
  setFocusPolicy (Qt::StrongFocus);
  setAttribute (Qt::WA_InputMethodEnabled);
  surface ()->setMouseTracking (true);
  surface ()->setAcceptDrops (true);
  grabGesture (Qt::PanGesture);
  grabGesture (Qt::PinchGesture);
  grabGesture (Qt::SwipeGesture);

#if (QT_VERSION >= QT_VERSION_CHECK(5,9,0))
  surface ()->setTabletTracking (true);
  for (QWidget *parent = surface()->parentWidget();
       parent != nullptr; parent = parent->parentWidget())
    parent->setTabletTracking(true);
#endif

  if (DEBUG_QT)
    debug_qt << "Creating " << from_qstring(objectName()) << " of widget "
             << (tm_widget() ? tm_widget()->type_as_string() : "NULL") << LF;
}

QTMWidget::~QTMWidget () {
  if (DEBUG_QT)
    debug_qt << "Destroying " << from_qstring(objectName()) << " of widget "
             << (tm_widget() ? tm_widget()->type_as_string() : "NULL") << LF;
}

qt_simple_widget_rep*
QTMWidget::tm_widget () const {
  return concrete_simple_widget (tmwid);
}

void
QTMWidget::scrollContentsBy (int dx, int dy) {
  QTMScrollView::scrollContentsBy (dx,dy);

  the_gui->force_update();
  // we force an update of the internal state to be in sync with the moving
  // scrollbars
}

void
QTMWidget::resizeEvent (QResizeEvent* event) {
  (void) event;
  // Is this ok?
  //coord2 s = from_qsize (event->size());
  //the_gui -> process_resize (tm_widget(), s.x1, s.x2);

  // the_gui->force_update();

  //FIXME: I would like to have a force_update here but this causes a failed
  //assertion in TeXmacs since the at the boot not every internal structure is
  //initialized at this point. It seems not too difficult to fix but I
  //postpone this to discuss with Joris.
  //
  //Not having a force_update results in some lack of sync of the surface
  //while the user is actively resizing with the mouse.
}

void
QTMWidget::resizeEventBis (QResizeEvent *event) {
  coord2 s = from_qsize (event->size());
  // old code : the_gui -> process_resize (tm_widget(), s.x1, s.x2);
}

/*!
 In the current implementation repainting takes place during the call to
 the widget's repaint_invalid_regions() method in the_gui::update. All
 we have to do is to take the backing store and put it on screen according
 to the QRegion marked invalid. 
 CHECK: Maybe just putting onscreen all the region bounding rectangles might 
 be less expensive.
*/
void
QTMWidget::paintEvent (QPaintEvent* event) {
  /*QPainter p (surface());
  for (auto qr : event->region()) {
    p.drawPixmap (QRect (qr.x(), qr.y(), qr.width(), qr.height()),
                  *(tm_widget()->backingPixmap),
                  QRect (retina_factor * qr.x(),
                         retina_factor * qr.y(),
                         retina_factor * qr.width(),
                         retina_factor * qr.height()));
  }*/
}

void
set_shift_preference (int key_code, char shifted) {
  set_user_preference ("shift-" * as_string (key_code), string (shifted));
}

bool
has_shift_preference (int key_code) {
  return has_user_preference ("shift-" * as_string (key_code));
}

string
get_shift_preference (char key_code) {
  return get_user_preference ("shift-" * as_string (key_code));
}


static unsigned int
mouse_state (QMouseEvent* event, bool flag) {
  unsigned int i= 0;
  Qt::MouseButtons bstate= event->buttons ();
  Qt::MouseButton  tstate= event->button ();
  Qt::KeyboardModifiers kstate= event->modifiers ();
  if (flag) bstate= bstate | tstate;
  if ((bstate & Qt::LeftButton     ) != 0) i += 1;
  if ((bstate & Qt::MiddleButton   ) != 0) i += 2;
  if ((bstate & Qt::RightButton    ) != 0) i += 4;
  if ((bstate & Qt::XButton1       ) != 0) i += 8;
  if ((bstate & Qt::XButton2       ) != 0) i += 16;
#ifdef Q_OS_MAC
    // We emulate right and middle clicks with ctrl and option, but we pass the
    // modifiers anyway: old code continues to work and new one can use them.
  if ((kstate & Qt::MetaModifier   ) != 0) i = 1024+4; // control key
  if ((kstate & Qt::AltModifier    ) != 0) i = 2048+2; // option key
  if ((kstate & Qt::ShiftModifier  ) != 0) i += 256;
  if ((kstate & Qt::ControlModifier) != 0) i += 4096;   // cmd key
#else
  if ((kstate & Qt::ShiftModifier  ) != 0) i += 256;
  if ((kstate & Qt::ControlModifier) != 0) i += 1024;
  if ((kstate & Qt::AltModifier    ) != 0) i += 2048;
  if ((kstate & Qt::MetaModifier   ) != 0) i += 4096;
#endif
  return i;
}

static string
mouse_decode (unsigned int mstate) {
  if (mstate & 2) return "middle";
  else if (mstate & 4) return "right";
    // we check for left clicks after the others for macos (see ifdef in mouse_state)
  else if (mstate & 1) return "left";
  else if (mstate & 8) return "up";
  else if (mstate & 16) return "down";
  return "unknown";
}

void
QTMWidget::kbdEvent (int key, Qt::KeyboardModifiers mods, const QString& s) {
  QKeyEvent ev (QEvent::KeyPress, key, mods, s);
  keyPressEvent (&ev);
}

void
QTMWidget::mousePressEvent (QMouseEvent* event) {
  if (is_nil (tmwid)) return;
  QPoint point = event->pos() + origin();
  coord2 pt = from_qpoint(point);
  unsigned int mstate= mouse_state (event, false);
  string s= "press-" * mouse_decode (mstate);
    // old code : the_gui -> process_mouse (tm_widget(), s, pt.x1, pt.x2,
    // old code :                         mstate, texmacs_time ());
  event->accept();
}

void
QTMWidget::mouseReleaseEvent (QMouseEvent* event) {
  if (is_nil (tmwid)) return;
  QPoint point = event->pos() + origin();
  coord2 pt = from_qpoint(point);
  unsigned int mstate = mouse_state (event, true);
  string s = "release-" * mouse_decode (mstate);
    // old code : the_gui->process_mouse (tm_widget(), s, pt.x1, pt.x2,
    // old code :                         mstate, texmacs_time());
  event->accept();
}

void
QTMWidget::mouseMoveEvent (QMouseEvent* event) {
  if (is_nil (tmwid)) return;
  QPoint point = event->pos() + origin();
  coord2 pt = from_qpoint(point);
  unsigned int mstate = mouse_state (event, false);
  string s = "move";
    // old code : the_gui->process_mouse (tm_widget(), s, pt.x1, pt.x2,
    // old code :                         mstate, texmacs_time ());
  event->accept();
}

#if (QT_VERSION >= 0x050000)
static unsigned int
tablet_state (QTabletEvent* event, bool flag) {
  unsigned int i= 0;
  Qt::MouseButtons bstate= event->buttons ();
  Qt::MouseButton  tstate= event->button ();
  if (flag) bstate= bstate | tstate;
  if ((bstate & Qt::LeftButton     ) != 0) i += 1;
  if ((bstate & Qt::MiddleButton   ) != 0) i += 2;
  if ((bstate & Qt::RightButton    ) != 0) i += 4;
  if ((bstate & Qt::XButton1       ) != 0) i += 8;
  if ((bstate & Qt::XButton2       ) != 0) i += 16;
  return i;
}

void
QTMWidget::tabletEvent (QTabletEvent* event) {
  if (is_nil (tmwid)) return;
  unsigned int mstate = tablet_state (event, true);
  string s= "move";
  if (event->button() != 0) {
    if (event->pressure () == 0) s= "release-" * mouse_decode (mstate);
    else s= "press-" * mouse_decode (mstate);
  }
  if ((mstate & 4) == 0 || s == "press-right") {
    QPoint point = event->pos() + origin() - surface()->pos();
    double x= point.x() + event->hiResGlobalX() - event->globalX();
    double y= point.y() + event->hiResGlobalY() - event->globalY();
    coord2 pt= coord2 ((SI) (x * PIXEL), (SI) (-y * PIXEL));
    array<double> data;
    data << ((double) event->pressure())
         << ((double) event->rotation())
         << ((double) event->xTilt())
         << ((double) event->yTilt())
         << ((double) event->z())
         << ((double) event->tangentialPressure());
      // old code :  the_gui->process_mouse (tm_widget(), s, pt.x1, pt.x2,
      // old code :                         mstate, texmacs_time (), data);
  }

  event->accept();
}
#endif

void
QTMWidget::gestureEvent (QGestureEvent* event) {
  if (is_nil (tmwid)) return;
  string s= "gesture";
  array<double> data;
  QPointF hotspot;
  if (QGesture *swipe_gesture = event->gesture(Qt::SwipeGesture)) {
    QSwipeGesture *swipe= static_cast<QSwipeGesture *> (swipe_gesture);
    s= "swipe";
    hotspot = swipe->hotSpot ();
    if (swipe->state() == Qt::GestureFinished) {
      if (swipe->horizontalDirection() == QSwipeGesture::Left)
        s= "swipe-left";
      else if (swipe->horizontalDirection() == QSwipeGesture::Right)
        s= "swipe-right";
      else if (swipe->verticalDirection() == QSwipeGesture::Up)
        s= "swipe-up";
      else if (swipe->verticalDirection() == QSwipeGesture::Down)
        s= "swipe-down";
    }
    else {
      event->accept ();
      return;
    }
  }
  else if (QGesture *pan_gesture = event->gesture(Qt::PanGesture)) {
    QPanGesture *pan= static_cast<QPanGesture *> (pan_gesture);
    string s= "pan";
    hotspot = pan->hotSpot ();
    //QPointF delta = pan->delta();
    //cout << "Pan " << delta.x() << ", " << delta.y() << LF;
  }
  else if (QGesture *pinch_gesture = event->gesture(Qt::PinchGesture)) {
    QPinchGesture *pinch= static_cast<QPinchGesture *> (pinch_gesture);
    s= "pinch";
    hotspot = pinch->hotSpot ();
    QPinchGesture::ChangeFlags changeFlags = pinch->changeFlags();
#if (QT_VERSION >= 0x050000)
    if (pinch->state() == Qt::GestureStarted) {
      pinch->setRotationAngle (0.0);
      pinch->setScaleFactor (1.0);
      s= "pinch-start";
    }
    else if (pinch->state() == Qt::GestureFinished) {
      pinch->setRotationAngle (0.0);
      pinch->setScaleFactor (1.0);
      s= "pinch-end";
    }
    else if (changeFlags & QPinchGesture::RotationAngleChanged) {
      qreal angle = pinch->rotationAngle();
      s= "rotate";
      data << ((double) angle);
    }
    else if (changeFlags & QPinchGesture::ScaleFactorChanged) {
      qreal scale = pinch->totalScaleFactor();
      s= "scale";
      data << ((double) scale);
    }
#else
    if (pinch->state() == Qt::GestureStarted) {
      QPoint point (hotspot.x(), hotspot.y());
      coord2 pt = from_qpoint (point);
      the_gui->process_mouse (tm_widget(), "pinch-start", pt.x1, pt.x2,
                              0, texmacs_time ());
    }
    if (changeFlags & QPinchGesture::RotationAngleChanged) {
      qreal a1 = pinch->lastRotationAngle();
      qreal a2 = pinch->rotationAngle();
      if (a2 != a1) {
        s= "rotate";
        data << ((double) a2);
      }
    }
    else if (changeFlags & QPinchGesture::ScaleFactorChanged) {
      qreal s1 = pinch->lastScaleFactor();
      qreal s2 = pinch->scaleFactor();
      if (s1 != s2) {
        s= "scale";
        data << ((double) s2);
      }
      else {
        pinch->setScaleFactor (1.0);
        s= "pinch-end";
      }
    }
    else if (pinch->state() == Qt::GestureFinished) {
      pinch->setRotationAngle (0.0);
      pinch->setScaleFactor (1.0);
      s= "pinch-end";
    }
#endif
  }
  else return;
  QPoint point (hotspot.x(), hotspot.y());
  coord2 pt = from_qpoint (point);
  //cout << s << ", " << pt.x1 << ", " << pt.x2 << LF;
    // old code : the_gui->process_mouse (tm_widget(), s, pt.x1, pt.x2,
    // old code :                       0, texmacs_time (), data);
  event->accept();
}


bool
QTMWidget::event (QEvent* event) {
    // Catch Keypresses to avoid default handling of (Shift+)Tab keys
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *ke = static_cast<QKeyEvent*> (event);
    keyPressEvent (ke);
    return true;
  }
  /* NOTE: we catch ShortcutOverride in order to disable the QKeySequences we
   assigned to QActions while building menus, etc. In doing this, we keep the
   shortcut text in the menus while relaying all keypresses through the editor*/
  if (event->type() == QEvent::ShortcutOverride) {
    event->accept();
    return true;
  }
  if (event->type() == QEvent::Gesture) {
    gestureEvent(static_cast<QGestureEvent*>(event));
    return true;
  }
  return QTMScrollView::event (event);
}

void
QTMWidget::focusInEvent (QFocusEvent * event) {
  if (!is_nil (tmwid)) {
    if (DEBUG_QT) debug_qt << "FOCUSIN: " << tm_widget()->type_as_string() << LF;
      // old code : the_gui->process_keyboard_focus (tm_widget(), true, texmacs_time());
  }
  QTMScrollView::focusInEvent (event);
}

void
QTMWidget::focusOutEvent (QFocusEvent * event) {
  if (!is_nil (tmwid)) {
    if (DEBUG_QT) debug_qt << "FOCUSOUT: " << tm_widget()->type_as_string() << LF;
      // old code :  the_gui -> process_keyboard_focus (tm_widget(), false, texmacs_time());
  }
  QTMScrollView::focusOutEvent (event);
}

QSize
QTMWidget::sizeHint () const {
  SI w = 0, h = 0;
  if (!is_nil (tmwid)) tm_widget()->handle_get_size_hint (w, h);
  return to_qsize (w, h);
}

void
QTMWidget::dragEnterEvent (QDragEnterEvent *event)
{
  if (is_nil (tmwid)) return;
  const QMimeData *md = event->mimeData();

  if (md->hasText() ||
      md->hasUrls() ||
      md->hasImage() ||
      md->hasFormat("application/pdf") ||
      md->hasFormat("application/postscript"))
      event->acceptProposedAction();
}


// cache to transfer drop data to the editor
// via standard mouse events, see dropEvent below

extern int drop_payload_serial;
extern hashmap<int, tree> payloads;

void
QTMWidget::dropEvent (QDropEvent *event) {
  if (is_nil (tmwid)) return;

  QPoint point = event->pos () + origin ();
  coord2 pt= from_qpoint (point);

  tree doc (CONCAT);
  const QMimeData *md= event->mimeData ();
  QByteArray buf;

  if (md->hasUrls ()) {
    QList<QUrl> l= md->urls ();
    for (int i=0; i<l.size (); i++) {
      string name;
#ifdef OS_MACOS
      name= from_qstring (fromNSUrl (l[i]));
#else
      name= from_qstring (l[i].toLocalFile ());
#endif
      string orig_name= name;
#ifdef OS_MINGW
      if (N(name) >=2 && is_alpha (name[0]) && name[1] == ':')
        name= "/" * locase_all (name (0, 1)) * name (2, N(name));
#endif
      string extension = suffix (name);
      if ((extension == "eps") || (extension == "ps")   ||
#if (QT_VERSION >= 0x050000)
          (extension == "svg") ||
#endif
          (extension == "pdf") || (extension == "png")  ||
          (extension == "jpg") || (extension == "jpeg")) {
        string w, h;
        qt_pretty_image_size (url_system (orig_name), w, h);
        tree im (IMAGE, name, w, h, "", "");
        doc << im;
      } else {
        doc << name;
      }
    }
  } else if (md->hasImage ()) {
    QBuffer qbuf (&buf);
    QImage image= qvariant_cast<QImage> (md->imageData());
    QSize size= image.size ();
    qbuf.open (QIODevice::WriteOnly);
    image.save (&qbuf, "PNG");
    int ww= size.width (), hh= size.height ();
    string w, h;
    qt_pretty_image_size (ww, hh, w, h);
    tree t (IMAGE, tree (RAW_DATA, string (buf.constData (), buf.size()), "png"),
            w, h, "", "");
    doc << t;
  } else if (md->hasFormat("application/postscript")) {
    buf= md->data("application/postscript");
    tree t (IMAGE, tree (RAW_DATA, string (buf.constData (), buf.size ()), "ps"),
                   "", "", "", "");
    doc << t;
  } else if (md->hasFormat("application/pdf")) {
    buf= md->data("application/pdf");
    tree t (IMAGE, tree (RAW_DATA, string (buf.constData (), buf.size ()), "pdf"),
                   "", "", "", "");
    doc << t;
  } else if (md->hasText ()) {
    buf= md->text ().toUtf8 ();
    doc << string (buf.constData (), buf.size ());
  }

  if (N(doc)>0) {
    if (N(doc) == 1)
      doc= doc[0];
    else {
      tree sec (CONCAT, doc[0]);
      for (int i=1; i<N(doc); i++)
        sec << " " << doc[i];
      doc= sec;
    }
    int ticket= drop_payload_serial++;
    payloads (ticket)= doc;
      // old code : the_gui->process_mouse (tm_widget(), "drop", pt.x1, pt.x2,
      // old code :                         ticket, texmacs_time ());
    event->acceptProposedAction();
  }
}

static unsigned int
wheel_state (QWheelEvent* event) {
  // TODO: factor mouse_state, tablet_state, wheel_state
  // This should be easier on modern versions of Qt
  unsigned int i= 0;
  Qt::MouseButtons bstate= event->buttons ();
  Qt::KeyboardModifiers kstate= event->modifiers ();
  if ((bstate & Qt::LeftButton     ) != 0) i += 1;
  if ((bstate & Qt::MiddleButton   ) != 0) i += 2;
  if ((bstate & Qt::RightButton    ) != 0) i += 4;
  if ((bstate & Qt::XButton1       ) != 0) i += 8;
  if ((bstate & Qt::XButton2       ) != 0) i += 16;
#ifdef Q_OS_MAC
    // We emulate right and middle clicks with ctrl and option, but we pass the
    // modifiers anyway: old code continues to work and new one can use them.
  if ((kstate & Qt::MetaModifier   ) != 0) i = 1024+4; // control key
  if ((kstate & Qt::AltModifier    ) != 0) i = 2048+2; // option key
  if ((kstate & Qt::ShiftModifier  ) != 0) i += 256;
  if ((kstate & Qt::ControlModifier) != 0) i += 4096;   // cmd key
#else
  if ((kstate & Qt::ShiftModifier  ) != 0) i += 256;
  if ((kstate & Qt::ControlModifier) != 0) i += 1024;
  if ((kstate & Qt::AltModifier    ) != 0) i += 2048;
  if ((kstate & Qt::MetaModifier   ) != 0) i += 4096;
#endif
  return i;
}

void
QTMWidget::wheelEvent(QWheelEvent *event) {
  if (is_nil (tmwid)) return;
  if (as_bool (call ("wheel-capture?"))) {
#if (QT_VERSION >= 0x060000)
    QPointF pos  = event->position();
    QPointF point = QPointF (pos.x(), pos.y()) + origin();
#else
    QPoint  point= event->pos() + origin();
#endif
#if (QT_VERSION >= 0x050000)
    QPoint  wheel= event->pixelDelta();
#else
    double delta= event->delta();
    bool   hor  = event->orientation() == Qt::Horizontal;
    QPoint wheel (hor? delta: 0.0, hor? 0.0: delta);
#endif
    coord2 pt(point.x(), point.y());
    coord2 wh = from_qpoint (wheel);
    unsigned int mstate= wheel_state (event);
    array<double> data; data << ((double) wh.x1) << ((double) wh.x2);
      // old code : the_gui -> process_mouse (tm_widget(), "wheel", pt.x1, pt.x2,
      // old code :                         mstate, texmacs_time (), data);
  }
  else if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
    if (event->angleDelta().y() > 0)
      call ("zoom-in",  sqrt (sqrt (2.0)));
    else
      call ("zoom-out",  sqrt (sqrt (2.0)));
  }
  else QAbstractScrollArea::wheelEvent (event);
}
