#include <iostream>
#include <C_General.hpp>
#include <C_Trace.hpp>
#include <C_File.hpp>
#include <C_Arguments.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>

void aplicarCanny(C_Image imagen) {
    double desviacion = 1.0;
    C_Image imagenGaussiana(imagen);
    
    // Paso 1: Suavizar la imagen con un filtro gaussiano
    imagenGaussiana.Gaussian((float)desviacion); 

    for (int i = imagen.FirstRow(); i <= imagen.LastRow(); i++) {
        for (int j = imagen.FirstCol(); j <= imagen.LastCol(); j++) {
            imagenGaussiana(i, j) = imagen(i, j);
        }
    }

    // Paso 2: Calcular el gradiente de la imagen
    C_Image imagenBordes(imagenGaussiana);
    C_Matrix matrizGaussiana(imagenGaussiana);

    imagenBordes.Gradient(imagenGaussiana);

    // Paso 3: Supresión no máxima

    C_Image imagenSuprimida(imagenBordes);
    

    // Paso 4: Umbralización con histéresis

    // Paso 5: Dibujar los bordes en la imagen resultante
    imagenGaussiana.WriteBMP("prueba.bmp");
    imagenBordes.WriteBMP("prueba2.bmp");
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
