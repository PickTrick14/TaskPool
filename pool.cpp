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

int GraphPool::ConnectPool(void) {
    unsigned long long id_1 = GetId();
    unsigned long long id_2 = GetId();

    if (graph_connect.count(id_1) == 0) {
        graph_connect[id_1] = std::set<unsigned long long>();
    }

    if (graph_connect.count(id_2) == 0) {
        graph_connect[id_2] = std::set<unsigned long long>();
    }

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

int GraphPool::UnconnectPool() {

    auto it_11 = graph_connect.begin();
    std::advance(it_11, rand() % graph_connect.size());
    auto it_12 = it_11->second.begin();
    std::set<unsigned long long> tmp = it_11->second;
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
        if (ConnectPool()) {  // если не соединены, то увеличиваем количество пройденных
            i++;
        }    
    }
    amount_edges += i;
    return;
}

void GraphPool::UnconnectPools(unsigned long long amount) {  // разъединение бссейнов со случайными номерами
    unsigned long long i = 0;
    while (i < amount && i < amount_edges) {
        std::cout << i << std::endl;
        if (UnconnectPool()) {  // если соединены, то увеличиваем количество пройденных
            ++i;
            std::cout << i << std::endl;
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

        if (graph_connect.count(cur_neigh) != 0) {
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
    }

    return std::make_pair(res_litr, res_count);
}