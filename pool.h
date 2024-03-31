#ifndef POOL_H

#include <vector>
#include <set>
#include <map>

class Pool {
    private:
        double litres;
    public:
        Pool(double litr = 0) : litres(litr) {}
        void AddLitres(double litr) { litres += litr; }
        void SetLitres(double litr) { litres = litr; }
        double GetLitres() const { return litres; }
};

class GraphPool {
    private:
        std::vector<Pool> pools;  // вектор хранения объектов "Бассейн"
        unsigned long long size;  // количество бассейнов в графе
        std::map<unsigned long long, std::set<unsigned long long>> graph_connect;    // словарь смежности бассейнов
        unsigned long long max_litres;  // максимальный объем воды для добавленияль
        unsigned long long amount_edges;  // количество связей в графе

        unsigned long long GetId() const;

        void AddPool();  // добавление бассейна в список

        int FillPool(std::set<unsigned long long> &id_pass);  // заполнение бассейна водой от 1 до max_litres л

        int ConnectPool();  // соединение бассейнов со случайными номерами

        int UnconnectPool();  // разъединение бассейнов со случайными номерами

        void SetNewSumLitresLink(unsigned long long id);

        void SetNewSumLitres();

        std::pair<double, unsigned long long> DfsLitres(unsigned long long id, std::vector<bool> &pass, bool set_litr = false, double litr = 0);

    public:
        GraphPool(unsigned long long amount, unsigned long long max);  // создание графа (пока как отдельные бассейны) бассейнов

        void AddPools(unsigned long long amount); // добавление бассейнов в список

        void FillPools(unsigned long long amount);  // заполнение бссейнов со случайными номерами водой

        void ConnectPools(unsigned long long amount);  // соединение бссейнов со случайными номерами

        void UnconnectPools(unsigned long long amount);  // разъединение бссейнов со случайными номерами

        void ShowLitres();  // вывод объема воды всех бассейнов
};

#endif