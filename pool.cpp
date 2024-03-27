#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <stack>
#include "pool.h"

unsigned long long GraphPool::GetId() const {
    return rand() % size;
}

void GraphPool::AddPool() {  // добавление бассейна в список
    pools.push_back(Pool());
    return;
}

void GraphPool::FillPool(unsigned long long id) {  // заполнение бассейна водой от 1 до 500 л
    unsigned long long l = rand() % max_litres + 1;  // количество воды для добавления

    std::pair<double, unsigned long long> tmp = DfsLitres(id);  // текущее количество воды в бассейнах в связи и их количество

    double new_sum = tmp.first + l;
    unsigned long long new_amount = tmp.second;

    DfsLitres(id, true, new_sum / new_amount);  // установление нового количества воды в бассейнах в связи и их количества
    return;
}

int GraphPool::ConnectPool(unsigned long long id_1, unsigned long long id_2) {
    if (std::find(graph_connect[id_1].begin(), graph_connect[id_1].end(), id_2) != graph_connect[id_1].end()) { // если такая связь есть, то возвращаемся
        return 0;
    }
    graph_connect[id_1].insert(id_2);
    graph_connect[id_2].insert(id_1);
    // добавление индексов в списки смежности

    std::pair<double, unsigned long long> tmp = DfsLitres(id_1);
    double new_sum = tmp.first;
    unsigned long long new_amount = tmp.second;
    DfsLitres(id_1, true, new_sum / new_amount);  // установление нового количества воды в бассейнах в связи и их количества
    return 1;
}

int GraphPool::UnconnectPool(unsigned long long id_1, unsigned long long id_2) {
    auto it_12 = std::find(graph_connect[id_1].begin(), graph_connect[id_1].end(), id_2);

    if (it_12 == graph_connect[id_1].end()) { // если такой связи нет, то возвращаемся
        return 0;
    }
    graph_connect[id_1].erase(it_12);
    graph_connect[id_2].erase(std::find(graph_connect[id_2].begin(), graph_connect[id_2].end(), id_1));
    // удаление индексов из списков смежности

    return 1;
}

GraphPool::GraphPool(unsigned long long amount, unsigned long long max) : max_litres(max) {  // создание графа (пока как отдельные бассейны) бассейнов
    AddPools(amount);  // добавление бассейнов в список
}

void GraphPool::AddPools(unsigned long long amount) {  // добавление бассейнов в список
    for (unsigned long long i = 0; i < amount; i++) {
        AddPool();
    }
    this->size += amount;
    return;
}

void GraphPool::FillPools(unsigned long long amount) {  // заполнение бссейнов со случайными номерами водой
    std::set<unsigned long long> ids_pass;  // set из индексов, в которые добавили воду
    unsigned long long i = 0;
    while (i < amount && i < size) {
        unsigned long long id = GetId();
        if (ids_pass.find(id) == ids_pass.end()) {
            ids_pass.insert(id);
            FillPool(id);
            ++i;
        }
    }
    return;
}

void GraphPool::ConnectPools(unsigned long long amount) {  // соединение бссейнов со случайными номерами

    unsigned long long i = 0;
    unsigned long long max_edges = size * (size - 1) / 2;

    while (i < amount && i < max_edges) {
        unsigned long long id_1 = GetId();
        unsigned long long id_2 = GetId();
        if (ConnectPool(id_1, id_2)) {  // если не соединены, то увеличиваем количество пройденных
            i++;
        }    
    }
    amount_edges += i;
    return;
}

void GraphPool::UnconnectPools(unsigned long long amount) {  // разъединение бссейнов со случайными номерами
    unsigned long long i = 0;
    while (i < amount && i < amount_edges) {
        unsigned long long id_1 = GetId();
        unsigned long long id_2 = GetId();
        if (UnconnectPool(id_1, id_2)) {  // если соединены, то увеличиваем количество пройденных
            ++i;
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
}

std::pair<double, unsigned long long> GraphPool::DfsLitres(unsigned long long id, bool set_litr, double litr) {

    double res_litr = pools[id].GetLitres();
    unsigned long long res_count = 1;

    if (set_litr) {
        pools[id].SetLitres(litr);
    }

    std::vector<bool> visit_ids(size, false);  // вектор из значений, прошли вершину или нет
    visit_ids[id] = true;

    std::stack<unsigned long long> q;  // стек из индексов бассейнов, которые ещё не обработаны
    q.push(id);
    while (!q.empty()) {
        unsigned long long cur_neigh = q.top();
        q.pop();

        for (unsigned long long neighbor : graph_connect[cur_neigh]) {
            if (!visit_ids[neighbor]) {
                visit_ids[neighbor] = true;
                q.push(neighbor);
                if (!set_litr) {
                    res_litr += pools[neighbor].GetLitres();
                    ++res_count;
                } else {
                    pools[neighbor].SetLitres(litr);
                }
            }
    
        }
    }

    return std::make_pair(res_litr, res_count);
}