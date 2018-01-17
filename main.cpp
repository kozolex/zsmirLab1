#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

int main()
{
    VideoCapture cap(0); //Klasa do przechwytywania wideo z plików wideo, kamer itp.
    if ( !cap.isOpened() )  // jeżeli nie uzyskasz połączenia
    {
        cout << "Nie moge polaczyc sie z kamera / wideo" << endl;
        return -1;
    }
    Mat imgTmp;
    cap.read(imgTmp);
    while (true)
    {
        Mat imgOriginal;
        bool bSuccess = cap.read(imgOriginal);
        if (!bSuccess)
        {
            cout << "Nie mozna odczytac strumienia " << endl;
            break;
        }
        imshow("Kamera RGB", imgOriginal);
        if (waitKey(30) == 27) //czekaj 30ms na wciśnięcie esc jeżeli zostanie wciśnięty przerwij pętle
        {
            cout << "Klawisz esc - KONIEC" << endl;
            break;
        }
    }
    return 0;
}
