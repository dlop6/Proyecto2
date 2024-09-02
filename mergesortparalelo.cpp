#include <omp.h>  // Biblioteca para paralelización en OpenMP

#pragma comment(lib, "libomp.lib")  // Vincula la librería OpenMP
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

// Función para leer una porción de un archivo binario
void read_chunk(const string& filename, streampos start, streamsize size, vector<char>& buffer) {
    ifstream file(filename, ios::binary);  // Abre el archivo en modo binario
    if (file) {
        file.seekg(start);  // Posiciona el puntero de lectura en 'start'
        file.read(buffer.data(), size);  // Lee 'size' bytes en el buffer
    }
}

// Convierte un vector de char a un vector de enteros
vector<int> convert_to_ints(const vector<char>& data) {
    vector<int> result(data.size() / sizeof(int));  // Crea un vector del tamaño adecuado para enteros
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] = *reinterpret_cast<const int*>(&data[i * sizeof(int)]);  // Convierte los bytes a enteros
    }
    return result;
}

// Función para leer un archivo en paralelo y convertir los datos a enteros
vector<int> lecturaArchivo() {
    string filename = "random_numbers.txt";  // Nombre del archivo a leer
    
    // Obtener el tamaño total del archivo
    ifstream file(filename, ios::binary | ios::ate);  // Abre el archivo y posiciona el puntero al final
    streamsize file_size = file.tellg();  // Obtiene el tamaño del archivo
    file.close();  // Cierra el archivo

    // Número de partes en las que se dividirá el archivo
    int num_chunks = 32;
    streamsize chunk_size = file_size / num_chunks;  // Tamaño de cada porción

    // Vector para almacenar las porciones leídas
    vector<vector<char>> data(num_chunks);

    // Capturar el tiempo inicial de lectura
    double start_time = omp_get_wtime();

    // Lectura en paralelo de las porciones del archivo
    #pragma omp parallel for
    for (int i = 0; i < num_chunks; ++i) {
        streampos start = i * chunk_size;  // Inicio de la porción
        streamsize size = (i == num_chunks - 1) ? (file_size - start) : chunk_size;  // Última porción puede ser más pequeña
        data[i].resize(size);  // Redimensiona el buffer
        read_chunk(filename, start, size, data[i]);  // Lee la porción
    }

    // Capturar el tiempo final de lectura
    double end_time = omp_get_wtime();

    // Calcular el tiempo transcurrido
    double time_taken = end_time - start_time;
    cout << "Tiempo transcurrido en la lectura: " << time_taken << " segundos" << endl;

    // Combinar las porciones leídas en un solo vector
    vector<char> combined_data;
    for (const auto& chunk : data) {
        combined_data.insert(combined_data.end(), chunk.begin(), chunk.end());
    }

    // Convertir los datos combinados a enteros
    vector<int> int_data = convert_to_ints(combined_data);

    return int_data;
}

// Función para fusionar dos subarrays
void merge(vector<int>& arr, int l, int m, int r) {
    int n1 = m - l + 1;  // Tamaño del primer subarray
    int n2 = r - m;  // Tamaño del segundo subarray

    // Arrays temporales
    vector<int> L(n1), R(n2);

    // Copia los datos a los arrays temporales
    for (int i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    // Fusionar los arrays temporales en arr[l..r]
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copiar los elementos restantes de L[], si hay alguno
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copiar los elementos restantes de R[], si hay alguno
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// Función que implementa el algoritmo de Merge Sort
void mergeSort(vector<int>& arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;  // Encuentra el punto medio

        // Umbral mínimo para paralelización
        int threshold = 1000000;  // Se puede ajustar este valor

        // Si el tamaño del subarray es grande, paralelizar
        if ((r - l) > threshold) {
            #pragma omp parallel sections
            {
                #pragma omp section
                mergeSort(arr, l, m);  // Ordenar la primera mitad en paralelo
                #pragma omp section
                mergeSort(arr, m + 1, r);  // Ordenar la segunda mitad en paralelo
            }
        } else {
            // Si es pequeño, no paralelizar
            mergeSort(arr, l, m);
            mergeSort(arr, m + 1, r);
        }

        merge(arr, l, m, r);  // Fusionar los subarrays
    }
}

int main() {
    vector<int> data = lecturaArchivo();  // Leer el archivo y obtener los datos
    int arr_size = data.size();  // Tamaño del array

    // Medir el tiempo del ordenamiento
    double start_time = omp_get_wtime();
    mergeSort(data, 0, arr_size - 1);
    double end_time = omp_get_wtime();

    // Mostrar tamaño de los datos y tiempo de ordenamiento
    cout << "Size: " << data.size() << endl;
    cout << "Tiempo transcurrido en el ordenamiento paralelo: " << end_time - start_time << " segundos" << endl;

    return 0;
}
