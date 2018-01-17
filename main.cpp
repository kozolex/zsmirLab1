#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "serialib/serialib.h"
#define DEVICE_PORT "\\\\.\\COM27"
using namespace cv;
using namespace std;


int iLowH=0, iHighH=179, iLowS=0, iHighS=255, iLowV=0, iHighV=255;
void mouseEvent(int evt, int x, int y, int flags, void* param)
{
    Mat* hsv = (Mat*) param;
    if (evt == CV_EVENT_LBUTTONDOWN)
    {
        iLowH = (int)(*hsv).at<Vec3b>(y, x)[0] - 5;
        iHighH = (int)(*hsv).at<Vec3b>(y, x)[0] + 5;

        iLowS = (int)(*hsv).at<Vec3b>(y, x)[1] - 30;
        iHighS = (int)(*hsv).at<Vec3b>(y, x)[1] + 50;

        iLowV = (int)(*hsv).at<Vec3b>(y, x)[2] - 50;
        iHighV = (int)(*hsv).at<Vec3b>(y, x)[2] + 100;

        if (iLowH <0)
            iLowH=0;
        if (iLowS <0)
            iLowS=0;
        if (iHighH >255)
            iHighH = 255;
        if (iHighV >255)
            iHighV = 255;
        cout<<"\n\niLowH: "<<iLowH<<"\niHighH: "<<iHighH<<"\n\niLowS: "<<iLowS<<"\niHighS: "<<iHighS<<"\n\niLowV: "<<iLowV<<"\niHighV: "<<iHighV<<endl;
    }
}

int main()
{
    int sizeEroDil = 5;
    serialib LS;
    int Ret;
    Ret=LS.Open(DEVICE_PORT,9600);

    if (Ret!=1)
    {
        cout<<"Blad polaczenia z portem szeregowym\n";
        cout<<"kod bledu:"<<Ret<<endl;
        return -1;
    }
    else
        cout<<"Port szeregowy polaczony !\n";
    VideoCapture cap(0); //Klasa do przechwytywania wideo z plików wideo, kamer itp.
    if ( !cap.isOpened() )  // jeżeli nie uzyskasz połączenia to...
    {
        cout << "Nie moge polaczyc sie z kamera / wideo" << endl;
        return -1;
    }


    namedWindow("Kontrola", CV_WINDOW_AUTOSIZE);
    createTrackbar("LowH", "Kontrola", &iLowH, 179); //Kolor (0 - 179)
    createTrackbar("HighH", "Kontrola", &iHighH, 179);
    createTrackbar("LowS", "Kontrola", &iLowS, 255); //Nasycenie (0 - 255)
    createTrackbar("HighS", "Kontrola", &iHighS, 255);
    createTrackbar("LowV", "Kontrola", &iLowV, 255);//Jasnosc (0 - 255)
    createTrackbar("HighV", "Kontrola", &iHighV, 255);
    createTrackbar("EroDill", "Kontrola", &sizeEroDil, 20);//Morfologia (0 - 20)

    Mat imgOriginal;
    Mat imgHSV;
    Mat imgThresholded;
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    RNG rng(12345);
    int iLastX = -1;
    int iLastY = -1;
    while (true)
    {
        bool bSuccess = cap.read(imgOriginal);
        if (!bSuccess)
        {
            cout << "Nie mozna odczytac strumienia " << endl;
            break;
        }
        setMouseCallback("Kamera RGB", mouseEvent, &imgHSV);
        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);
        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);

        if (sizeEroDil>0)
        {
            //morfologiczne otwarcie
            erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(sizeEroDil, sizeEroDil)) );
            dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(sizeEroDil, sizeEroDil)) );
            //morfologiczne zamknięcie
            dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(sizeEroDil, sizeEroDil)) );
            erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(sizeEroDil, sizeEroDil)) );
        }

        Canny( imgThresholded, canny_output, 100, 100*2, 3 );
        findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

        unsigned int contBigIndex = 0;
        unsigned int maxContour = 0;

        for( int i = 0; i< contours.size(); i++ )
        {
            if(contourArea( contours[i],false) > maxContour)
            {
                maxContour = contourArea( contours[i],false);
                contBigIndex = i;
            }
        }
        drawContours( imgOriginal, contours, contBigIndex, Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) ), 2, 8, hierarchy, 0, Point() );
        imgThresholded = Mat::zeros( imgThresholded.size(),CV_8U );
        drawContours( imgThresholded, contours, contBigIndex, Scalar(255, 255, 255), CV_FILLED, 8, hierarchy, 0, Point() );

        Moments oMoments = moments(imgThresholded);
        double dM01 = oMoments.m01;
        double dM10 = oMoments.m10;
        double dArea = oMoments.m00;
        int posX = dM10 / dArea;
        int posY = dM01 / dArea;
        if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0 && iLastX !=posX )
        {
            line(imgOriginal,Point(posX, posY),Point(posX, posY), Scalar(0,0,255), 3); //Draw a red line from the previous point to the current point
            putText(imgOriginal,format("x = %d", posX),Point(posX, posY+15), FONT_HERSHEY_SIMPLEX, 0.5,Scalar(0,255,255),1,false);
            putText(imgOriginal,format("y = %d", posY),Point(posX, posY+30), FONT_HERSHEY_SIMPLEX, 0.5,Scalar(255,255,0),1,false);
            char c_string = static_cast<char> (posX/3.55);
            Ret=LS.WriteChar(c_string);
            if (Ret!=1)
            {
                cout<<"Blad przesylu danych\n";
                cout<<"kod bledu:"<<Ret<<"\n";
                return Ret;
            }

        }
        iLastX = posX;
        iLastY = posY;

        imshow("Kamera HSV", imgHSV);
        imshow("Segmentacja - wynik", imgThresholded);
        imshow("Krawedzie", canny_output);
        imshow("Kamera RGB", imgOriginal);
        if (waitKey(30) == 27)
        {
            cout << "Klawisz esc - KONIEC" << endl;
            break;
        }
    }
    return 0;
}

