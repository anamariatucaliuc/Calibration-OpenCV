#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdio>

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>



#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

/** trier par rapport à la 2ème coordonnée */
bool sortByY(Point2d p1, Point2d p2) {
	return p1.y < p2.y;
}

/** on va trier les points 2D (point 2D qui représentent les 4 coins du labyrinthe qu'on a détecté) */
vector<Point2d> sortPoints(vector<Point2d> coord) {

	sort(coord.begin(), coord.end(), sortByY);

	if (coord[0].x > coord[1].x) swap(coord[0], coord[1]);
	if (coord[3].x > coord[2].x) swap(coord[2], coord[3]);

	return coord;
}

int main(int argc, char* argv[])
{
	// les constantes pour la correspondance des points 3D 
	static double dimX = 1.41428571429;	// racine de 2
	static double dimY = 1.00000000000;

	// declaration des 2 vecteurs : le vecteur de rotation(rvec -> 3*1) et le vecteur de translation(tvec -> 3 x 1)
	cv::Mat rvec(3,1,cv::DataType<double>::type);
	cv::Mat tvec(3,1,cv::DataType<double>::type);

	// le résultat qu'on attend (matrice de rotation 3*3 qu'on obtient après avoir appliqué Rodrigues)
	cv::Mat Matrice_rotation(3,3,cv::DataType<double>::type);
	// Pour déterminer les paramètres extrinsèques on a besoin des paramètres intrinsèques et des coefficients de distorsion
	// les paramètres intrinsèques déterminés avec le chessboard avant
	Mat cameraMatrix = Mat(3, 3, CV_64FC1); // cameraMatrix c'est la matrice intrinsèque (3 x 3)
	/* Matrice_intrinseque =	[ fx, 0, cx
					0, fy, cy
					0, 0, 1] où fx,fy sont les distances focales et cx,cy le centre de l'image en x et y */
	cameraMatrix.at<double>(0, 0) = 5.8416593244319370e+02;	// fx
	cameraMatrix.at<double>(0, 1) = 0.0f;
	cameraMatrix.at<double>(0, 2) = 320.;			// cx

	cameraMatrix.at<double>(1, 0) = 0.0f;
	cameraMatrix.at<double>(1, 1) = 5.8416593244319370e+02;	// fy
	cameraMatrix.at<double>(1, 2) = 240.;			// cy

	cameraMatrix.at<double>(2, 0) = 0.0f;
	cameraMatrix.at<double>(2, 1) = 0.0f;
	cameraMatrix.at<double>(2, 2) = 1.0f;


	// les coefficients de distorsion qu'on a aussi déterminé avec le chessboard
	Mat distCoeffs = Mat(5, 1, CV_64FC1);		// la matrice avec les coefficients de distorsion (5*1)

	distCoeffs.at<double>(0, 0) = -3.4991412075307704e-02;
	distCoeffs.at<double>(0, 1) = -2.1674621756008237e-02;
	distCoeffs.at<double>(0, 2) = 0.;
	distCoeffs.at<double>(0, 3) = 0.;
	distCoeffs.at<double>(0, 4) = -1.8628392761446691e-02;
	
	// les 4 coins du labyrinthe, on a 4 points qui ont des coordonnées 2D qu'on a réussi à détecter
   	vector<Point2d> points_labyrinthe2d;
	points_labyrinthe2d.push_back(Point2d(484,366));
	points_labyrinthe2d.push_back(Point2d(138,136));
	points_labyrinthe2d.push_back(Point2d(502,124));
	points_labyrinthe2d.push_back(Point2d(120,378));
	
	// on va trier ces 4 points pour les avoir en bonne ordre
	sortPoints(points_labyrinthe2d);
	
	// on prend les points des 4 coins de l'image et on les affiche de façon triée 
	for (int i = 0; i < points_labyrinthe2d.size(); i++)
		cout << points_labyrinthe2d[i] << endl;

	// points 3D les points correspondants au 4 points du points_labyrinthe2d 
	vector<Point3f> correspondance3d;	
	// pour que le centre du rectangle soit à l'origine (0,0,0) 
	// si on a (w,h) la taille on prends >> (-w/2,-h/2,0); (w/2,-h/2,0); (-w/2,h/2,0) et (w/2,h/2,0); h = 1 et w = racine(2) car l'image représente un rectangle et pas un carré  	
	correspondance3d.push_back(Point3f(-dimX/2,-1/2,0)); 
	correspondance3d.push_back(Point3f(dimX/2, -1/2, 0));
	correspondance3d.push_back(Point3f(-dimX/2, 1/2, 0));
	correspondance3d.push_back(Point3f(dimX/2, 1/2, 0));


	// on calcule rvecs and tvecs à partir de la fonction OpenCV - solvePnP() à partir de la matrice intrinsèque(cameraMatrix) + les coefficients de distorsion(distCoeffs) + les 4 points 2Ds détectés(points_labyrinthe2d) et leur correspondance en 3D(correspondance3d) qui sont dans l'ordre pour avoir la bonne correspondance 2D <-> 3D
	solvePnP(correspondance3d, points_labyrinthe2d, cameraMatrix, distCoeffs, rvec, tvec, false, CV_ITERATIVE);
	
	// on affiche le résultat (rvec et tvec)
	cout << "vecteur rotation (3 x 1): " << rvec<< endl;
	cout << "vecteur translation (3 x 1): " << tvec << endl;
	
	// on applique la fonction Rodrigues() d'OpenCV pour obtenir une matrice de rotation(Matrice_rotation) de dimensions 3 x 3 
	Rodrigues(rvec,Matrice_rotation);
	
	// un 1er affichage >>> Matrice_rotation est une matrice 3 x 3 	
	for(int i=0; i<3; i++){
		for(int j=0; j<3; j++)
        		std::cout << Matrice_rotation.at<double>(i,j) << " ";
    		std::cout << endl;
	}
	// un 2ème affichage pour la matrice de rotation(Matrice_rotation) - ici on a plus de décimales
	cout << "affichage matrice 3x3 : " << Matrice_rotation << endl;
	
	//pour le calcul des angles d'Euler
	Mat Angles_Euler(3, 1, CV_64F);
	// on rècupere les 9 valeurs(coefficients) de la matrice de rotation
    	double coeff1 = Matrice_rotation.at<double>(0, 0);
    	double coeff2 = Matrice_rotation.at<double>(0, 1);
	double coeff3 = Matrice_rotation.at<double>(0, 2);
    	double coeff4 = Matrice_rotation.at<double>(1, 0);
    	double coeff5 = Matrice_rotation.at<double>(1, 1);
    	double coeff6 = Matrice_rotation.at<double>(1, 2);
    	double coeff7 = Matrice_rotation.at<double>(2, 0);
    	double coeff8 = Matrice_rotation.at<double>(2, 1);
	double coeff9 = Matrice_rotation.at<double>(2, 2);

    	double theta, psi, phi;

    	// supposant que les angles sont en radians on applique les formules
    	if (coeff4 > 0.998) { // cas 1
         	phi = 0;
         	theta = CV_PI / 2;
         	psi = atan2(coeff3, coeff9);
    	}
    	else if (coeff4 < -0.998) { // cas -1
        	phi = 0;
        	theta = -CV_PI / 2;
        	psi = atan2(coeff3, coeff9);
    	}
    	else	// sinon 
    	{
       		phi = atan2(-coeff6, coeff5);
        	theta = asin(coeff4);
        	psi = atan2(-coeff7, coeff1);
   	}
	// conversion radians -> degrés
    	Angles_Euler.at<double>(0) = phi* 180 / CV_PI;
    	Angles_Euler.at<double>(1) = theta* 180 / CV_PI;
    	Angles_Euler.at<double>(2) = psi* 180 / CV_PI;

    cout << "les angles d'euler sont: " << Angles_Euler << endl;
    
    // on détermine la position de la caméra OpenCV
    // article : http://ksimek.github.io/2012/08/22/extrinsic/?fbclid=IwAR3vBlwUFRkRVA0Mcv4nTHg40ELWyfaZh4HkCwEI4YLNYMf7FdB5GA4LJVs
    cv::Mat R(3,3,cv::DataType<double>::type);
    cv::Mat Camera_OpenGL(4,4,cv::DataType<double>::type);
    
    // inverse([R|tvec]) = [R'|-R'*tvec], où R' est la transposé de R (Matrice_rotation)
    R = Matrice_rotation.t();     // l'inverse d'une matrice de rotation est sa transposé 
    tvec = -R * tvec; 	  	  // pour tvec

    cv::Mat Camera_position = cv::Mat::eye(4, 4, R.type()); // Camera_position c'est une matrice  4 x 4
    Camera_position( cv::Range(0,3), cv::Range(0,3) ) = R * 1; // on copie R dans la matrice Camera_position
    Camera_position( cv::Range(0,3), cv::Range(3,4) ) = tvec * 1; // on copie tvec dans la matrice Camera_position

    // on affiche la matrice Camera_position(matrice 4 x 4) on a la pose de la caméra dans le cadre objet(labyrinthe) 
    cout << "position de la caméra" << Camera_position << endl;
    // pour OpenGL les axes sont différents
    // on va appliquer une rotation autour de l'axe X de 180 degrés
    cv::Mat RotX = (cv::Mat_<double>(4,4) << 1,0,0,0,		// matrice 4 x 4
				     		0,-1,0,0,
				     		0,0,-1,0,
				     		0,0,0,1);  
    // on l'affiche 
    cout << "matrice rotation 4x4 autour de l'axe X avec 180 degrés" << RotX << endl;
	
    Camera_OpenGL = Camera_position * RotX; // OpenGL( 4 x 4) avec la pose de la caméra
    cout << "matrice 4x4 matrice OpenGL - caméra position" << Camera_OpenGL << endl; // la dernière colonne de ce matrice contient la translation selon les axes on peut déterminer la profondeur après pour la mise à l'échelle selon le mouvement du labyrinthe 
	
    return 0;
}
