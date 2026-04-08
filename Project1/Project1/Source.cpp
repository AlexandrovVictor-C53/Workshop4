#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;

// Функция для среднего арифметического
double mean(const vector<double>& v) {
    double s = 0;
    for (double x : v) s += x;
    return s / v.size();
}

// Функция для стандартного отклонения (несмещённое)
double stdev(const vector<double>& v, double m) {
    double ss = 0;
    for (double x : v) {
        double d = x - m;
        ss += d * d;
    }
    return sqrt(ss / (v.size() - 1));
}

// Коэффициент Стьюдента для 30 измерений и доверительной вероятности 0.95
double student() {
    return 2.042;
}

int main() {
    // Параметры установки
    const double h = 0.272;       // м
    const double delta_h = 0.001; // м
    const double v0 = 1.050;      // м/с
    const double delta_v0 = 0.005; // м/с

    // Список материалов и соответствующих файлов с данными
    vector<string> materials = { "Алюминий", "Латунь", "Сталь", "Дерево", "Плексиглас", "Свинец" };
    vector<string> files = { "aluminium.txt", "brass.txt", "steel.txt", "wood.txt", "plexiglass.txt", "lead.txt" };

    // Вывод заголовка
    cout << fixed << setprecision(4);
    cout << "Лабораторная работа №4. Проверка принципа эквивалентности масс\n";
    cout << "Параметры: h = " << h << " ± " << delta_h << " м, v0 = " << v0 << " ± " << delta_v0 << " м/с\n\n";

    // Обработка каждого материала
    for (size_t idx = 0; idx < materials.size(); ++idx) {
        // Открываем файл с данными
        ifstream fin(files[idx]);
        if (!fin) {
            cerr << "Ошибка: не удалось открыть файл " << files[idx] << endl;
            continue;
        }

        // Читаем все времена (в миллисекундах)
        vector<double> t_ms;
        double val;
        while (fin >> val) {
            t_ms.push_back(val);
        }
        fin.close();

        int n = t_ms.size();
        if (n == 0) {
            cerr << "Файл " << files[idx] << " пуст" << endl;
            continue;
        }

        // Переводим в секунды
        vector<double> t_sec(n);
        for (int i = 0; i < n; ++i) t_sec[i] = t_ms[i] / 1000.0;

        // 1. Среднее время
        double t_mean_sec = mean(t_sec);
        double t_mean_ms = t_mean_sec * 1000;

        // 2. Стандартное отклонение отдельного измерения
        double t_sd_sec = stdev(t_sec, t_mean_sec);
        double t_sd_ms = t_sd_sec * 1000;

        // 3. Стандартная ошибка среднего
        double t_sem_sec = t_sd_sec / sqrt(n);
        // 4. Доверительный интервал Δt
        double delta_t_sec = student() * t_sem_sec;
        double delta_t_ms = delta_t_sec * 1000;

        // 5. Вычисляем ускорение g для каждого измерения и среднее g
        vector<double> g_vals(n);
        double sum_g = 0;
        for (int i = 0; i < n; ++i) {
            double t = t_sec[i];
            double g = 2.0 * (h - v0 * t) / (t * t);
            g_vals[i] = g;
            sum_g += g;
        }
        double g_mean = sum_g / n;

        // 6. Погрешность косвенного измерения Δg
        // Вычисляем частные производные в точке t = t_mean_sec
        double dg_dh = 2.0 / (t_mean_sec * t_mean_sec);
        double dg_dv0 = -2.0 / t_mean_sec;
        double dg_dt = -4.0 * (h - v0 * t_mean_sec) / (t_mean_sec * t_mean_sec * t_mean_sec) - 4.0 * v0 / (t_mean_sec * t_mean_sec);

        double delta_g = sqrt(dg_dh * dg_dh * delta_h * delta_h +
            dg_dv0 * dg_dv0 * delta_v0 * delta_v0 +
            dg_dt * dg_dt * delta_t_sec * delta_t_sec);

        // Вывод результатов
        cout << "=== " << materials[idx] << " ===\n";
        cout << "Среднее время t = " << t_mean_ms << " ± " << delta_t_ms << " мс\n";
        cout << "Стандартное отклонение S_t = " << t_sd_ms << " мс\n";
        cout << "Ускорение g = " << g_mean << " ± " << delta_g << " м/с^2\n";
        cout << "Относительная погрешность δg = " << (delta_g / g_mean) * 100 << "%\n\n";
    }

    return 0;
}