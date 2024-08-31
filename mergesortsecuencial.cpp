#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace std;

// Función para leer un archivo en chunks
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

// Función para leer el archivo completo
vector<int> lecturaArchivoSecuencial() {
    string filename = "random_numbers.txt";
    
    // Obtener el tamaño total del archivo
    ifstream file(filename, ios::binary | ios::ate);
    streamsize file_size = file.tellg();
    file.close();

    // Leer el archivo completo
    auto start_read = chrono::high_resolution_clock::now(); // Medir el tiempo de inicio de lectura
    vector<char> combined_data(file_size);
    read_chunk(filename, 0, file_size, combined_data);
    auto end_read = chrono::high_resolution_clock::now(); // Medir el tiempo de finalización de lectura

    // Convertir los datos leídos a enteros
    vector<int> int_data = convert_to_ints(combined_data);

    // Calcular el tiempo transcurrido en la lectura
    chrono::duration<double> read_time = end_read - start_read;
    cout << "Tiempo transcurrido en la lectura del archivo: " << read_time.count() << " segundos" << endl;

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
void mergeSortSecuencial(vector<int>& arr, int l, int r) {
    if (l < r) {
        // Encuentra el punto medio
        int m = l + (r - l) / 2;

        // Divide y ordena cada mitad
        mergeSortSecuencial(arr, l, m);
        mergeSortSecuencial(arr, m + 1, r);

        // Combina las mitades ordenadas
        merge(arr, l, m, r);
    }
}

// Función para imprimir el array
void printArray(const vector<int>& A) {
    for (int num : A)
        cout << num << " ";
    cout << endl;
}

int main() {
    // Leer el archivo
    auto start_read = chrono::high_resolution_clock::now(); // Medir el tiempo de inicio de lectura
    vector<int> data = lecturaArchivoSecuencial();
    auto end_read = chrono::high_resolution_clock::now(); // Medir el tiempo de finalización de lectura
    int arr_size = data.size();

    cout << "Datos leídos: " << arr_size << endl;
    cout << "Leidos en un tiempo de: " << chrono::duration<double>(end_read - start_read).count() << " segundos" << endl;


    // Medir el tiempo de ejecución de Merge Sort
    auto start_time = chrono::high_resolution_clock::now();
    mergeSortSecuencial(data, 0, arr_size - 1);
    auto end_time = chrono::high_resolution_clock::now();

    // Calcular el tiempo transcurrido
    chrono::duration<double> time_taken = end_time - start_time;
    cout << "Tiempo transcurrido en Merge Sort: " << time_taken.count() << " segundos" << endl;

    

    return 0;
}
