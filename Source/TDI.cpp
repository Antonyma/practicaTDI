#include <iostream>
#include <C_General.hpp>
#include <C_Trace.hpp>
#include <C_File.hpp>
#include <C_Arguments.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>

void aplicarCanny(C_Image imagen) {
	// Paso 1: Suavizar la imagen con un filtro gaussiano
	double desviacion = 1.0;
	C_Image imagenGaussiana(imagen.FirstRow(), imagen.LastRow(), imagen.FirstCol(), imagen.LastCol(), 0, 256);
	
	imagenGaussiana.Gaussian((float)desviacion); // Matriz gaussiana con derivacion aplicada
	for (int i = imagen.FirstRow(); i <= imagen.LastRow(); i++) {
		for (int j = imagen.FirstCol(); j <= imagen.LastCol(); j++) {
			if (i >= imagen.FirstRow() && i <= imagen.LastRow() && j >= imagen.FirstCol() && j <= imagen.LastCol()) {
				//cout << "i: " << i << ", j: " << j << endl;
				imagenGaussiana(i, j) = imagen(i, j);
			}
		}
	}
	
	// Paso 2: Calcular el gradiente de la imagen
	
	//m.Gradient();
	// Paso 3: Supresión no máxima

	// Paso 4: Umbralización con histéresis

	// Paso 5: Dibujar los bordes en la imagen resultante
	imagenGaussiana.WriteBMP("prueba.bmp"); //imagen negra, no filtra
} 


int Test(int argc, char** argv);
int main(int argc, char** argv){
	C_Image imagen;

	if (!C_FileExists("Hercules.bmp"))
		cout << "\nError, el fichero no existe"; //Compruebo si el nombre de la imagen existe y de no existir lanzo un error diciendo que esa imagen no existe
	else
	{
		imagen.ReadBMP("Hercules.bmp");
		//return Test(argc, argv);
		C_Image imagen2(imagen.FirstRow(), imagen.LastRow(), imagen.FirstCol(), imagen.LastCol());
		cout << "Dimensiones de la imagen: " << imagen.LastRow() - imagen.FirstRow() + 1 << " x " << imagen.LastCol() - imagen.FirstCol() + 1 << endl;
		imagen.Grey();
		aplicarCanny(imagen);
	}
}