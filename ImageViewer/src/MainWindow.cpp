// system includes
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QSplitter>
#include <QDebug>

// local includes
#include "MainWindow.h"
#include "CaptureScreen.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_currentImage(nullptr), m_tesseractAPI(nullptr)
{
    initUI();
}

MainWindow::~MainWindow()
{
    // Destroy object to release memory
    if(m_tesseractAPI != nullptr) {
        m_tesseractAPI->End();
        delete m_tesseractAPI;
    }
}

/**
 * Instantiates all widgets
 */
void MainWindow::initUI()
{
    // maximize window
    //this->setWindowState(Qt::WindowMaximized);
    this->resize(1600, 1200);

    // setup menubar
    m_fileMenu = menuBar()->addMenu("&File");

    // setup toolbar
    m_fileToolBar = addToolBar("File");

    // main area split in two parts (image and text)
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    m_imageScene = new QGraphicsScene(this);
    m_imageView = new QGraphicsView(m_imageScene);
    splitter->addWidget(m_imageView);     // left side is image view

    m_editor = new QTextEdit(this);
    splitter->addWidget(m_editor);        // right side is editor view

    // initial sizes of the two views
    QList<int> sizes = {400, 400};
    splitter->setSizes(sizes);

    setCentralWidget(splitter);

    // setup status bar
    m_mainStatusBar = statusBar();
    m_mainStatusLabel = new QLabel(m_mainStatusBar);
    m_mainStatusBar->addPermanentWidget(m_mainStatusLabel);
    m_mainStatusLabel->setText("C++ II HS2019 - OCR GUI - Simon Schweizer");

    // create actions (menus, toolbars etc.)
    createActions();
}

/**
 * create all actions for dropdown menu and toolbar
 */
void MainWindow::createActions()
{
    // create all actions and put them to menus
    m_openAction = new QAction("&Open", this);
    m_fileMenu->addAction(m_openAction);
    m_saveImageAsAction = new QAction("Save &Image as", this);
    m_fileMenu->addAction(m_saveImageAsAction);
    m_saveTextAsAction = new QAction("Save &Text as", this);
    m_fileMenu->addAction(m_saveTextAsAction);
    m_exitAction = new QAction("E&xit", this);
    m_fileMenu->addAction(m_exitAction);

    // add all actions to the toolbars
    m_fileToolBar->addAction(m_openAction);
    m_captureAction = new QAction("Capture screen", this);
    m_fileToolBar->addAction(m_captureAction);
    m_ocrAction = new QAction("OCR", this);
    m_fileToolBar->addAction(m_ocrAction);
    m_detectAreaCheckBox = new QCheckBox("Detect text areas", this);
    m_fileToolBar->addWidget(m_detectAreaCheckBox);

    // connect signals and slots
    connect(m_exitAction, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
    connect(m_openAction, SIGNAL(triggered(bool)), this, SLOT(openImage()));
    connect(m_saveImageAsAction, SIGNAL(triggered(bool)), this, SLOT(saveImageAs()));
    connect(m_saveTextAsAction, SIGNAL(triggered(bool)), this, SLOT(saveTextAs()));
    connect(m_ocrAction, SIGNAL(triggered(bool)), this, SLOT(extractText()));
    connect(m_captureAction, SIGNAL(triggered(bool)), this, SLOT(captureScreen()));

    // set up some shortcuts
    setupShortcuts();
}

/**
 * Open a new image
 */
void MainWindow::openImage()
{
    QFileDialog dialog(this);
    dialog.setWindowTitle("Open Image");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    QStringList filePaths;
    if (dialog.exec()) {
        filePaths = dialog.selectedFiles();
        showImage(filePaths.at(0));
    }
}

/**
 * Shows an image selected from path
 *
 * @param path to image to be shown
 */
void MainWindow::showImage(QString path)
{
    QPixmap image(path);
    showImage(image);
    m_currentImagePath = path;
    QString status = QString("%1, %2x%3, %4 Bytes").arg(path).arg(image.width())
        .arg(image.height()).arg(QFile(path).size());
    m_mainStatusLabel->setText(status);
}

/**
 * Show image with detected frames
 *
 * @param mat image with frames
 */
void MainWindow::showImage(cv::Mat mat)
{
    QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);

    QPixmap pixmap = QPixmap::fromImage(image);
    m_imageScene->clear();
    m_imageView->resetMatrix();
    m_currentImage = m_imageScene->addPixmap(pixmap);
    m_imageScene->update();
    m_imageView->setSceneRect(pixmap.rect());
}

/**
 * show an image captured from the screen (QPixmap overloading)
 *
 * @param image to be shown
 */
void MainWindow::showImage(QPixmap image)
{
    m_imageScene->clear();
    m_imageView->resetMatrix();
    m_currentImage = m_imageScene->addPixmap(image);
    m_imageScene->update();
    m_imageView->setSceneRect(image.rect());
 }

/**
 * Save imageview content as image
 */
void MainWindow::saveImageAs()
{
    // check if there is an image to save
    if (m_currentImage == nullptr) {
        QMessageBox::information(this, "Information", "Noting to save.");
        return;
    }

    // save dialog
    QFileDialog dialog(this);
    dialog.setWindowTitle("Save Image as..");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
        if(QRegExp(".+\\.(png|bmp|jpg)").exactMatch(fileNames.at(0))) {
            m_currentImage->pixmap().save(fileNames.at(0));
        } else {
            QMessageBox::information(this, "Error", "Save error: bad format or filename.");
        }
    }
}

/**
 * Save editorview content as text file
 */
void MainWindow::saveTextAs()
{
    QFileDialog dialog(this);
    dialog.setWindowTitle("Save Text as..");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Text files (*.txt)"));
    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
        if(QRegExp(".+\\.(txt)").exactMatch(fileNames.at(0))) {
            QFile file(fileNames.at(0));
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QMessageBox::information(this, "Error", "Can't save text.");
                return;
            }
            // write text to stream
            QTextStream out(&file);
            out << m_editor->toPlainText() << "\n";
        } else {
            QMessageBox::information(this, "Error", "Save error: format or filename not ok.");
        }
    }
}

/**
 * Setting up two shortcuts for convenience
 */
void MainWindow::setupShortcuts()
{
    // CNTRL + O to open a new image
    QList<QKeySequence> shortcuts;
    shortcuts << (Qt::CTRL + Qt::Key_O);
    m_openAction->setShortcuts(shortcuts);

    // CNTRL + Q to close the application
    shortcuts.clear();
    shortcuts << (Qt::CTRL + Qt::Key_Q);
    m_exitAction->setShortcuts(shortcuts);
}

/**
 * Extract text from image either using Tesseract OCR
 */
void MainWindow::extractText()
{
    // check if ther is an image to perfrom ocr on
    if (m_currentImage == nullptr) {
        QMessageBox::information(this, "Information", "Please open an image.");
        return;
    }

    // if there is an image, create Tesseract instance
    char *old_ctype = strdup(setlocale(LC_ALL, NULL));
    setlocale(LC_ALL, "C");

    // only initialize tesseract api once
    if (m_tesseractAPI == nullptr) {
        m_tesseractAPI = new tesseract::TessBaseAPI();
        // Initialize tesseract-ocr with pretrained eng data
        if (m_tesseractAPI->Init("/home/simon/programs/tesseract/share/tessdata", "eng")) {
            QMessageBox::information(this, "Error", "Failed to initialize tesseract.");
            return;
        }
    }

    // convert image into pixmap (convertion to 8Biit RGB allows any input format)
    QPixmap pixmap = m_currentImage->pixmap();
    QImage image = pixmap.toImage();
    image = image.convertToFormat(QImage::Format_RGB888);

    // pass image to tess api
    m_tesseractAPI->SetImage(image.bits(), image.width(), image.height(),
        3, image.bytesPerLine());

    // get the text from the image
    if (m_detectAreaCheckBox->checkState() == Qt::Checked) {
        std::vector<cv::Rect> areas;
        cv::Mat newImage = detectTextAreas(image, areas);   // detect areas if OCR checkbox activated
        showImage(newImage);
        m_editor->setPlainText("");
        for(cv::Rect &rect : areas) {
            m_tesseractAPI->SetRectangle(rect.x, rect.y, rect.width, rect.height);
            char *outText = m_tesseractAPI->GetUTF8Text();
            m_editor->setPlainText(m_editor->toPlainText() + outText);
            delete [] outText;
        }
    } else {
        char *outText = m_tesseractAPI->GetUTF8Text();
        m_editor->setPlainText(outText);
        delete [] outText;
    }

    // reset locale
    setlocale(LC_ALL, old_ctype);
    free(old_ctype);
}

/**
 * To detect text areas using openCV
 *
 * @param image to perform text detection on
 * @param areas holding the detected areas
 * @returns image with rectangles drawn around text images
 */
cv::Mat MainWindow::detectTextAreas(QImage &image, std::vector<cv::Rect> &areas)
{
    // set up neural network
    float confThreshold = 0.5;                              // confidence threshold
    float nmsThreshold = 0.4;                               // non-maximum suppression
    int inputWidth = 320;                                   // EAST requires multiple of 32
    int inputHeight = 320;
    std::string model = "./frozen_east_text_detection.pb";  // pretrained model data
    // Load dnn network
    if (m_net.empty()) {
        m_net = cv::dnn::readNet(model);
    }

    std::vector<cv::Mat> outs;                              // output layers of the model
    std::vector<std::string> layerNames(2);                 // names of the two layers used
    layerNames[0] = "feature_fusion/Conv_7/Sigmoid";        // sogmoid activation - wheter given region has text or no
    layerNames[1] = "feature_fusion/concat_3";              // feature map output - containing geometry of the image

    // convert image
    cv::Mat frame = cv::Mat(image.height(), image.width(), CV_8UC3, image.bits(), image.bytesPerLine()).clone();
    cv::Mat blob;

    // blobFromImage(input, output, scale factor, output size, training mean, swap R and B channel, crop output)
    cv::dnn::blobFromImage( frame, blob, 1.0, cv::Size(inputWidth, inputHeight),
        cv::Scalar(123.68, 116.78, 103.94), true, false
    ); // cv::Scalar holds the (rgb) mean used while the model was trained

    // pass input layer (blob) to dnn model and perform a round of forwarding
    m_net.setInput(blob);
    // outs contains the two output layers
    m_net.forward(outs, layerNames);

    // extract the two layers
    cv::Mat scores = outs[0];
    cv::Mat geometry = outs[1];

    // decode the layers into candidate text areas (boxes) and corresponding confidences
    std::vector<cv::RotatedRect> boxes;
    std::vector<float> confidences;
    decode(scores, geometry, confThreshold, boxes, confidences);

    // filter the candidate areas using non-max suppression
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

    // resizing ratio and color of boxes
    cv::Point2f ratio((float)frame.cols / inputWidth, (float)frame.rows / inputHeight);
    cv::Scalar red = cv::Scalar(255, 0, 0);

    // iterate over indices and place rectangles
    for (size_t i = 0; i < indices.size(); ++i) {
        cv::RotatedRect& box = boxes[indices[i]];
        cv::Rect area = box.boundingRect();

        // reverse resizing for the rectangles
        area.x *= ratio.x;
        area.width *= ratio.x;
        area.y *= ratio.y;
        area.height *= ratio.y;
        areas.push_back(area);
        cv::rectangle(frame, area, red, 1);
        QString index = QString("%1").arg(i);
        cv::putText( frame, index.toStdString(), cv::Point2f(area.x, area.y - 2),
            cv::FONT_HERSHEY_SIMPLEX, 0.5, red, 1
        );
    }
    return frame;
}

/**
 * Extract confidences and area from dnn output layers
 * Comment: I used the following repository:
 * https://github.com/opencv/opencv/blob/master/samples/dnn/text_detection.cpp#L119
 *
 * @param scores of each of the detected areas (first layer output)
 * @param geometry information of the image (second layer output)
 * @param scoreThresh = confidence threshold
 * @param detections of possible area candidates
 * @param confidences for each candidate
 */
void MainWindow::decode(const cv::Mat& scores, const cv::Mat& geometry, float scoreThresh,
    std::vector<cv::RotatedRect>& detections, std::vector<float>& confidences)
{
    CV_Assert(scores.dims == 4); CV_Assert(geometry.dims == 4);
    CV_Assert(scores.size[0] == 1); CV_Assert(scores.size[1] == 1);
    CV_Assert(geometry.size[0] == 1);  CV_Assert(geometry.size[1] == 5);
    CV_Assert(scores.size[2] == geometry.size[2]);
    CV_Assert(scores.size[3] == geometry.size[3]);

    detections.clear();
    const int height = scores.size[2];
    const int width = scores.size[3];
    for (int y = 0; y < height; ++y) {
        const float* scoresData = scores.ptr<float>(0, 0, y);
        const float* x0_data = geometry.ptr<float>(0, 0, y);
        const float* x1_data = geometry.ptr<float>(0, 1, y);
        const float* x2_data = geometry.ptr<float>(0, 2, y);
        const float* x3_data = geometry.ptr<float>(0, 3, y);
        const float* anglesData = geometry.ptr<float>(0, 4, y);

        for (int x = 0; x < width; ++x) {
            float score = scoresData[x];
            if (score < scoreThresh)
                continue;

            // Decode a prediction.
            // Multiple by 4 because feature maps are 4 time less than input image.
            float offsetX = x * 4.0f, offsetY = y * 4.0f;
            float angle = anglesData[x];
            float cosA = std::cos(angle);
            float sinA = std::sin(angle);
            float h = x0_data[x] + x2_data[x];
            float w = x1_data[x] + x3_data[x];

            // map the text areas (from resized image) back to the original input image
            cv::Point2f offset(offsetX + cosA * x1_data[x] + sinA * x2_data[x],
                offsetY - sinA * x1_data[x] + cosA * x2_data[x]);
            cv::Point2f p1 = cv::Point2f(-sinA * h, -cosA * h) + offset;
            cv::Point2f p3 = cv::Point2f(-cosA * w, sinA * w) + offset;
            cv::RotatedRect r(0.5f * (p1 + p3), cv::Size2f(w, h), -angle * 180.0f / (float)CV_PI);
            detections.push_back(r);
            confidences.push_back(score);
        } // end for
    } // end for
} // end decode

/**
 * When capture screen mode is requested, minimize the GUI main window and enter the capture mode
 */
void MainWindow::captureScreen()
{
    this->setWindowState(this->windowState() | Qt::WindowMinimized);
    QTimer::singleShot(1000, this, SLOT( startCapture()) );
}

/**
 * When capture screen mode is entered, show the whole-screen widget
 */
void MainWindow::startCapture()
{
    CaptureScreen *cap = new CaptureScreen(this);
    cap->show();
    cap->activateWindow();
}
