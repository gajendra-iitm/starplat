#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/ch_graham_andrew.h>
#include <chrono>
#include <iostream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;

int main()
{
  CGAL::IO::set_ascii_mode(std::cin);
  CGAL::IO::set_ascii_mode(std::cout);

  std::istream_iterator<Point_2> in_start(std::cin);
  std::istream_iterator<Point_2> in_end;
  std::ostream_iterator<Point_2> out(std::cout, "\n");

  // Start timing
  auto start = std::chrono::high_resolution_clock::now();

  // Execute the convex hull algorithm
  CGAL::ch_graham_andrew(in_start, in_end, out);

  // Stop timing
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  // Output the time taken in milliseconds
  std::cerr << "Time taken: " << duration.count() << " ms\n";

  return 0;
}
