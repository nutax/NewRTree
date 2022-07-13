#include <iostream>
#include <random>
#include <chrono>
#include "rtree.h"

#define WIDTH 800
#define HEIGHT 500
std::random_device rd;

void print_vector(const std::vector<double>& vec) {
    std::cout << "result" << "\n";
    for (auto& i : vec) {
        std::cout << i << "\n";
    }
}

void generate_random_poly_rtree(RTree* rtree) {
    double width_float = 0.01f + (double)rd() / (double)std::random_device::max() * 0.02f;
    double height_float = 0.01f + (double)rd() / (double)std::random_device::max() * 0.02f;

    auto width = (uint32_t)(width_float * WIDTH);
    auto height = (uint32_t)(height_float * HEIGHT);

    // generate a random point p1 inside the window
    Vec2 p1 = { (rd() % WIDTH), (rd() % HEIGHT) };
    // with the random point p1, generate the other 3 points p2, p3, p4
    Vec2 p2 = { p1.x + width, p1.y };
    Vec2 p3 = { p1.x + width, p1.y + height };
    Vec2 p4 = { p1.x, p1.y + height };

    rtree->insert({ p1, p2, p3, p4 });
    // min = minimo inferior izquierdo = p4
    // max = maximo superior derecho = p2
}


Vec2 generate_random_poly_rtree_search(RTree* rtree) {
    double width_float = 0.01f + (double)rd() / (double)std::random_device::max() * 0.02f;
    double height_float = 0.01f + (double)rd() / (double)std::random_device::max() * 0.02f;

    auto width = (uint32_t)(width_float * WIDTH);
    auto height = (uint32_t)(height_float * HEIGHT);

    // generate a random point p1 inside the window
    Vec2 p1 = { (rd() % WIDTH), (rd() % HEIGHT) };
    // with the random point p1, generate the other 3 points p2, p3, p4
    Vec2 p2 = { p1.x + width, p1.y };
    Vec2 p3 = { p1.x + width, p1.y + height };
    Vec2 p4 = { p1.x, p1.y + height };

    rtree->insert({ p1, p2, p3, p4 });
    return p1;
    // min = minimo inferior izquierdo = p4
    // max = maximo superior derecho = p2
}


std::vector<Vec2> generate_random_poly_rtree_to_delete(RTree* rtree) {
    double width_float = 0.01f + (double)rd() / (double)std::random_device::max() * 0.02f;
    double height_float = 0.01f + (double)rd() / (double)std::random_device::max() * 0.02f;

    auto width = (uint32_t)(width_float * WIDTH);
    auto height = (uint32_t)(height_float * HEIGHT);

    // generate a random point p1 inside the window
    Vec2 p1 = { (rd() % WIDTH), (rd() % HEIGHT) };
    // with the random point p1, generate the other 3 points p2, p3, p4
    Vec2 p2 = { p1.x + width, p1.y };
    Vec2 p3 = { p1.x + width, p1.y + height };
    Vec2 p4 = { p1.x, p1.y + height };

    // min = minimo inferior izquierdo = p4
    // max = maximo superior derecho = p2

    rtree->insert({ p1, p2, p3, p4 });
    return { p4, p2 };
}

std::vector<double> test_insert_rtree(int tests_cases, int inserts) {
    std::vector<std::vector<double>> total_times;
    for (int i = 0; i < tests_cases; i++) {
        std::vector<double> time_vector;
        auto rtree = new RTree();

        for (int j = 0; j < inserts; j++) {
            auto start = std::chrono::high_resolution_clock::now();
            for (int k = 0; k < 10; k++) {
                generate_random_poly_rtree(rtree);
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            time_vector.push_back(duration);
        }
        delete rtree;

        total_times.push_back(time_vector);
    }

    // get average
    std::vector<double> average_times;
    for (int i = 0; i < inserts; i++) {
        double sum = 0;
        for (int j = 0; j < tests_cases; j++) {
            sum += total_times.at(j).at(i);
        }
        average_times.push_back(sum / tests_cases);
    }

    return average_times;
}

std::vector<double> test_delete_rtree(int tests_cases, int deletes) {
    std::vector<std::vector<double>> total_times;

    for (int i = 0; i < tests_cases; i++) {
        std::vector<double> time_vector;
        auto rtree = new RTree();
        std::vector<std::vector<Vec2>> points_to_delete;

        for (int j = 0; j < deletes * 1000; j++) {
            auto points = generate_random_poly_rtree_to_delete(rtree);
            points_to_delete.push_back(points);
        }

        for (int j = 0; j < deletes; j++) {
            for (int k = 0; k < 1000; k++) {
                //auto point_to_delete = points_to_delete.at(k + 1000 * j);
                //auto start = std::chrono::high_resolution_clock::now();
                //rtree->eraseRandom();
                //auto end = std::chrono::high_resolution_clock::now();
                //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                //time_vector.push_back(duration);
            }
        }

        delete rtree;
        total_times.push_back(time_vector);
    }

    std::vector<double> total_items;
    for (int i = 0; i < deletes; i++) {
        for (int j = 0; j < tests_cases; j++) {
            total_items.push_back(total_times.at(j).at(i));
        }
    }

    return total_items;
}

std::vector<double> test_overlap_rtree(int tests_cases, int polygons) {
    std::vector<std::vector<double>> total_coefficients;

    for (int j = 0; j < tests_cases; j++) {
        std::vector<double> coefficients;
        std::vector<Vec2> random_points;
        for (int i = 0; i < polygons; i++) {
            Vec2 p1 = { (rd() % WIDTH), (rd() % HEIGHT) };
            random_points.push_back(p1);
        }
        auto rtree = new RTree();

        for (int i = 0; i < polygons; i++) {
            generate_random_poly_rtree(rtree);
            coefficients.push_back(rtree->testOverlapping(random_points));
        }
        delete rtree;
        total_coefficients.push_back(coefficients);
    }

    // get average
    std::vector<double> average_coefficients;
    for (int i = 0; i < polygons; i++) {
        double sum = 0;
        for (int j = 0; j < tests_cases; j++) {
            sum += total_coefficients.at(j).at(i);
        }
        average_coefficients.push_back(sum / tests_cases);
    }

    return average_coefficients;
}

std::vector<double> test_knn_rtree(int tests_cases, int inserts, int k) {
    std::vector<std::vector<double>> total_times;

    for (int i = 0; i < tests_cases; i++) {
        std::vector<double> local_times;
        auto rtree = new RTree();

        for (int j = 0; j < inserts; j++) {
            for (int k = 0; k < 1000; k++) {
                generate_random_poly_rtree(rtree);
            }

            auto start = std::chrono::high_resolution_clock::now();
            rtree->forEachNearest(k, { (static_cast<float>(rd() % WIDTH)), (static_cast<float>(rd() % HEIGHT)) }, [](Poly const&, Vec2, Vec2, float) {});
            auto end = std::chrono::high_resolution_clock::now();
            auto duration_k = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

            local_times.push_back(duration_k);
        }
        delete rtree;
        total_times.push_back(local_times);
    }

    std::vector<double> average_times;
    for (int i = 0; i < inserts; i++) {
        double sum = 0;
        for (int j = 0; j < tests_cases; j++) {
            sum += total_times.at(j).at(i);
        }
        average_times.push_back(sum / tests_cases);
    }

    return average_times;
}


std::vector<double> test_search_rtree(int tests_cases, int inserts) {
    std::vector<std::vector<double>> total_times;

    for (int i = 0; i < tests_cases; i++) {
        std::vector<double> local_times;
        auto rtree = new RTree();

        for (int j = 0; j < inserts; j++) {
            std::vector<Vec2> points_inserted;
            for (int k = 0; k < 1000; k++) {
                auto point_inserted = generate_random_poly_rtree_search(rtree);
                points_inserted.push_back(point_inserted);
            }
            auto start = std::chrono::high_resolution_clock::now();
            for (const auto& point_inserted : points_inserted) {
                rtree->forEachNearest(1, point_inserted, [](Poly const&, Vec2, Vec2, float) {});
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            local_times.push_back(duration);
        }
        delete rtree;
        total_times.push_back(local_times);
    }

    std::vector<double> total_items;
    for (int i = 0; i < inserts; i++) {
        for (int j = 0; j < tests_cases; j++) {
            total_items.push_back(total_times.at(j).at(i));
        }
    }

    return total_items;
}


int main() {
    //print_vector(test_delete_rtree(100, 5));
    //    print_vector(test_insert_rtree(100, 500));
    print_vector(test_overlap_rtree(1, 500));
    //    print_vector(test_search_rtree(100, 5));
    //    print_vector(test_knn_rtree(100, 5, 1));
    //    print_vector(test_knn_rtree(100, 5, 5));
    //    print_vector(test_knn_rtree(100, 5, 20));
}