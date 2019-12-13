# C++ II HS2019 Project - OCR GUI

## Context

This repository holds the source code for the coding project of the c++ II
lecture HS2019.

## Application description
ImageViewer implements a UI split into an image view and a text editor view.
An image can be loaded into the image view and the user can then decide
whether to perform Optical Character Recognition (OCR) using the Tesseract API
directly, or if the image should be analysed for text region prior OCR. The later
uses the EAST DNN with a pretrained model.
In both cases, performing OCR will result in editable text which is shown in the
text editor view.
Furthermore, a screen capture functionality allows to take screen captured images.
When in screen capture mode, the topmost window is replaced by a combined image
of all available screens. The user can then use the mouse to select the desired
region and either confirm (return key) or terminate (escape key).
The user can save both the image as well as the text.

## Prerequisites
* [tesseract-ocr 4.1.0](https://github.com/tesseract-ocr/tesseract/releases/tag/4.1.0) - Tesseract used to perform Optical Character Recognition (OCR)
* [tessdata](https://github.com/tesseract-ocr/tessdata) - Pretrained data for the LSTM AI model used in Tesseract 4.1.0. Please make sure, line 257 in src/MainWindow.cpp specifies the correct path to tessdata/
* [opencv2/opencv.hpp](https://github.com/opencv/opencv)
* [opencv2/dnn.hpp](https://docs.opencv.org/3.4/db/ddc/dnn_2dnn_8hpp.html) - Pretrained EAST DNN model used for text area detection
* [Frozen East text detection model](https://www.dropbox.com/s/r2ingd0l3zt8hxs/frozen_east_text_detection.tar.gz?dl=1) - pretrained model data. Download and place the .pb file in ImageViewer/src/

## Authors
* **Simon Schweizer**

## Acknowledgments
[Tutorial](https://www.learnopencv.com/deep-learning-based-text-detection-using-opencv-c-python/) used for EAST model.
