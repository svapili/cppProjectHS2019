/**
 * @file CaptureScreen.cc
 * @brief
 * @author Simon Schweizer
 *
 */

#ifndef CAPTURESCREEN_H
#define CAPTURESCREEN_H

// system includes
#include <QWidget>
#include <QPixmap>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPoint>

// local includes
#include "MainWindow.h"

/**
 * Implementation of screen capture capability for OCR GUI
 *
 * CaptureScreen implements screen capture functionality so that
 * the user can not only load images from the drive, but also in form
 * of a screen shot.
 * When capture screen mode is entered, the topmost window is replaced
 * by a combined image of all available screens. He can then use his mouse
 * to select the desired region and either confirm (return) or terminate
 * (escape) the screen capture mode.
 */
class CaptureScreen : public QWidget {
    Q_OBJECT

public:
    explicit CaptureScreen(MainWindow *w);
    ~CaptureScreen();

protected:
    void paintEvent(QPaintEvent *event) override;           // to show the screen image as widget
    void mouseMoveEvent(QMouseEvent *event) override;       // called when the mouse is moved
    void mousePressEvent(QMouseEvent *event) override;      // called when a button of the mouse is pressed
    void mouseReleaseEvent(QMouseEvent *event) override;    // called when a pressed mouse button is released

private slots:
    void closeScreenCapture();      // terminate screen capture mode without taking image
    void confirmCapture();          // cofirm capture and return to GUI

private:
    void initShortcuts();
    QPixmap captureDesktop();

private:
    MainWindow *m_window;             // pointer to main window
    QPixmap m_screen;                 // storing the image of the whole screen
    QPoint m_topRight, m_bottomLeft;  // top right and bottom left point of selected rectangle
    bool m_mouseIsPressed;            // flag to indicate if mouse is pressed or just moving
};


#endif // CAPTURESCREEN_H
