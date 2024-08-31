#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>

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
    int num_chunks = 4;
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

    // Combinar los datos leídos
    vector<char> combined_data;
    for (const auto& chunk : data) {
        combined_data.insert(combined_data.end(), chunk.begin(), chunk.end());
    }

    // Convertir los datos combinados a enteros
    vector<int> int_data = convert_to_ints(combined_data);

    return int_data;
}

int main() {
    vector<int> data = lecturaArchivo();

    // Imprimir los primeros 10 elementos leidos
    cout << "Primeros 10 elementos leidos: ";
    for (int i = 0; i < 10; ++i) {
        cout << data[i] << " ";
    }
    return 0;
}
