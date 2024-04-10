#include "pool.h"
#include <iostream>
#include <chrono>

constexpr unsigned long long N = 17500000;  //  35000000
constexpr unsigned long long K = 5500000;  //  11000000
constexpr unsigned long long L = 12000000;  // 24000000
constexpr unsigned long long M = 2000000;  //  4000000
constexpr unsigned int MAX_LITR = 500;



int main(void) {

    auto start = std::chrono::high_resolution_clock::now();

    std::cout << "Начало работы..." << std::endl;
    std::cout << "Создание графа..." << std::endl;
    GraphPool net(N, MAX_LITR);
    std::cout << "Создание графа COMPLETED" << std::endl;

    std::cout << "Заполнение бассейнов №1..." << std::endl;
    net.FillPools(N);
    // net.ShowLitres();
    std::cout << "Заполнение бассейнов №1 COMPLETED" << std::endl;

    std::cout << "Соединение бассейнов..." << std::endl;
    net.ConnectPools(K);
    // net.ShowLitres();
    std::cout << "Соединение бассейнов COMPLETED" << std::endl;

    std::cout << "Заполнение бассейнов №2..." << std::endl;
    net.FillPools(L);
    // net.ShowLitres();
    std::cout << "Заполнение бассейнов №2 COMPLETED" << std::endl;

    std::cout << "Разъединение бассейнов..." << std::endl;
    net.UnconnectPools(M);
    std::cout << "Разъединение бассейнов COPMLETED" << std::endl;
    
    std::cout << "Заполнение бассейнов №3..." << std::endl;
    net.FillPools(L);
    // net.ShowLitres();
    std::cout << "Заполнение бассейнов №3 COMPLETED" << std::endl;

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Время выполнения: " << elapsed.count() << " s" << std::endl;

    return 0;
}