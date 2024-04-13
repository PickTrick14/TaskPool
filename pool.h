#ifndef POOL_H
#define POOL_H

#include <vector>
#include <unordered_set>
#include <random>

class Pool {
    private:
        double litres;
        
    public:
        std::unordered_set<unsigned int> *id_connect;

        Pool(double litr = 0) : litres(litr), id_connect(nullptr) {}
        void AddLitres(double litr) { litres += litr; }
        void SetLitres(double litr) { litres = litr; }
        double GetLitres() const { return litres; }

        void AddIdConnect(unsigned int id);

        void RemoveIdConnect(unsigned int id);

        ~Pool() {
            if (id_connect != nullptr) {
                delete id_connect;
            }
        }
};

class GraphPool {
    private:
        std::vector<Pool> pools;  // вектор хранения объектов "Бассейн"

        unsigned int max_litres;  // максимальный объем воды для добавленияль
        unsigned int amount_edges;  // количество связей в графе
        std::unordered_set<unsigned int> ids_connect;  // индексы, которые имеют связанные бассейны
        bool first_filled;
        std::mt19937 rng;

        unsigned int GetId() const;

        void FillPool(std::unordered_set<unsigned int> &id_pass);  // заполнение бассейна водой от 1 до max_litres л

        void ConnectPool(std::unordered_set<unsigned int> &id_pass);  // соединение бассейнов со случайными номерами

        int UnconnectPool();  // разъединение бассейнов со случайными номерами

        void SetNewSumLitres(std::unordered_set<unsigned int> &id_pass);

        void DfsLitres(unsigned int id, std::vector<bool> &pass);

    public:
        GraphPool(unsigned int amount, unsigned int max);  // создание графа (пока как отдельные бассейны) бассейнов

        void FillPools(unsigned int amount);  // заполнение бссейнов со случайными номерами водой

        void ConnectPools(unsigned int amount);  // соединение бссейнов со случайными номерами

        void UnconnectPools(unsigned int amount);  // разъединение бссейнов со случайными номерами

        void ShowLitres();  // вывод объема воды всех бассейнов
};

#endif