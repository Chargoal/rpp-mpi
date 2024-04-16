#include <stdio.h>
#include <iostream>
#include <math.h>
#include <mpi.h>
#include <string>
#include <chrono>
#include <fstream>
#include <vector>

using namespace std;

int main(int argc, char *argv[]){


    long long MOD = 1e9 + 7, p = 31;
    vector<long long> step;
    int rank, size;
    MPI_Init(&argc, &argv);

    // get number of processors
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // get rank of each processor
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // get the name of processor
    int  namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Get_processor_name(processor_name, &namelen);
    
   // printf("Process %d of %d on %s\n",rank, size,processor_name);

    if (argc != 2) {
        if (rank == 0) {
            cerr << "Usage: " << argv[0] << " <input_file>\n";
        }
        MPI_Finalize();
        return 1;
    }
    
    string filename = argv[1];
    string line;
    int sz;
    auto start = chrono::high_resolution_clock::now(); 
    if (rank == 0) {
        ifstream file(filename);
        if (file.is_open()) {
            getline(file, line);
            sz = line.length();
            file.close();
        } else {
            cerr << "Unable to open file " << filename << endl;
            MPI_Finalize();
            return 1;
        }
        start = chrono::high_resolution_clock::now();
    }
    
   

    MPI_Bcast(&MOD, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&p, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    MPI_Bcast(&sz, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    int sub_size = (sz+size - 1) / size;

    step.resize(sub_size  + 1);

    if (rank == 0) {
        step[0] = 1;
        for (int i = 1; i <= sub_size; ++i)
            step[i] = (step[i-1]*p) % MOD;
    }

    MPI_Bcast(step.data(), sub_size+1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    string sub_string(sub_size, ' ');

    MPI_Scatter(line.c_str(), sub_size, MPI_CHAR,
                (void*)sub_string.c_str(), sub_size, MPI_CHAR,
                0, MPI_COMM_WORLD);

     int x = max(0, sz - rank * sub_size);
    if (x < sub_size) sub_string = sub_string.substr(0, x);

   // cout << rank << " " << sub_string << " " << sub_string.length() << " " << x << "\n";
    

    long long hash = 0;
    vector<long long> hash_values(size);
    for (int i = 0; i < sub_string.length(); ++i)
        hash = (hash + ((sub_string[i] - 'a' + 1)*step[i] % MOD)) % MOD;
    
    MPI_Gather(&hash, 1, MPI_LONG_LONG, hash_values.data(), 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    if (rank == 0) {
       long long result = 0;
       long long _pow = 1;
       for (int i = 0; i < size; ++i){
            cout << "[" << i << " proccess]:: " << hash_values[i] << "\n";
            result = (result + (hash_values[i]*_pow % MOD)) % MOD;
            _pow = (_pow * step[sub_size]) % MOD;
       } 
      
       auto stop = chrono::high_resolution_clock::now();
       auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
       cout << "RESULT :: " << result << "\n";
       cout << "DURATION :: " << duration.count() << " ms\n";
    }

    MPI_Finalize();
}