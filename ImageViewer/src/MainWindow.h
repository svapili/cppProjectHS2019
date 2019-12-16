/**
 * @file MainWindow.cc
 * @brief
 * @author Simon Schweizer
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// system includes
#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QStatusBar>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QTextEdit>
#include <QCheckBox>
#include <QTimer>

// local includes
#include "tesseract/baseapi.h"
#include "opencv2/opencv.hpp"
#include "opencv2/dnn.hpp"


/**
 * Implementation of the main window of the OCR GUI
 *
 * MainWindow implements the OCR GUI functionality consisting of
 * a two split window with an image view (left) and a text view (right).
 * An image can be loaded into the image view and the user can then decide
 * whether he wishes to perform Optical character recognition (ORC) using
 * the Tesseract API directly, or if the image should be analysed for
 * text regions prior OCR. The later uses the EAST deep neural network
 * implemented in opencv::dnn.
 * In both cases, performing OCR will result in editable text which is shown
 * in the text view.
 * Furthermore, the user can save the image as well as the text.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent=nullptr);
    ~MainWindow();
    void showImage(QPixmap);    // show image from screen capture

private:
    void initUI();              // all widgets (without actions)
    void createActions();       // to create all the actions
    void showImage(QString);    // show image from a path
    void showImage(cv::Mat);    // show image with openCV text areas
    void setupShortcuts();      // some key shortcuts

    void decode(const cv::Mat& scores, const cv::Mat& geometry, float scoreThresh,
        std::vector<cv::RotatedRect>& detections, std::vector<float>& confidences);
    cv::Mat detectTextAreas(QImage &image, std::vector<cv::Rect>&);

private slots:
    void openImage();
    void saveImageAs();
    void saveTextAs();
    void extractText();
    void captureScreen();
    void startCapture();

private:
    QMenu *m_fileMenu;

    QToolBar *m_fileToolBar;

    QGraphicsScene *m_imageScene;
    QGraphicsView *m_imageView;

    QTextEdit *m_editor;

    QStatusBar *m_mainStatusBar;
    QLabel *m_mainStatusLabel;

    QAction *m_openAction;
    QAction *m_saveImageAsAction;
    QAction *m_saveTextAsAction;
    QAction *m_exitAction;
    QAction *m_captureAction;
    QAction *m_ocrAction;                     // action to trigger optical caracter recognition
    QCheckBox *m_detectAreaCheckBox;

    QString m_currentImagePath;
    QGraphicsPixmapItem *m_currentImage;

    tesseract::TessBaseAPI *m_tesseractAPI;   // interface to handle ocr
    cv::dnn::Net m_net;                       // deep neural network instance containing pretrained EAST model
};

#endif // MAINWINDOW_H
