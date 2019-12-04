#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    Mat image;
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

   cout <<  "Resolution :" <<image.size().width<<"x"<<image.size().height << std::endl ;
//================================================================================================
//================================================================================================
//============== version 1 pour calculer la matrice intrinseque par la formule ===================

	float distance_focale_x = image.size().width;
    float distance_focale_y = image.size().height;
    float centre_image_x = (image.size().width)/2;
    float centre_image_y = (image.size().height)/2;
   
   
    Mat matrice_intrinseque = (Mat_<double>(3,3) << distance_focale_x, 0, centre_image_x, 0, distance_focale_y, centre_image_y, 0, 0, 1);
    cout << "matrice_intrinseque = " << endl << " " << matrice_intrinseque << endl << endl;

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}
