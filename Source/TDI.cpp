#include <iostream>
#include <C_File.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>

void suprimirNoMaximos(C_Image& imagenBordes, C_Matrix& matrizGaussiana, C_Image& imagenSuprimida) {
    // Excluye los p�xeles de los bordes para asegurar que los p�xeles que se comparan tengan vecinos en todas las direcciones
    // Bucle comienza en FirstRow()+1 y FirstCol()+1 y termina en LastRow()-1 y LastCol()-1
    for (int i = imagenBordes.FirstRow() + 1; i < imagenBordes.LastRow() - 1; i++) {
        for (int j = imagenBordes.FirstCol() + 1; j < imagenBordes.LastCol() - 1; j++) {
            double theta = matrizGaussiana(i, j);

            double valorDirGrad, valorDirOpGrad, valorActual;

            // Direcci�n horizontal
            if ((theta > -22.5 && theta <= 22.5) || (theta > 157.5 && theta <= -157.5)) {
                valorDirGrad = imagenBordes(i, j + 1);
                valorDirOpGrad = imagenBordes(i, j - 1);
            }
            // Direcci�n diagonal superior derecha e inferior izquierda
            else if ((theta > 22.5 && theta <= 67.5) || (theta > -157.5 && theta <= -112.5)) {
                valorDirGrad = imagenBordes(i + 1, j - 1);
                valorDirOpGrad = imagenBordes(i - 1, j + 1);
            }
            // Direcci�n vertical
            else if ((theta > 67.5 && theta <= 112.5) || (theta > -112.5 && theta <= -67.5)) {
                valorDirGrad = imagenBordes(i + 1, j);
                valorDirOpGrad = imagenBordes(i - 1, j);
            }
            //Direcci�n diagonal superior izquierda e inferior derecha
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

void umbralizacionConHist(C_Image& imagenSuprimida, int& umbralMin, int& umbralMax) {
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
                         
                        // Comprueba que (x, y) estan dentro de los l�mites de la imagen y si su valor es mayor que el umbralMax
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

void aplicarCanny(C_Image & imagen) {
    double desviacion;
    C_Matrix matrizImagen(imagen), matrizGaussiana(-1, 1, -1, 1);

    // Suavizar la imagen con un filtro gaussiano
    cout << "\nPaso 2: Suavizar imagen con filtro gaussiano (Valor recomendado entre 0.5 - 1.50)";
    std::cout << "\nIntroduce el valor de la desviacion: ";
    std::cin >> desviacion;

    matrizGaussiana.Gaussian((float)desviacion);
    matrizImagen.Convolution(matrizImagen, matrizGaussiana);
    C_Image imagenGaussiana(matrizImagen, 256);

    imagenGaussiana.WriteBMP("test2_FiltroGaussiano.bmp");

    cout << "\nImagen guardada con nombre test2_FiltroGaussiano" << endl;

    // Calcular el gradiente de la imagen
    // Guardas la imagen en la matriz Gaussiana 
    C_Image imagenBordes;

    cout << "\nPaso 3: Calculo de bordes utilizando el gradiente";

    imagenBordes.Gradient(matrizImagen);
    imagenBordes.WriteBMP("test3_Bordes.bmp");

    cout << "\nImagen guardada con nombre test3_Bordes" << endl;

    // Paso 3: Supresion no maxima
    C_Image imagenSuprimida(imagenBordes);

    cout << "\nPaso 4: Supresion de no maximos de la imagen";

    suprimirNoMaximos(imagenBordes, matrizImagen, imagenSuprimida);
    imagenSuprimida.WriteBMP("test4_SupresionNoMaximo.bmp");
    
    cout << "\nImagen guardada con nombre test4_SupresionNoMaximo" << endl;

    // Paso 4: Umbralizacion con histeresis    
    int umbralMin, umbralMax;

    cout << "\nPaso 5: Umbralizaci�n con histeresis, se reccomienda empezar con valores bajos y ajustarlos gradualmente hasta obtener un resultado satisfactorio";
    std::cout << "\nIntroduce el valor de umbral minimo (Ej: 10): ";
    std::cin >> umbralMin;
    std::cout << "\nIntroduce el valor de umbral maximo (Ej: 30): ";
    std::cin >> umbralMax;

    C_Image imagenBordesFinales(imagenSuprimida);
    umbralizacionConHist(imagenBordesFinales, umbralMin, umbralMax);
    imagenBordesFinales.WriteBMP("test5_Umbralizado.bmp");

    cout << "\nImagen guardada con nombre test5_Umbralizado" << endl;
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
        cout << "\nPaso 1: Convertir a escala de gris de 256 niveles";
        imagen.WriteBMP("test1_Gris.bmp");
        cout << "\nImagen guardada con nombre test1_Gris" << endl;
        aplicarCanny(imagen);
    }
}
