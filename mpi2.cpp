#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) {
            cout << "Usage: mpiexec -n <number of processes> mpiSort.exe <input file name>\n";
        }
        MPI_Finalize();
        return 1;
    }

    string filename = argv[1];
    ifstream inputFile(filename);
    if (!inputFile) {
        if (rank == 0) {
            cout << "Error: Unable to open file " << filename << endl;
        }
        MPI_Finalize();
        return 1;
    }

    vector<int> numbers;
    int number;
    while (inputFile >> number) {
        numbers.push_back(number);
    }

    inputFile.close();

    // Distribute data among processes
    int localSize = numbers.size() / size;
    vector<int> localData(localSize);
    MPI_Scatter(numbers.data(), localSize, MPI_INT, localData.data(), localSize, MPI_INT, 0, MPI_COMM_WORLD);

    // Start timer
    auto start = high_resolution_clock::now();

    // Sort local data
    bubbleSort(localData);

    // Gather sorted data
    MPI_Gather(localData.data(), localSize, MPI_INT, numbers.data(), localSize, MPI_INT, 0, MPI_COMM_WORLD);

    // Final merge sort on root process
    if (rank == 0) {
        bubbleSort(numbers);
        // Stop timer
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);

        cout << "Sorting duration: " << duration.count() << " milliseconds" << endl;

        // Output sorted array to file
        ofstream outputFile("output.txt");
        if (!outputFile) {
            cout << "Error: Unable to create output file." << endl;
        } else {
            for (int num : numbers) {
                outputFile << num << " ";
            }
            outputFile.close();
        }
    }

    MPI_Finalize();
    return 0;
}
