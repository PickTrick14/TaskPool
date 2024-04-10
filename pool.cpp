#include <iostream>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <stack>
#include "pool.h"

void Pool::AddIdConnect(unsigned int id) { 
    if (id_connect == nullptr) {
        id_connect = new std::unordered_set<unsigned int>;
    }
    (*id_connect).insert(id);  
}

void Pool::RemoveIdConnect(unsigned int id) {
    if (id_connect != nullptr) {
        (*id_connect).erase(id);
    }
    if ((*id_connect).size() == 0) {
        delete id_connect;
        id_connect = nullptr;
    }
}

unsigned int GraphPool::GetId() const {
    return rand() % pools.size();
}

void GraphPool::FillPool(std::unordered_set<unsigned int> &id_pass) {
    unsigned int id = GetId();
    while (id_pass.find(id) != id_pass.end()) {
        id = GetId();
    }
    id_pass.insert(id);
    pools[id].AddLitres(rand() % max_litres + 1);
}

int GraphPool::ConnectPool(std::unordered_set<unsigned int> &id_pass) {
    unsigned int id_1 = GetId();
    std::unordered_set<unsigned int> *connect_1 = pools[id_1].id_connect;
    unsigned int id_2 = GetId();

    while (id_1 == id_2) {
        id_2 = GetId();
    }

    std::unordered_set<unsigned int> *connect_2 = pools[id_2].id_connect;

    if (connect_1 != nullptr && connect_2 != nullptr) {
        if ((*connect_1).find(id_2) != (*connect_1).end()) {
            return 0;
        }
    }

    pools[id_1].AddIdConnect(id_2);
    pools[id_2].AddIdConnect(id_1);

    ids_connect.insert(id_1);
    ids_connect.insert(id_2);

    ++amount_edges;
    id_pass.insert(id_1);

    return 1;
}

int GraphPool::UnconnectPool() {

    unsigned int id_1 = *(ids_connect.begin());
    unsigned int id_2 = *(pools[id_1].id_connect->begin());

    pools[id_1].RemoveIdConnect(id_2);
    pools[id_2].RemoveIdConnect(id_1);

    if (pools[id_1].id_connect == nullptr) {
        ids_connect.erase(id_1);
    }

    if (pools[id_2].id_connect == nullptr) {
        ids_connect.erase(id_2);
    }

    return 1;
}

GraphPool::GraphPool(unsigned int amount, unsigned int max) : max_litres(max), amount_edges(0) {
    pools = std::vector<Pool>(amount);
}

void GraphPool::FillPools(unsigned int amount) {
    unsigned int i = 0;
    unsigned int limit = std::min(amount, (unsigned int) pools.size());
    std::unordered_set<unsigned int> *id_pass = new std::unordered_set<unsigned int>();

    while (i < limit) {
        FillPool(*id_pass);
        ++i;
    }

    SetNewSumLitres(*id_pass);

    delete id_pass;
}

void GraphPool::ConnectPools(unsigned int amount) {
    unsigned int i = 0;
    unsigned int limit = std::min(amount, (unsigned int) (pools.size() * (pools.size() - 1) / 2));
    std::unordered_set<unsigned int> *id_pass = new std::unordered_set<unsigned int>();

    while (i < limit) {
        if (ConnectPool(*id_pass)) {
            ++i;
        }
    }
    SetNewSumLitres(*id_pass);
    delete id_pass;
}

void GraphPool::UnconnectPools(unsigned int amount) {
    unsigned int i = 0;
    unsigned int limit = std::min(amount, amount_edges);
    while (i < limit) {
        if (UnconnectPool()) {
            ++i;
        }
    }
}

void GraphPool::ShowLitres() {
    unsigned int i = 0;
    for (; i < pools.size(); ++i) {
        std::cout << "Pool №" << i << ": " << pools[i].GetLitres() << std::endl;
    }
}

void GraphPool::SetNewSumLitres(std::unordered_set<unsigned int> &id_need) {
    auto it = id_need.begin();
    std::vector<bool> *ids_pass = new std::vector<bool>(pools.size(), false);

    while (it != id_need.end()) {
        if (!(*ids_pass)[*it]) {
            (*ids_pass)[*it] = true;
            DfsLitres(*it, *ids_pass);   // сумма литров и количество бассейнов в одной связи
        }
        ++it;
    }

    delete ids_pass;

    return;
}

void GraphPool::DfsLitres(unsigned int id, std::vector<bool> &pass, bool set_litr, double litr) {

    double res_litr = pools[id].GetLitres();
    unsigned int res_count = 1;

    if (set_litr) {  // если уставновлен флаг на пересчет воды в бассейнах
        pools[id].SetLitres(litr);
    }

    std::stack<unsigned int> q;  // стек из индексов бассейнов, которые ещё не обработаны
    std::stack<unsigned int> set_ind;  // стек из индексов, для которых надо будет установить новый объём литров

    q.push(id);
    set_ind.push(id);

    while (!q.empty()) {
        unsigned int cur_neigh = q.top();
        q.pop();

        auto graph_connect = pools[cur_neigh].id_connect;

        if (graph_connect != nullptr) {
            for (unsigned int neighbor : *graph_connect) {
                if (!pass[neighbor]) {
                    pass[neighbor] = true;
                    q.push(neighbor);
                    set_ind.push(neighbor);

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

    double res = res_litr / res_count;

    while (!set_ind.empty()) {
        unsigned int cur_ind = set_ind.top();
        set_ind.pop();
        pools[cur_ind].SetLitres(res);
    }
}