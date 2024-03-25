#include "pool.h"
#include <iostream>
#include <chrono>

constexpr unsigned long long N = 35000000;  //  35000000
constexpr unsigned long long K = 11000000;  //  11000000
constexpr unsigned long long L = 24000000;  // 24000000
constexpr unsigned long long M = 4000000;  //  4000000
constexpr unsigned int MAX_LITR = 500;



int main(void) {

    auto start = std::chrono::high_resolution_clock::now();

    std::cout << "Начало работы..." << std::endl;

    GraphPool net(N, MAX_LITR);

    net.FillPools(N);
    net.ShowLitres();
    std::cout << "Заполнение бассейнов..." << std::endl;

    net.ConnectPools(K);
    net.ShowLitres();
    std::cout << "Соединение бассейнов..." << std::endl;

    net.FillPools(L);
    net.ShowLitres();
    std::cout << "Заполнение бассейнов..." << std::endl;

    net.UnconnectPools(M);
    net.FillPools(L);
    net.ShowLitres();
    std::cout << "Разъединение и заполнение бассейнов..." << std::endl;

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Время выполнения: " << elapsed.count() << " s" << std::endl;

    return 0;
}