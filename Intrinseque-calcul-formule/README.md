##### ==============================================================
## Test sur  le calcul de la matrice intrinseque avec la formule vue 
### ======================================================== matrice 3x3
### Matrice_intrinseque =
###   			[ fx, 0, cx
###				0, fy, cy
### 				0, 0, 1]
#### avec image qui est l'image en entrée (test.jpg)
#### fx = image.size().width;
#### fy = image.size().height;
#### cx = (image.size().width)/2;
#### cy = (image.size().height)/2;


#### résultat pour l'image test.jpg on a : 
./test.out test.jpg >>>
 
Resolution :1280x720
matrice_intrinseque = 
 [1280, 0, 640;
 0, 720, 360;
 0, 0, 1]

