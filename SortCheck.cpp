#include <iostream>
#include <fstream>
#include <vector>

bool isSorted(const std::vector<int>& arr) {
    for (size_t i = 1; i < arr.size(); ++i) {
        if (arr[i] < arr[i - 1]) {
            return false;
        }
    }
    return true;
}

int main() {
    std::ifstream inputFile("output.txt");
    if (!inputFile) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }

    std::vector<int> numbers;
    int num;
    while (inputFile >> num) {
        numbers.push_back(num);
    }

    if (isSorted(numbers)) {
        std::cout << "The array is sorted." << std::endl;
    } else {
        std::cout << "The array is not sorted." << std::endl;
    }

    return 0;
}
