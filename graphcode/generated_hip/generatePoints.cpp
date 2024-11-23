#include <iostream>
#include <vector>
#include <random>
#include <fstream> // To handle file output
#include <utility> // For std::pair

// Function to generate n random floating-point coordinate points
std::vector<std::pair<double, double>> generateRandomPoints(int n, double lowerBound, double upperBound) {
    std::vector<std::pair<double, double>> points;
    points.reserve(n); // Reserve space for efficiency

    // Initialize random number generation
    std::random_device rd;                        // Obtain a random number from hardware
    std::mt19937 gen(rd());                       // Seed the generator
    std::uniform_real_distribution<> dist(lowerBound, upperBound); // Define the range

    for (int i = 0; i < n; ++i) {
        double x = dist(gen);
        double y = dist(gen);
        points.emplace_back(x, y); // Add the point to the vector
    }

    return points;
}

int main() {
    int n; // Number of points to generate
    std::cout << "Enter the number of points to generate: ";
    std::cin >> n;

    if(n <= 0){
        std::cerr << "Number of points must be positive!" << std::endl;
        return 1;
    }

    std::cout << "Generating " << n << " points..." << std::endl;

    // Define the range for x and y coordinates
    double lowerBound = 0.0, upperBound = 400.0; // Adjust range as needed

    // Generate n random points
    std::vector<std::pair<double, double>> randomPoints = generateRandomPoints(n, lowerBound, upperBound);

    // Output the generated points to a text file
    std::ofstream outFile("testcase.txt"); // Open the output file
    if (!outFile) {
        std::cerr << "Error opening file for writing!" << std::endl;
        return 1;
    }

    outFile << n << std::endl; // Write the number of points
    for (const auto& point : randomPoints) {
        outFile << point.first << " " << point.second << std::endl; // Write each point
    }

    outFile.close(); // Close the file after writing
    std::cout << "Points successfully written to testcase.txt" << std::endl;

    return 0;
}
