#ifndef POOL_H

#include <vector>
#include <set>

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
        std::vector<std::vector<unsigned long long>> graph_connect;    // вектор смежности для каждого бассейна
        unsigned long long max_litres;  // максимальный объем воды в бассейне

        unsigned long long GetId() const;

        void AddPool();  // добавление бассейна в список

        void FillPool(unsigned long long id);  // заполнение бассейна водой от 1 до max_litres л

        int ConnectPool(unsigned long long id_1, unsigned long long id_2);  // соединение бассейнов со случайными номерами

        int UnconnectPool(unsigned long long id_1, unsigned long long id_2);  // разъединение бассейнов со случайными номерами

        std::pair<double, unsigned long long> DfsLitres(unsigned long long id, bool set_litr = false, double litr = 0);

    public:
        GraphPool(unsigned long long amount, unsigned long long max);  // создание графа (пока как отдельные бассейны) бассейнов

        void AddPools(unsigned long long amount); // добавление бассейнов в список

        void FillPools(unsigned long long amount);  // заполнение бссейнов со случайными номерами водой

        void ConnectPools(unsigned long long amount);  // соединение бссейнов со случайными номерами

        void UnconnectPools(unsigned long long amount);  // разъединение бссейнов со случайными номерами

        void ShowLitres();  // вывод объема воды всех бассейнов
};

#endif