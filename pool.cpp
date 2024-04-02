#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <stack>
#include <omp.h>
#include <chrono>
#include <iterator>
#include "pool.h"

unsigned long long GraphPool::GetId() const {
    return rand() % size;
}

void GraphPool::AddPool() {  // добавление бассейна в список
    pools.push_back(Pool());
    return;
}

int GraphPool::FillPool(std::set<unsigned long long> &id_pass) {  // заполнение бассейна водой от 1 до 500 л
    unsigned long long id = GetId();
    if (id_pass.find(id) != id_pass.end()) {  // если найдено, то возвращаемся, не добавляя
        return 0;
    }
    unsigned long long l = rand() % max_litres + 1;  // количество воды для добавления
    pools[id].AddLitres(l);

    return 1;
}

int GraphPool::ConnectPool() {

    unsigned long long id_1 = GetId();
    unsigned long long id_2 = GetId();

    if (id_1 == id_2) {
        return 0;
    }

    bool flag = true;
    auto it_1 = std::find(graph_connect_ind.begin(), graph_connect_ind.end(), id_1);
    unsigned long long ind_edge_1 = it_1 - graph_connect_ind.begin();

    if (it_1 == graph_connect_ind.end()) {
        graph_connect_ind.push_back(id_1);
        ind_edge_1 = graph_connect_ind.size() - 1;
        graph_connect_edge.push_back(std::set<unsigned long long>());
        flag = false;
    }

    auto it_2 = std::find(graph_connect_ind.begin(), graph_connect_ind.end(), id_2);
    unsigned long long ind_edge_2 = it_2 - graph_connect_ind.begin();

    if (it_2 == graph_connect_ind.end()) {
        graph_connect_ind.push_back(id_2);
        ind_edge_2 = graph_connect_ind.size() - 1;
        graph_connect_edge.push_back(std::set<unsigned long long>());
        flag = false;
    }
    
    if (flag && std::find(graph_connect_ind.begin(), graph_connect_ind.end(), id_2) != graph_connect_ind.end()) { // если такая связь есть, то возвращаемся
        return 0;
    }

    graph_connect_edge[ind_edge_1].insert(id_2);
    graph_connect_edge[ind_edge_2].insert(id_1);
    // добавление индексов в списки смежности

    return 1;
}

int GraphPool::UnconnectPool() {

    unsigned long long offset_1 = GetId() % graph_connect_ind.size();
    unsigned long long offset_12 = GetId() % graph_connect_edge[offset_1].size();  

    // где-то здесь почему-то размер 0 у set

    auto tmp_it = (graph_connect_edge[offset_1]).begin();
    std::advance(tmp_it, offset_12);

    unsigned long long id_1 = graph_connect_ind[offset_1];
    unsigned long long id_2 = *tmp_it;

    unsigned long long offset_2 = std::find(graph_connect_ind.begin(), graph_connect_ind.end(), id_2) - graph_connect_ind.begin();

    auto it_21 = tmp_it;
    auto it_12 = graph_connect_edge[offset_2].find(id_1);

    graph_connect_edge[offset_2].erase(it_12);
    graph_connect_edge[offset_1].erase(it_21);

    if (graph_connect_edge[offset_2].size() == 0) {
        graph_connect_ind.erase(graph_connect_ind.begin() + offset_2);
        graph_connect_edge.erase(graph_connect_edge.begin() + offset_2);
    }

    if (graph_connect_edge[offset_1].size() == 0) {
        graph_connect_ind.erase(graph_connect_ind.begin() + offset_1);
        graph_connect_edge.erase(graph_connect_edge.begin() + offset_1);
    }


    // удаление индексов из списков смежности

    return 1;
}

GraphPool::GraphPool(unsigned long long amount, unsigned long long max) : max_litres(max) {  // создание графа (пока как отдельные бассейны) бассейнов
    graph_connect_edge = std::vector<std::set<unsigned long long>>{};
    graph_connect_ind = std::vector<unsigned long long>{};
    AddPools(amount);  // добавление бассейнов в список
}

void GraphPool::AddPools(unsigned long long amount) {  // добавление бассейнов в список
    for (unsigned long long i = 0; i < amount; i++) {
        AddPool();
    }
    size += amount;
    return;
}

void GraphPool::FillPools(unsigned long long amount) {  // заполнение бссейнов со случайными номерами водой
    std::set<unsigned long long> ids_pass;  // set из индексов, в которые добавили воду
    unsigned long long i = 0;
    unsigned long long limit = std::min(amount, size);

    #pragma omp parallel for shared(i, limit, ids_pass) schedule(dynamic)  // параллельные вычисления, т.к. надо только добавлять воду
        for (i = 0; i < limit; ++i) {
            if (!FillPool(ids_pass)) {
                --i;
            }
        }
    SetNewSumLitres();
    return;
}

void GraphPool::ConnectPools(unsigned long long amount) {  // соединение бссейнов со случайными номерами

    unsigned long long i = 0;
    unsigned long long limit = std::min(amount, size * (size - 1) / 2);  // максимальное количество соединений

    for (i = 0; i < limit; ++i) {
        if (!ConnectPool()) {
            --i;
        }
    }

    SetNewSumLitres();  // пересчет литров в бассейнах

    amount_edges += i;
    return;
}

void GraphPool::UnconnectPools(unsigned long long amount) {  // разъединение бссейнов со случайными номерами
    unsigned long long i = 0;
    unsigned long long limit = std::min(amount_edges, amount);  // максимальное количество разъединений

    for (i = 0; i < limit; ++i) {
        if (!UnconnectPool()) {
            --i;
        }
    }
    amount_edges -= i;
    return;
}

void GraphPool::ShowLitres() {
    for (unsigned long long i = 0; i < size; i++) {
        std::cout << "Litres in pool " << i << ": " << pools[i].GetLitres() << std::endl;
        fflush(stdout);
    }
    std::cout << "-----------------------------------------------------------------------" << std::endl;
    fflush(stdout);
    return;
}

void GraphPool::SetNewSumLitres() {
    auto it = graph_connect_edge.begin();
    std::vector<bool> ids_pass(size, false);

    while (it != graph_connect_edge.end()) {
        if (!ids_pass[graph_connect_ind[it - graph_connect_edge.begin()]]) {
            ids_pass[graph_connect_ind[it - graph_connect_edge.begin()]] = true;
            std::pair<double, unsigned long long> tmp = DfsLitres(graph_connect_ind[it - graph_connect_edge.begin()], ids_pass);   // сумма литров и количество бассейнов в одной связи
            double new_sum = tmp.first;
            unsigned long long new_amount = tmp.second;
            DfsLitres(graph_connect_ind[it - graph_connect_edge.begin()], ids_pass, true, new_sum / new_amount);
        }
        ++it;
    }

    return;
}

std::pair<double, unsigned long long> GraphPool::DfsLitres(unsigned long long id, std::vector<bool> &pass, bool set_litr, double litr) {

    double res_litr = pools[id].GetLitres();
    unsigned long long res_count = 1;

    if (set_litr) {  // если уставновлен флаг на пересчет воды в бассейнах
        pools[id].SetLitres(litr);
    }

    std::stack<unsigned long long> q;  // стек из индексов бассейнов, которые ещё не обработаны
    q.push(id);
    while (!q.empty()) {
        unsigned long long cur_neigh = q.top();
        q.pop();

        auto it = std::find(graph_connect_ind.begin(), graph_connect_ind.end(), cur_neigh);

        if (it != graph_connect_ind.end()) {
            for (unsigned long long neighbor : graph_connect_edge[it - graph_connect_ind.begin()]) {
                if (!pass[neighbor]) {
                    pass[neighbor] = true;
                    q.push(neighbor);

                    if (!set_litr) {  // не если уставновлен флаг на пересчет воды в бассейнах, то добавляем в сумму
                        res_litr += pools[neighbor].GetLitres();
                        ++res_count;
                    } else {
                        pools[neighbor].SetLitres(litr);  // иначе присваиваем нвоое значение объема воды
                    }
                }
            }
        }
    }

    return std::make_pair(res_litr, res_count);
}