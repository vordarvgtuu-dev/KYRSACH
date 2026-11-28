#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <locale.h>
#include <float.h>
#include <stdlib.h>
#include <time.h>

#define EPS 1e-9
#define GRAPH_WIDTH 60
#define MAX_POINTS 500

typedef double (*math_func)(double);


typedef struct {
    double x;
    double y;
} point_t;


double func_y(double x) {
    return log(x * x) - x + 4.0;
}


double func_v(double x) {
    if (fabs(x) > 1.0)
        return fabs(x);
    else if (x <= -1.0)
        return 1.0 + x;
    else
        return sin(x);
}

int calc_func(math_func f, double x, double* result) {
    if (f == func_y && fabs(x) <= EPS)
        return 1; 
    double val = f(x);
    if (!isfinite(val))
        return 2; 

    *result = val;
    return 0;
}

void clear_buf(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


int count_points(double xmin, double xmax, double dx) {
    double steps = (xmax - xmin) / dx + EPS;
    if (steps >= MAX_POINTS) return MAX_POINTS + 1;
    if (steps < 0.0) return 0;
    return (int)steps + 1;
}

int get_start_idx(int type) {
    return (type == 2 || type == 4) ? 1 : 0;
}

int get_end_idx(int type, double xmin, double xmax, double dx, int n) {
    int last = n - 1;
    if ((type == 2 || type == 3) && fabs(xmin + (n - 1) * dx - xmax) < dx * 1e-6)
        last = n - 2;
    return last;
}


point_t* table_func(math_func f, double xmin, double xmax, double dx, int* out_n) {
    int total = count_points(xmin, xmax, dx);
    if (total > MAX_POINTS) total = MAX_POINTS;
    if (total <= 0) total = 1;

    point_t* arr = (point_t*)malloc(total * sizeof(point_t));
    if (arr == NULL) {
        *out_n = 0;
        return NULL;
    }

    int k = 0;
    for (int i = 0; i < total; i++) {
        double x = xmin + i * dx;
        double y;
        if (calc_func(f, x, &y) == 0) {
            arr[k].x = x;
            arr[k].y = y;
            k++;
        }
    }
    *out_n = k;
    return arr;
}

point_t* sort_func(point_t* arr, int n, char c) {
    if (arr == NULL || n <= 1) return arr;

    int asc = (c == 'a' || c == 'A');
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            int need_swap = asc ? (arr[j].y > arr[j + 1].y) : (arr[j].y < arr[j + 1].y);
            if (need_swap) {
                point_t tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
    return arr;
}


void print_points_table(const point_t* arr, int n, const char* func_name) {
    printf(".------------.-----------------.\n");
    printf("| x          | %-15s |\n", func_name);
    printf(".------------.-----------------.\n");
    for (int i = 0; i < n; i++) {
        if (isnan(arr[i].y))
            printf("| %10.4f | %15s |\n", arr[i].x, "не определена");
        else if (isinf(arr[i].y))
            printf("| %10.4f | %15s |\n", arr[i].x, "переполнение");
        else
            printf("| %10.4f | %15.6f |\n", arr[i].x, arr[i].y);
    }
    printf(".------------.-----------------.\n");
}


double find_min(math_func f, double xmin, double dx, int i_first, int i_last) {
    double min = DBL_MAX, y;
    for (int i = i_first; i <= i_last; i++) {
        if (calc_func(f, xmin + i * dx, &y) == 0 && y < min)
            min = y;
    }
    return min;
}

double find_max(math_func f, double xmin, double dx, int i_first, int i_last) {
    double max = -DBL_MAX, y;
    for (int i = i_first; i <= i_last; i++) {
        if (calc_func(f, xmin + i * dx, &y) == 0 && y > max)
            max = y;
    }
    return max;
}

int y_to_col(double y, double ymin, double ymax, int width) {
    if (fabs(ymax - ymin) < EPS) return width / 2;
    int col = (int)(((y - ymin) / (ymax - ymin)) * (width - 1) + 0.5);
    if (col < 0) return 0;
    if (col >= width) return width - 1;
    return col;
}

int main(void) {
    int choice, action, type;
    double xmin, xmax, dx, x, y;
    math_func f;
    const char* name;
    point_t pts[MAX_POINTS];

    setlocale(LC_ALL, "Russian");
    srand((unsigned)time(NULL));

    do {
        printf("\nТабулирование функции (Вариант 6)\n");
        printf("1 - Y(x) = ln(x^2) - x + 4\n");
        printf("2 - V(x) - кусочная функция\n");
        printf("0 - выход\n");
        printf("Ваш выбор: ");

        if (scanf("%d", &choice) != 1) {
            clear_buf();
            printf("Ошибка: нужно ввести число.\n");
            choice = -1;
            continue;
        }
        clear_buf();

        if (choice == 0) {
            printf("Работа программы завершена.\n");
            break;
        }
        if (choice != 1 && choice != 2) {
            printf("Нет такого пункта меню.\n");
            continue;
        }

        f = (choice == 1) ? func_y : func_v;
        name = (choice == 1) ? "Y(x)" : "V(x)";

        printf("\nЧто сделать с функцией %s?\n", name);
        printf("1 - вычислить значение в точке\n");
        printf("2 - таблица значений\n");
        printf("3 - график\n");
        printf("4 - набор введённых аргументов\n");
        printf("5 - десять значений от xmin с шагом dx\n");
        printf("6 - N случайных аргументов на промежутке\n");
        printf("7 - ПЗ2: таблица с сортировкой\n");
        printf("Ваш выбор: ");

        if (scanf("%d", &action) != 1 || action < 1 || action > 7) {
            clear_buf();
            printf("Ошибка: выберите пункт от 1 до 7.\n");
            continue;
        }
        clear_buf();


        if (action == 1) {
            printf("Введите x: ");
            if (scanf("%lf", &x) != 1) {
                clear_buf();
                printf("Ошибка: нужно ввести число.\n");
                continue;
            }
            clear_buf();

            int st = calc_func(f, x, &y);
            if (st == 1) printf("%s не определена в точке x = %g\n", name, x);
            else if (st == 2) printf("Значение %s в точке x = %g не помещается в double\n", name, x);
            else printf("%s = %.6f при x = %g\n", name, y, x);
        }

        else if (action == 4) {
            int m;
            printf("Сколько аргументов ввести (1..%d)? ", MAX_POINTS);
            if (scanf("%d", &m) != 1 || m <= 0 || m > MAX_POINTS) {
                clear_buf();
                printf("Ошибка: некорректное количество.\n");
                continue;
            }
            clear_buf();

            for (int k = 0; k < m; k++) {
                printf("Введите x[%d]: ", k + 1);
                if (scanf("%lf", &pts[k].x) != 1) {
                    clear_buf();
                    printf("Ошибка ввода. Повторите.\n");
                    k--;
                    continue;
                }
                clear_buf();
                int st = calc_func(f, pts[k].x, &y);
                pts[k].y = (st == 0) ? y : ((st == 1) ? NAN : INFINITY);
            }
            printf("\nТаблица значений для введённого набора аргументов\n");
            print_points_table(pts, m, name);
        }

        else if (action == 5) {
            printf("Введите xmin и dx: ");
            if (scanf("%lf %lf", &xmin, &dx) != 2 || dx <= 0.0) {
                clear_buf();
                printf("Ошибка: введите два числа, dx > 0.\n");
                continue;
            }
            clear_buf();

            int cnt_def = 0, cnt_ovf = 0;
            double ylo = DBL_MAX, yhi = -DBL_MAX;

            for (int j = 0; j < 10; j++) {
                pts[j].x = xmin + j * dx;
                int st = calc_func(f, pts[j].x, &y);
                if (st == 1) {
                    pts[j].y = NAN;
                }
                else if (st == 2) {
                    pts[j].y = INFINITY;
                    cnt_ovf++;
                }
                else {
                    pts[j].y = y;
                    cnt_def++;
                    if (y < ylo) ylo = y;
                    if (y > yhi) yhi = y;
                }
            }
            printf("\nДесять значений функции %s от %g с шагом %g\n", name, xmin, dx);
            print_points_table(pts, 10, name);
            printf("определено: %d, переполнено: %d", cnt_def, cnt_ovf);
            if (cnt_def > 0) printf(", y_min = %.6f, y_max = %.6f", ylo, yhi);
            printf("\n");
        }

        else if (action == 6) {
            int nn;
            printf("Введите xmin, xmax и N: ");
            if (scanf("%lf %lf %d", &xmin, &xmax, &nn) != 3 || xmin >= xmax || nn <= 0 || nn > MAX_POINTS) {
                clear_buf();
                printf("Ошибка: проверьте границы и N (1..%d).\n", MAX_POINTS);
                continue;
            }
            clear_buf();

            for (int k = 0; k < nn; k++) {
                pts[k].x = xmin + (xmax - xmin) * ((double)rand() / RAND_MAX);
                int st = calc_func(f, pts[k].x, &y);
                pts[k].y = (st == 0) ? y : ((st == 1) ? NAN : INFINITY);
            }
            printf("\nТаблица значений для %d случайных аргументов на [%.4f; %.4f]\n", nn, xmin, xmax);
            print_points_table(pts, nn, name);
        }

        else if (action == 7) {
            char sort_c;
            printf("Введите xmin, xmax и шаг dx: ");
            if (scanf("%lf %lf %lf", &xmin, &xmax, &dx) != 3 || dx <= 0.0 || xmin >= xmax) {
                clear_buf();
                printf("Ошибка: проверьте xmin, xmax и dx > 0.\n");
                continue;
            }
            clear_buf();
            printf("Вид сортировки (a - по возрастанию, d - по убыванию): ");
            if (scanf(" %c", &sort_c) != 1 || (sort_c != 'a' && sort_c != 'A' && sort_c != 'd' && sort_c != 'D')) {
                clear_buf();
                printf("Ошибка: введите a или d.\n");
                continue;
            }
            clear_buf();
            int n_total = count_points(xmin, xmax, dx);
            if (n_total > MAX_POINTS) {
                printf("Внимание: взяты первые %d точек из %d.\n", MAX_POINTS, n_total);
                n_total = MAX_POINTS;
            }
            int n_valid = 0;
            point_t* sorted_pts = table_func(f, xmin, xmax, dx, &n_valid);
            if (sorted_pts == NULL) {
                printf("Ошибка выделения памяти.\n");
                continue;
            }
            sort_func(sorted_pts, n_valid, sort_c);
            printf("\nОтсортированная таблица (%s)\n", (sort_c == 'a' || sort_c == 'A') ? "по возрастанию" : "по убыванию");
            print_points_table(sorted_pts, n_valid, name);
            if (n_valid < n_total)
                printf("Внимание: %d точек не определены и не вошли в таблицу.\n", n_total - n_valid);
            free(sorted_pts);
            }

        else {
                printf("Тип промежутка (1: [a;b], 2: (a;b), 3: [a;b), 4: (a;b]): ");
                if (scanf("%d", &type) != 1 || type < 1 || type > 4) {
                    clear_buf();
                    printf("Нет такого типа промежутка.\n");
                    continue;
                }
                clear_buf();
                printf("Введите xmin, xmax и шаг dx: ");
                if (scanf("%lf %lf %lf", &xmin, &xmax, &dx) != 3 || dx <= 0.0 || xmin > xmax || (xmin == xmax && type != 1)) {
                    clear_buf();
                    printf("Ошибка: некорректные границы или шаг.\n");
                    continue;
                }
                clear_buf();
                int n = count_points(xmin, xmax, dx);
                if (n > MAX_POINTS) {
                    printf("Слишком много точек (максимум %d).\n", MAX_POINTS);
                    continue;
                }
                int i_first = get_start_idx(type);
                int i_last = get_end_idx(type, xmin, xmax, dx, n);
                if (i_first > i_last) {
                    printf("Нет ни одной точки на промежутке.\n");
                    continue;
                }
                if (action == 2) {
                    int k = 0;
                    for (int i = i_first; i <= i_last; i++) {
                        pts[k].x = xmin + i * dx;
                        int st = calc_func(f, pts[k].x, &y);
                        pts[k].y = (st == 0) ? y : ((st == 1) ? NAN : INFINITY);
                        k++;
                    }
                    printf("\nТаблица значений функции %s\n", name);
                    print_points_table(pts, k, name);
                }
                else if (action == 3) {
                    double ymin = find_min(f, xmin, dx, i_first, i_last);
                    double ymax = find_max(f, xmin, dx, i_first, i_last);
                    if (ymin > ymax) {
                        printf("Функцию нельзя вычислить ни в одной точке.\n");
                        continue;
                    }
                    int axis_col = (ymin <= 0.0 && ymax >= 0.0) ? y_to_col(0.0, ymin, ymax, GRAPH_WIDTH) : -1;
                    printf("\nГрафик %s (y от %.4f до %.4f, '|' - ось y = 0)\n\n", name, ymin, ymax);
                    for (int i = i_first; i <= i_last; i++) {
                        x = xmin + i * dx;
                        int point_col = (calc_func(f, x, &y) == 0) ? y_to_col(y, ymin, ymax, GRAPH_WIDTH) : -1;
                        printf("%10.4f :", x);
                        for (int col = 0; col < GRAPH_WIDTH; col++) {
                            if (col == point_col) putchar('*');
                            else if (col == axis_col) putchar('|');
                            else putchar(' ');
                        }
                        putchar('\n');
                    }
                }
                }
} while (choice != 0);
return 0;
}
