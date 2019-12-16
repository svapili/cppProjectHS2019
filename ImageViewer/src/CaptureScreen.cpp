// system includes
#include <QApplication>
#include <QGuiApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QRect>
#include <QScreen>
#include <QPainter>
#include <QColor>
#include <QRegion>
#include <QShortcut>

// local includes
#include "CaptureScreen.h"

CaptureScreen::CaptureScreen(MainWindow *w) : QWidget(nullptr), m_window(w)
{
    // create a borderless tool window that is on top
    // ignore arrangement, stay topmost window, no title, widget is a tool window
    setWindowFlags(Qt::BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);

    // delete widget instance if closed
    setAttribute(Qt::WA_DeleteOnClose);
    // setMouseTracking(true);

    // capture the whole screen in a single image
    m_screen = captureDesktop();
    resize(m_screen.size());
    initShortcuts();
}

CaptureScreen::~CaptureScreen() {
}


/**
 * Captures all available screens into one image
 *
 * @returns pixmap image of all screens (combined)
 */
QPixmap CaptureScreen::captureDesktop() {

    // combine all available screens into one geometry
    QRect geometry;
    for (QScreen *const screen : QGuiApplication::screens()) {
        geometry = geometry.united(screen->geometry());
    }

    // create a pixmap image from the combined geometry
    QPixmap pixmap(QApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId(),
                                                          geometry.x(),
                                                          geometry.y(),
                                                          geometry.width(),
                                                          geometry.height()
                                                          )
            );

    // set the devices pixel ratio
    pixmap.setDevicePixelRatio(QApplication::desktop()->devicePixelRatio());
    return pixmap;
}

/**
 * Shows the captured screen (called everytime the widget updates)
 *
 * @param any paint event
 */
void CaptureScreen::paintEvent(QPaintEvent*) {

    // draw the captured screen
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_screen);

    // draw a translucent gray over selected area (which is constantly updated)
    QRegion grey(rect());
    if( m_topRight.x() != m_bottomLeft.x() && m_topRight.y() != m_bottomLeft.y() ) {
        painter.setPen( QColor(210, 100, 40, 255) );
        painter.drawRect( QRect(m_topRight, m_bottomLeft) );
        grey = grey.subtracted( QRect(m_topRight, m_bottomLeft) );
    }
    painter.setClipRegion(grey);
    QColor overlayColor(20, 20, 20, 50);
    painter.fillRect(rect(), overlayColor);
    painter.setClipRect( rect() );
}

/**
 * When mouse is pressed, save the position
 *
 * @param event of the mouse
 */
void CaptureScreen::mousePressEvent(QMouseEvent *event)
{
    m_mouseIsPressed = true;
    m_topRight = event->pos();
    m_bottomLeft = event->pos();
    update();
}

/**
 * When mouse is moved (and pressed), update bottom left coordinate
 *
 * @param event of the mouse
 */
void CaptureScreen::mouseMoveEvent(QMouseEvent *event)
{
    // check if mouse is just moved or also pressed
    if( !m_mouseIsPressed ) return;
    // if pressed and moved, update bottomLeft coorinate
    m_bottomLeft = event->pos();
    update();
}

/**
 * When mouse is released, deactivate mouseIsPressed flag, and update bottom left coordinate
 *
 * @param event of the mouse
 */
void CaptureScreen::mouseReleaseEvent(QMouseEvent *event)
{
    m_mouseIsPressed = false;
    m_bottomLeft = event->pos();
    update();
}

/**
 * When screen capture is terminated (escape key), close screen widget and return to normal view
 */
void CaptureScreen::closeScreenCapture()
{
    this->close();
    m_window->showNormal();
    m_window->activateWindow();
}

/**
 * When screen capture is confirmed (return key), show capture as image in GUI
 */
void CaptureScreen::confirmCapture()
{
    QPixmap image = m_screen.copy(QRect(m_topRight, m_bottomLeft));
    m_window->showImage(image);
    closeScreenCapture();
}

/**
 * Shortcuts to either close or confirm the screen capture mode
 */
void CaptureScreen::initShortcuts() {
    new QShortcut(  Qt::Key_Escape, this, SLOT( closeScreenCapture() )  );
    new QShortcut(  Qt::Key_Return, this, SLOT( confirmCapture() )  );
}
