#include <iostream>
#include <C_General.hpp>
#include <C_Trace.hpp>
#include <C_File.hpp>
#include <C_Arguments.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>
#include "TDI.h"

void suprimirNoMaximos(C_Image& imagenBordes, C_Matrix& matrizGaussiana, C_Image& imagenSuprimida) {
    // Excluye los píxeles de los bordes para asegurar que los píxeles que se comparan tengan vecinos en todas las direcciones
    // Bucle comienza en FirstRow()+1 y FirstCol()+1 y termina en LastRow()-1 y LastCol()-1
    for (int i = imagenBordes.FirstRow() + 1; i < imagenBordes.LastRow() - 1; i++) {
        for (int j = imagenBordes.FirstCol() + 1; j < imagenBordes.LastCol() - 1; j++) {
            double theta = matrizGaussiana(i, j);

            double valorDirGrad, valorDirOpGrad, valorActual;

            // Dirección horizontal
            if ((theta > -22.5 && theta <= 22.5) || (theta > 157.5 && theta <= -157.5)) {
                valorDirGrad = imagenBordes(i, j + 1);
                valorDirOpGrad = imagenBordes(i, j - 1);
            }
            // Dirección diagonal superior derecha e inferior izquierda
            else if ((theta > 22.5 && theta <= 67.5) || (theta > -157.5 && theta <= -112.5)) {
                valorDirGrad = imagenBordes(i + 1, j - 1);
                valorDirOpGrad = imagenBordes(i - 1, j + 1);
            }
            // Dirección vertical
            else if ((theta > 67.5 && theta <= 112.5) || (theta > -112.5 && theta <= -67.5)) {
                valorDirGrad = imagenBordes(i + 1, j);
                valorDirOpGrad = imagenBordes(i - 1, j);
            }
            //Dirección diagonal superior izquierda e inferior derecha
            else {
                valorDirGrad = imagenBordes(i - 1, j - 1);
                valorDirOpGrad = imagenBordes(i + 1, j + 1);
            }

            valorActual = imagenBordes(i, j);

            //Comprueba que sea maximo local
            if (valorActual > valorDirGrad && valorActual > valorDirOpGrad) {
                imagenSuprimida(i, j) = valorActual;
            }
            else {
                imagenSuprimida(i, j) = 0;
            }
        }
    }
}

void umbralizacionConHist(C_Image& imagenSuprimida, int umbralMin, int umbralMax) {
    for (int i = imagenSuprimida.FirstRow(); i <= imagenSuprimida.LastRow(); i++) {
        for (int j = imagenSuprimida.FirstCol(); j <= imagenSuprimida.LastCol(); j++) {
            int valorPixel = (int)imagenSuprimida(i, j);

            // Valor mayor o igual que umbralMax
            if (valorPixel >= umbralMax) {
                imagenSuprimida(i, j) = 255;
            }
            // Valor menor que umbralMin
            else if (valorPixel < umbralMin) {
                imagenSuprimida(i, j) = 0;
            }
            // Valor entre ambos umbrales
            else {
                bool pixelVecinoMayor = false;
                // Recorrido de 3x3
                for (int x = i - 1; x <= i + 1; x++) {
                    for (int y = j - 1; y <= j + 1; y++) {
                         
                        // Comprueba que (x, y) estan dentro de los límites de la imagen y si su valor es mayor que el umbralMax
                        if (x >= imagenSuprimida.FirstRow() && x <= imagenSuprimida.LastRow() && y >= imagenSuprimida.FirstCol() && y <= imagenSuprimida.LastCol() && imagenSuprimida(x, y) > umbralMax) {
                            pixelVecinoMayor = true;
                            break;
                        }
                    }
                    if (pixelVecinoMayor) break;
                }
                // pixelVecinoMayor = true => es borde
                if (pixelVecinoMayor) {
                    imagenSuprimida(i, j) = 255;
                }
                else {
                    imagenSuprimida(i, j) = 0;
                }
            }
        }
    }
}

void aplicarCanny(C_Image imagen) {
    double desviacion = 0.25;
    C_Image imagenGaussiana(imagen);
    
    // Paso 1: Suavizar la imagen con un filtro gaussiano
    imagenGaussiana.Gaussian((float)desviacion); 

    // Recorre la imagen y guarda el valor de cada pixel en la imagen Gaussiana 
    for (int i = imagen.FirstRow(); i <= imagen.LastRow(); i++) {
        for (int j = imagen.FirstCol(); j <= imagen.LastCol(); j++) {
            imagenGaussiana(i, j) = imagen(i, j);
        }
    }

    // Paso 2: Calcular el gradiente de la imagen
    // Guardas la matriz Gaussiana en la imagen 
    C_Image imagenBordes(imagenGaussiana);
    C_Matrix matrizGaussiana(imagenGaussiana);

    imagenBordes.Gradient(matrizGaussiana);

    // Paso 3: Supresión no máxima
    C_Image imagenSuprimida(imagenBordes);
    suprimirNoMaximos(imagenBordes, matrizGaussiana, imagenSuprimida);
    

    // Paso 4: Umbralización con histéresis    
    int umbralMin = 30;
    int umbralMax = 120;
    C_Image imagenBordesFinales(imagenSuprimida);
    umbralizacionConHist(imagenSuprimida, umbralMin, umbralMax);
    
    // Paso 5: Dibujar los bordes en la imagen resultante
    imagenGaussiana.WriteBMP("prueba.bmp");
    imagenBordes.WriteBMP("prueba2.bmp");
    imagenSuprimida.WriteBMP("prueba3.bmp");
    imagenBordesFinales.WriteBMP("prueba4.bmp");
}

int main(int argc, char** argv) {
    C_Image imagen;

    std::string nombreImagen;
    std::cout << "Ingrese el nombre de la imagen de entrada: ";
    std::cin >> nombreImagen;

    nombreImagen += ".bmp";

    if (!C_FileExists(nombreImagen.c_str()))
        cout << "\nError, el fichero no existe"; //Compruebo si el nombre de la imagen existe 
    else
    {
        imagen.ReadBMP(nombreImagen.c_str());
        cout << "Dimensiones de la imagen: " << imagen.LastRow() - imagen.FirstRow() + 1 << " x " << imagen.LastCol() - imagen.FirstCol() + 1 << endl;
        imagen.Grey();
        aplicarCanny(imagen);
    }
}
