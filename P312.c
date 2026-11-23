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

double func_y(double x);
double func_v(double x);
int calc_func(math_func f, double x, double* result);

point_t* table_func(math_func f, double xmin, double xmax, double dx);
point_t* sort_func(point_t* arr, char c);
void print_sorted_table(math_func f, double xmin, double xmax, double dx, char sort_c);

int count_points(double xmin, double xmax, double dx);
int get_start_idx(int type);
int get_end_idx(int type, double xmin, double xmax, double dx, int n);
double find_min(math_func f, double xmin, double dx, int i_first, int i_last);
double find_max(math_func f, double xmin, double dx, int i_first, int i_last);

int y_to_col(double y, double ymin, double ymax, int width);
int get_axis_col(double ymin, double ymax, int width);
char get_graph_char(int col, int point_col, int axis_col);
void clear_buf(void);

//         /
//         ⎪ |x|     при |x| > 1
// V(x) = <  0,       при x = -1
//         ⎪ sin(x),  при -1 < x <= 1
//         \ 

int main(void)
{
    int choice, action, type;
    double xmin, xmax, dx;
    double x, y, ymin, ymax;
    int n, i, i_first, i_last, col, point_col, axis_col;
    int status;
    math_func f;
    const char* name;
    char sort_c;
    setlocale(LC_ALL, "Russian");


    do
    {
        printf("\nТабулирование функции (Вариант 6)\n");
        printf("1 - Y(x) = ln(x^2) - x + 4\n");
        printf("2 - V(x) - кусочная функция\n");
        printf("0 - выход\n");
        printf("Ваш выбор: ");

        if (scanf("%d", &choice) != 1)
        {
            clear_buf();
            printf("Ошибка: нужно ввести число.\n");
            choice = -1;
            continue;
        }
        clear_buf();

        if (choice == 0)
        {
            printf("Работа программы завершена.\n");
            break;
        }
        if (choice != 1 && choice != 2)
        {
            printf("Нет такого пункта меню.\n");
            continue;
        }

        if (choice == 1) {
            f = func_y;
            name = "Y(x)";
        }
        else {
            f = func_v;
            name = "V(x)";
        }


        printf("\nЧто сделать с функцией %s?\n", name);
        printf("1 - вычислить значение в точке\n");
        printf("2 - таблица значений\n");
        printf("3 - график\n");
        printf("4 - набор введённых аргументов\n");
        printf("5 - десять наборов (xmin, dx)\n");
        printf("6 - N случайных аргументов на промежутке\n");
        printf("7 - ПЗ2: таблица с сортировкой (table_func + sort_func)\n");
        printf("Ваш выбор: ");


        if (scanf("%d", &action) != 1)
        {
            clear_buf();
            printf("Ошибка: нужно ввести число.\n");
            continue;
        }
        clear_buf();

        if (action < 1 || action > 7)
        {
            printf("Нет такого пункта меню.\n");
            continue;
        }

        if (action == 1)
        {
            printf("Введите x: ");
            if (scanf("%lf", &x) != 1)
            {
                clear_buf();
                printf("Ошибка: нужно ввести число.\n");
                continue;
            }
            clear_buf();

            status = calc_func(f, x, &y);
            if (status == 1)
                printf("%s не определена в точке x = %g\n", name, x);
            else if (status == 2)
                printf("Значение %s в точке x = %g не помещается в double\n", name, x);
            else
                printf("%s = %.6f при x = %g\n", name, y, x);
            continue;
        }

        if (action == 4)
        {
            int m, k;
            double xv, yv;

            printf("Сколько аргументов ввести? ");
            if (scanf("%d", &m) != 1 || m <= 0 || m > MAX_POINTS)
            {
                clear_buf();
                printf("Ошибка: число должно быть от 1 до %d.\n", MAX_POINTS);
                continue;
            }
            clear_buf();

            printf("\nТаблица значений для введённого набора аргументов\n");
            printf(".------------.-----------------.\n");
            printf("| x          | %-15s |\n", name);
            printf(".------------.-----------------.\n");
            for (k = 0; k < m; k++)
            {
                printf("Введите x[%d]: ", k + 1);
                if (scanf("%lf", &xv) != 1)
                {
                    clear_buf();
                    printf("Ошибка: нужно ввести число. Повторите ввод.\n");
                    k--;
                    continue;
                }
                clear_buf();

                status = calc_func(f, xv, &yv);
                if (status == 1)
                    printf("| %10.4f | %15s |\n", xv, "не определена");
                else if (status == 2)
                    printf("| %10.4f | %15s |\n", xv, "переполнение");
                else
                    printf("| %10.4f | %15.6f |\n", xv, yv);
            }
            printf(".------------.-----------------.\n");
            continue;

        }

        if (action == 5)
        {
            int k, j, cnt_def, cnt_ovf;
            double xmi, dxi, xv, yv, ylo, yhi;

            printf("\nДесять наборов (xmin, dx), по 10 точек в каждом\n");
            for (k = 0; k < 10; k++)
            {
                printf("Набор %d. Введите xmin и dx: ", k + 1);
                if (scanf("%lf %lf", &xmi, &dxi) != 2 || dxi <= 0.0)
                {
                    clear_buf();
                    printf("Ошибка: введите два числа, dx > 0.\n");
                    k--;
                    continue;
                }
                clear_buf();

                cnt_def = 0;
                cnt_ovf = 0;
                ylo = DBL_MAX;
                yhi = -DBL_MAX;

                printf(".------------.-----------------.\n");
                printf("| x          | %-15s |\n", name);
                printf(".------------.-----------------.\n");
                for (j = 0; j < 10; j++)
                {
                    xv = xmi + j * dxi;
                    status = calc_func(f, xv, &yv);
                    if (status == 1)
                    {
                        printf("| %10.4f | %15s |\n", xv, "не определена");
                    }
                    else if (status == 2)
                    {
                        cnt_ovf++;
                        printf("| %10.4f | %15s |\n", xv, "переполнение");
                    }
                    else
                    {
                        cnt_def++;
                        if (yv < ylo) ylo = yv;
                        if (yv > yhi) yhi = yv;
                        printf("| %10.4f | %15.6f |\n", xv, yv);
                    }
                }
                printf(".------------.-----------------.\n");
                printf("определено: %d, переполнено: %d", cnt_def, cnt_ovf);
                if (cnt_def > 0)
                    printf(", y_min = %.6f, y_max = %.6f", ylo, yhi);
                printf("\n");
            }
            continue;
        }

        if (action == 6)
        {
            int k, nn;
            double rmin, rmax, xv, yv;

            printf("Введите xmin, xmax и N: ");
            if (scanf("%lf %lf %d", &rmin, &rmax, &nn) != 3)
            {
                clear_buf();
                printf("Ошибка: нужно ввести два числа и целое число.\n");
                continue;
            }
            clear_buf();

            if (rmin >= rmax || nn <= 0 || nn > MAX_POINTS)
            {
                printf("Ошибка: проверьте границы и N (от 1 до %d).\n", MAX_POINTS);
                continue;
            }

            srand((unsigned)time(NULL));
            printf("\nТаблица значений для %d случайных аргументов на [%.4f; %.4f]\n", nn, rmin, rmax);
            printf(".------------.-----------------.\n");
            printf("| x          | %-15s |\n", name);
            printf(".------------.-----------------.\n");
            for (k = 0; k < nn; k++)
            {
                xv = rmin + (rmax - rmin) * ((double)rand() / RAND_MAX);
                status = calc_func(f, xv, &yv);
                if (status == 1)
                    printf("| %10.4f | %15s |\n", xv, "не определена");
                else if (status == 2)
                    printf("| %10.4f | %15s |\n", xv, "переполнение");
                else
                    printf("| %10.4f | %15.6f |\n", xv, yv);
            }
            printf(".------------.-----------------.\n");
            continue;
        }

        if (action == 7)
        {
            printf("Введите начало и конец промежутка (xmin xmax): ");
            if (scanf("%lf %lf", &xmin, &xmax) != 2)
            {
                clear_buf();
                printf("Ошибка: нужно ввести два числа.\n");
                continue;
            }
            clear_buf();

            printf("Введите шаг табуляции dx: ");
            if (scanf("%lf", &dx) != 1 || dx <= 0.0 || xmin >= xmax)
            {
                clear_buf();
                printf("Ошибка: проверьте корректность ввода xmin, xmax и dx.\n");
                continue;
            }
            clear_buf();

            printf("Введите вид сортировки (a/A - по возрастанию, d/D - по убыванию): ");
            if (scanf(" %c", &sort_c) != 1)
            {
                clear_buf();
                printf("Ошибка: нужно ввести символ.\n");
                continue;
            }
            clear_buf();

            if (sort_c != 'a' && sort_c != 'A' && sort_c != 'd' && sort_c != 'D')
            {
                printf("Ошибка: нужно ввести a, A, d или D.\n");
                continue;
            }

            print_sorted_table(f, xmin, xmax, dx, sort_c);
            continue;
        }

        printf("Тип промежутка:\n");
        printf("1 - отрезок [xmin; xmax]\n");
        printf("2 - интервал (xmin; xmax)\n");
        printf("3 - полуинтервал [xmin; xmax)\n");
        printf("4 - полуинтервал (xmin; xmax]\n");
        printf("Ваш выбор: ");
        if (scanf("%d", &type) != 1 || type < 1 || type > 4)
        {
            clear_buf();
            printf("Нет такого типа промежутка.\n");
            continue;
        }
        clear_buf();
        printf("Введите начало и конец промежутка (xmin xmax): ");
        if (scanf("%lf %lf", &xmin, &xmax) != 2)
        {
            clear_buf();
            printf("Ошибка: нужно ввести два числа.\n");
            continue;
        }
        clear_buf();
        printf("Введите шаг табуляции dx: ");
        if (scanf("%lf", &dx) != 1 || dx <= 0.0)
        {
            clear_buf();
            printf("Ошибка: шаг должен быть числом больше нуля.\n");
            continue;
        }
        clear_buf();
        if (xmin > xmax || (xmin == xmax && type != 1))
        {
            printf("Ошибка: некорректные границы промежутка.\n");
            continue;
        }
        n = count_points(xmin, xmax, dx);
        if (n > MAX_POINTS)
        {
            printf("Слишком много точек (максимум %d).\n", MAX_POINTS);
            continue;
        }
        i_first = get_start_idx(type);
        i_last = get_end_idx(type, xmin, xmax, dx, n);
        if (i_first > i_last)
        {
            printf("Нет ни одной точки на промежутке.\n");
            continue;
        }
        if (action == 2)
        {
            printf("\nТаблица значений функции %s\n", name);
            printf(".------------.-----------------.\n");
            printf("| x          | %-15s |\n", name);
            printf(".------------.-----------------.\n");
            for (i = i_first; i <= i_last; i++)
            {
                x = xmin + i * dx;
                status = calc_func(f, x, &y);
                if (status == 1)      printf("| %10.4f | %15s |\n", x, "не определена");
                else if (status == 2) printf("| %10.4f | %15s |\n", x, "переполнение");
                else                  printf("| %10.4f | %15.6f |\n", x, y);
            }
            printf(".------------.-----------------.\n");
        }
        else if (action == 3)
        {
            ymin = find_min(f, xmin, dx, i_first, i_last);
            ymax = find_max(f, xmin, dx, i_first, i_last);
            if (ymin > ymax)
            {
                printf("Функцию нельзя вычислить ни в одной точке.\n");
                continue;
            }
            axis_col = get_axis_col(ymin, ymax, GRAPH_WIDTH);
            printf("\nГрафик %s\n", name);
            printf("y от %.4f до %.4f, '|' - линия y = 0\n", ymin, ymax);
            printf("пустая строка - функция не определена\n\n");
            for (i = i_first; i <= i_last; i++)
            {
                x = xmin + i * dx;
                if (calc_func(f, x, &y) == 0)
                    point_col = y_to_col(y, ymin, ymax, GRAPH_WIDTH);
                else
                    point_col = -1;
                printf("%10.4f :", x);
                for (col = 0; col < GRAPH_WIDTH; col++)
                    putchar(get_graph_char(col, point_col, axis_col));
                printf("\n");
            }
        }
    } while (choice != 0);
    return 0;
}
void clear_buf(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
double func_y(double x)
{
    return log(x * x) - x + 4.0;
}
double func_v(double x)
{
    if (fabs(x) > 1.0)
        return fabs(x);
    else if (x <= -1.0)
        return 1.0 + x;
    else
        return sin(x);
}
int calc_func(math_func f, double x, double* result)
{
    double val;
    if (f == func_y && fabs(x) <= EPS)
        return 1;
    val = f(x);
    if (!isfinite(val))
        return 2;
    *result = val;
    return 0;
}
point_t* table_func(math_func f, double xmin, double xmax, double dx) 
{
    int n, i, k;
    point_t* arr;
    double x, y;
    n = count_points(xmin, xmax, dx);
    if (n > MAX_POINTS) n = MAX_POINTS;
    if (n <= 0) n = 1;
    arr = (point_t*)malloc((n + 1) * sizeof(point_t));
    if (arr == NULL) return NULL;
    k = 0;
    for (i = 0; i < n; i++)
    {
        x = xmin + i * dx;
        if (calc_func(f, x, &y) == 0)
        {
            arr[k].x = x;
            arr[k].y = y;
            k++;
        }
    }
    arr[k].x = 0.0;
    arr[k].y = NAN;
    return arr;
}
point_t* sort_func(point_t* arr, char c)
{
    int n, i, j, need_swap;
    point_t tmp;
    if (arr == NULL) return NULL;
    n = 0;
    while (!isnan(arr[n].y)) n++;
    if (n <= 1) return arr;
    for (i = 0; i < n - 1; i++)
    {
        for (j = 0; j < n - i - 1; j++)
        {
            need_swap = 0;
            if ((c == 'a' || c == 'A') && arr[j].y > arr[j + 1].y)
                need_swap = 1;
            if ((c == 'd' || c == 'D') && arr[j].y < arr[j + 1].y)
                need_swap = 1;
            if (need_swap)
            {
                tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
    return arr;
}


void print_sorted_table(math_func f, double xmin, double xmax, double dx, char sort_c)
{
    int n_total, n_valid, i;
    point_t* pts;
    n_total = count_points(xmin, xmax, dx);
    if (n_total > MAX_POINTS)
    {
        printf("Внимание: взяты первые %d точек из %d.\n", MAX_POINTS, n_total);
        n_total = MAX_POINTS;
    }
    if (n_total <= 0)
    {
        printf("Нет точек на промежутке.\n");
        return;
    }
    pts = table_func(f, xmin, xmax, dx);
    if (pts == NULL)
    {
        printf("Ошибка выделения памяти.\n");
        return;
    }
    n_valid = 0;
    while (!isnan(pts[n_valid].y)) n_valid++;
    sort_func(pts, sort_c);
    if (sort_c == 'a' || sort_c == 'A')
        printf("\nОтсортированная таблица (по возрастанию)\n");
    else
        printf("\nОтсортированная таблица (по убыванию)\n");
    printf(".------------.-----------------.\n");
    if (f == func_y)
        printf("| x          | Y(x)            |\n");
    else
        printf("| x          | V(x)            |\n");
    printf(".------------.-----------------.\n");
    for (i = 0; i < n_valid; i++)
        printf("| %10.4f | %15.6f |\n", pts[i].x, pts[i].y);
    printf(".------------.-----------------.\n");
    if (n_valid < n_total)
        printf("Внимание: %d точек не определены и не вошли в таблицу.\n", n_total - n_valid);
    free(pts);

}
int count_points(double xmin, double xmax, double dx)
{
    double steps = (xmax - xmin) / dx + EPS;
    if (steps >= MAX_POINTS)
        return MAX_POINTS + 1;
    if (steps < 0.0)
        return 0;
    return (int)steps + 1;
}
int get_start_idx(int type)
{
    if (type == 2 || type == 4) return 1;
    return 0;
}


int get_end_idx(int type, double xmin, double xmax, double dx, int n)
{
    int last = n - 1;
    if ((type == 2 || type == 3) && fabs(xmin + (n - 1) * dx - xmax) < dx * 1e-6)
        last = n - 2;
    return last;
}
double find_min(math_func f, double xmin, double dx, int i_first, int i_last)
{
    double min = DBL_MAX;
    double x, y;
    int i;
    for (i = i_first; i <= i_last; i++)
    {
        x = xmin + i * dx;
        if (calc_func(f, x, &y) == 0)
        {
            if (y < min) min = y;
        }
    }

    return min;
}
double find_max(math_func f, double xmin, double dx, int i_first, int i_last)
{
    double max = -DBL_MAX;
    double x, y;
    int i;
    for (i = i_first; i <= i_last; i++)
    {
        x = xmin + i * dx;
        if (calc_func(f, x, &y) == 0)
        {
            if (y > max) max = y;
        }
    }
    return max;
}
int y_to_col(double y, double ymin, double ymax, int width)
{
    double t;
    int col;
    if (fabs(ymax - ymin) < EPS)
        return width / 2;
    t = (y - ymin) / (ymax - ymin);
    col = (int)(t * (width - 1) + 0.5);
    if (col < 0) col = 0;
    if (col >= width) col = width - 1;
    return col;
}
int get_axis_col(double ymin, double ymax, int width)
{
    if (ymin > 0.0 || ymax < 0.0)
        return -1;
    return y_to_col(0.0, ymin, ymax, width);
}
char get_graph_char(int col, int point_col, int axis_col)
{
    if (col == point_col) return '*';
    if (col == axis_col)  return '|';
    return ' ';
}
