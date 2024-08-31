#include <omp.h>

#pragma comment(lib, "libomp.lib")
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

void read_chunk(const string& filename, streampos start, streamsize size, vector<char>& buffer) {
    ifstream file(filename, ios::binary);
    if (file) {
        file.seekg(start);
        file.read(buffer.data(), size);
    }
}

// Convierte un vector de char a un vector de int
vector<int> convert_to_ints(const vector<char>& data) {
    vector<int> result(data.size() / sizeof(int));
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] = *reinterpret_cast<const int*>(&data[i * sizeof(int)]);
    }
    return result;
}

vector<int> lecturaArchivo() {
    string filename = "random_numbers.txt";
    
    // Obtener el tamaño total del archivo
    ifstream file(filename, ios::binary | ios::ate);
    streamsize file_size = file.tellg();
    file.close();

    // Número de partes en las que se dividirá el archivo
    int num_chunks = 32;
    streamsize chunk_size = file_size / num_chunks;

    // Vector para almacenar los datos leídos
    vector<vector<char>> data(num_chunks);

    // Capturar el tiempo inicial
    double start_time = omp_get_wtime();

    #pragma omp parallel for
    for (int i = 0; i < num_chunks; ++i) {
        streampos start = i * chunk_size;
        streamsize size = (i == num_chunks - 1) ? (file_size - start) : chunk_size;
        data[i].resize(size);
        read_chunk(filename, start, size, data[i]);
    }

    // Capturar el tiempo final
    double end_time = omp_get_wtime();

    // Calcular el tiempo transcurrido
    double time_taken = end_time - start_time;

    cout << "Tiempo transcurrido en la lectura: " << time_taken << " segundos" << endl;

    // Combinar los datos leídos
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
    int n1 = m - l + 1;
    int n2 = r - m;

    // Arrays temporales
    vector<int> L(n1), R(n2);

    // Copia datos a arrays temporales L[] y R[]
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

    // Copia los elementos restantes de L[], si hay alguno
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copia los elementos restantes de R[], si hay alguno
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// Función que implementa Merge Sort
void mergeSort(vector<int>& arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;

        // Umbral mínimo para paralelización
        int threshold = 1000000;  // Puedes ajustar este valor según el tamaño del array

        if ((r - l) > threshold) {
            #pragma omp parallel sections
            {
                #pragma omp section
                mergeSort(arr, l, m);
                #pragma omp section
                mergeSort(arr, m + 1, r);
            }
        } else {
            // No paralelizar si la sub-arreglo es pequeño
            mergeSort(arr, l, m);
            mergeSort(arr, m + 1, r);
        }

        merge(arr, l, m, r);
    }
}

int main() {
    vector<int> data = lecturaArchivo();
    int arr_size = data.size();

    double start_time = omp_get_wtime();
    mergeSort(data, 0, arr_size - 1);
    double end_time = omp_get_wtime();

    cout << "Tiempo transcurrido en el ordenamiento paralelo: " << end_time - start_time << " segundos" << endl;

    // // Guardar los números ordenados en un archivo .txt
    // ofstream outfile("sorted_numbers.txt");
    // if (outfile.is_open()) {
    //     for (size_t i = 0; i < data.size(); ++i) {
    //         outfile << data[i];
    //         if (i != data.size() - 1) {
    //             outfile << ",";
    //         }
    //     }
    //     outfile.close();
    //     cout << "Los números ordenados han sido guardados en sorted_numbers.txt" << endl;
    // } else {
    //     cerr << "No se pudo abrir el archivo para escribir." << endl;
    // }

    return 0;
}
