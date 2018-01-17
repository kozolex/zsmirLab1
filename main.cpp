#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;
using namespace std;
int main()
{
    VideoCapture cap(0); //Klasa do przechwytywania wideo z plików wideo, kamer itp.
    if ( !cap.isOpened() )  // jeżeli nie uzyskasz połączenia to...
    {
        cout << "Nie moge polaczyc sie z kamera / wideo" << endl;
        return -1;
    }
    int iLowH=0, iHighH=179, iLowS=0, iHighS=255, iLowV=0, iHighV=255;
    namedWindow("Kontrola", CV_WINDOW_AUTOSIZE);
    createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
    createTrackbar("HighH", "Control", &iHighH, 179);
    createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
    createTrackbar("HighS", "Control", &iHighS, 255);
    createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
    createTrackbar("HighV", "Control", &iHighV, 255);

    Mat imgOriginal;
    Mat imgHSV;
    Mat imgThresholded;
    while (true)
    {
        bool bSuccess = cap.read(imgOriginal);
        if (!bSuccess)
        {
            cout << "Nie mozna odczytac strumienia " << endl;
            break;
        }
        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);
        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);

        imshow("Kamera RGB", imgOriginal);
        imshow("Kamera HSV", imgHSV);
        imshow("Segmentacja - wynik", imgThresholded);
        if (waitKey(30) == 27)
        {
            cout << "Klawisz esc - KONIEC" << endl;
            break;
        }
    }
    return 0;
}
