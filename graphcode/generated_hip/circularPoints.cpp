#include <iostream>
#include <cmath>
#include <vector>
#include <fstream> // Include for file operations

// Define a structure for a 2D point
struct Point {
    double x, y; // Use double coordinates for floating-point precision
};

// Function to generate the points of a polygon with floating-point coordinates
std::vector<Point> generatePolygonVertices(int n, double radius = 100.0) {
    std::vector<Point> points;
    for (int i = 0; i < n; ++i) {
        double angle = 2.0 * M_PI * i / n; // Calculate the angle in radians
        double x = radius * cos(angle);    // Use floating-point values for x
        double y = radius * sin(angle);    // Use floating-point values for y
        points.push_back({x, y});          // Add the point to the vector
    }
    return points;
}

int main() {
    int n; // Number of sides (vertices)
    std::cin>>n;
    double radius = 100.0; // Radius of the circle
    std::vector<Point> polygonPoints = generatePolygonVertices(n, radius);

    // Open the file to write the points
    std::ofstream outfile("testcase.txt");

    // Check if the file was opened successfully
    if (outfile.is_open()) {
        outfile << polygonPoints.size() << std::endl; // Write the number of points
        for (const auto& point : polygonPoints) {
            outfile << point.x << " " << point.y << std::endl; // Write each point to the file
        }
        outfile.close(); // Close the file after writing
        std::cout << "Points written to testcase.txt" << std::endl;
    } else {
        std::cerr << "Error opening file!" << std::endl;
    }

    return 0;
}
