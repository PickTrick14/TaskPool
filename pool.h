#ifndef POOL_H
#define POOL_H

#include <vector>
#include <unordered_set>

class Pool {
    private:
        double litres;
        
    public:
        std::unordered_set<unsigned long long> *id_connect;

        Pool(double litr = 0) : litres(litr), id_connect(nullptr) {}
        void AddLitres(double litr) { litres += litr; }
        void SetLitres(double litr) { litres = litr; }
        double GetLitres() const { return litres; }

        void AddIdConnect(unsigned long long id);

        void RemoveIdConnect(unsigned long long id);

        ~Pool() {
            if (id_connect != nullptr) {
                delete id_connect;
            }
        }
};

class GraphPool {
    private:
        std::vector<Pool> pools;  // вектор хранения объектов "Бассейн"
        unsigned long long size;  // количество бассейнов в графе

        unsigned long long max_litres;  // максимальный объем воды для добавленияль
        unsigned long long amount_edges;  // количество связей в графе
        std::unordered_set<unsigned long long> ids_connect;  // индексы, которые имеют связанные бассейны

        unsigned long long GetId() const;

        int FillPool(std::unordered_set<unsigned long long> &id_pass);  // заполнение бассейна водой от 1 до max_litres л

        int ConnectPool(std::unordered_set<unsigned long long> &id_pass);  // соединение бассейнов со случайными номерами

        int UnconnectPool();  // разъединение бассейнов со случайными номерами

        void SetNewSumLitres(std::unordered_set<unsigned long long> &id_pass);

        std::pair<double, unsigned long long> DfsLitres(unsigned long long id, std::vector<bool> &pass, bool set_litr = false, double litr = 0);

    public:
        GraphPool(unsigned long long amount, unsigned long long max);  // создание графа (пока как отдельные бассейны) бассейнов

        void FillPools(unsigned long long amount);  // заполнение бссейнов со случайными номерами водой

        void ConnectPools(unsigned long long amount);  // соединение бссейнов со случайными номерами

        void UnconnectPools(unsigned long long amount);  // разъединение бссейнов со случайными номерами

        void ShowLitres();  // вывод объема воды всех бассейнов
};

#endif