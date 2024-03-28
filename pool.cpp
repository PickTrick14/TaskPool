#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <stack>
#include <omp.h>
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

    if (graph_connect.find(id_1) == graph_connect.end()) {  // если списка смежности для бассейна id_1 нет, то создаем
        graph_connect[id_1] = std::set<unsigned long long>();
    }

    if (graph_connect.find(id_2) == graph_connect.end()) {  // если списка смежности для бассейна id_2 нет, то создаем
        graph_connect[id_2] = std::set<unsigned long long>();
    }

    if (std::find(graph_connect[id_1].begin(), graph_connect[id_1].end(), id_2) != graph_connect[id_1].end()) { // если такая связь есть, то возвращаемся
        return 0;
    }

    graph_connect[id_1].insert(id_2);
    graph_connect[id_2].insert(id_1);
    // добавление индексов в списки смежности

    return 1;
}

int GraphPool::UnconnectPool() {

    auto it_11 = graph_connect.begin();
    std::advance(it_11, rand() % graph_connect.size());
    auto it_12 = it_11->second.begin();
    std::set<unsigned long long> tmp = it_11->second;

    if (tmp.size() == 0) {  // если гонка (ещё set не удалили), то выходим из функции, т.к. связей нет
        return 0;
    }

    std::advance(it_12, rand() % tmp.size());

    unsigned long long id_1 = it_11->first;
    unsigned long long id_2 = *it_12;

    graph_connect[id_2].erase(std::find(graph_connect[id_2].begin(), graph_connect[id_2].end(), id_1));
    graph_connect[id_1].erase(it_12);

    if (graph_connect[id_1].size() == 0) {
        graph_connect.erase(id_1);
    }

    if (graph_connect[id_2].size() == 0) {
        graph_connect.erase(id_2);
    }
    // удаление индексов из списков смежности

    return 1;
}

GraphPool::GraphPool(unsigned long long amount, unsigned long long max) : max_litres(max) {  // создание графа (пока как отдельные бассейны) бассейнов
    graph_connect = std::map<unsigned long long, std::set<unsigned long long>>{};
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

    #pragma omp parallel for shared(i, limit) schedule(dynamic)  // параллельные вычисления, т.к. надо только добавлять в соединения
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

    #pragma omp parallel for shared(i, limit, graph_connect) schedule(dynamic)  // параллельные вычисления, т.к. надо только убрать в соединения
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

void GraphPool::SetNewSumLitresLink(unsigned long long id) {  // уставновление нового объема воды в бассейнах

    std::pair<double, unsigned long long> tmp = DfsLitres(id);   // сумма литров и количество бассейнов в одной связи
    double new_sum = tmp.first;
    unsigned long long new_amount = tmp.second;
    DfsLitres(id, true, new_sum / new_amount);  // установление нового количества воды в бассейнах в связи и их количества

    return;
}

void GraphPool::SetNewSumLitres() {
    auto it = graph_connect.begin();
    std::set<unsigned long long> ids_pass;

    while (it != graph_connect.end()) {
        if (ids_pass.find(it->first) == ids_pass.end()) {
            ids_pass.insert(it->first);
            SetNewSumLitresLink(it->first);
            ++it;
        }
    }

    /* #pragma omp parallel for shared(graph_connect) schedule(dynamic)
        for (std::map<unsigned long long, std::set<unsigned long long>>::iterator it = graph_connect.begin(); it != graph_connect.end(); it++) {
            SetNewSumLitresLink(it->first);
        } */

    return;
}

std::pair<double, unsigned long long> GraphPool::DfsLitres(unsigned long long id, bool set_litr, double litr) {

    double res_litr = pools[id].GetLitres();
    unsigned long long res_count = 1;

    if (set_litr) {  // если уставновлен флаг на пересчет воды в бассейнах
        pools[id].SetLitres(litr);
    }

    std::vector<bool> visit_ids(size, false);  // вектор из значений, прошли вершину или нет
    visit_ids[id] = true;

    std::stack<unsigned long long> q;  // стек из индексов бассейнов, которые ещё не обработаны
    q.push(id);
    while (!q.empty()) {
        unsigned long long cur_neigh = q.top();
        q.pop();

        if (graph_connect.find(cur_neigh) != graph_connect.end()) {
            for (unsigned long long neighbor : graph_connect[cur_neigh]) {
                if (!visit_ids[neighbor]) {
                    visit_ids[neighbor] = true;
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