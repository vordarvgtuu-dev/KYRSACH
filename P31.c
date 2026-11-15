
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <windows.h> // не работает #include <locale.h>
#include <float.h>

#define EPS         1e-9  
#define EXP_LIMIT   700.0
#define GRAPH_WIDTH 60 
#define MAX_POINTS  500    

double func_y(double x);
double func_v(double x);
int    y_is_defined(double x);
int    v_is_defined(double x);
int    is_defined(int choice, double x);
int    y_in_range(double x);
int    v_in_range(double x);
int    in_range(int choice, double x);
int    can_compute(int choice, double x);
double func_value(int choice, double x);
int    count_points(double xmin, double xmax, double dx);
int    first_index(int type);
int    last_index(int type, double xmin, double xmax, double dx, int n);
double find_min(int choice, double xmin, double dx, int i_first, int i_last);
double find_max(int choice, double xmin, double dx, int i_first, int i_last);
int    to_column(double y, double ymin, double ymax, int width);
int    axis_column(double ymin, double ymax, int width);
char   plot_char(int col, int point_col, int axis_col);

int main(void)
{
    int choice, action, type;
    double xmin, xmax, dx;
    double x, y, ymin, ymax;
    int n, i, i_first, i_last, col, point_col, axis_col;
    int ch;

    SetConsoleOutputCP(65001);

    do
    {
        printf("\n Табулирование функции (Вариант 6)\n");
        printf("1 - Y(x) = ln(x^2) - x + 4\n");
        printf("2 - V(x) - кусочная функция\n");
        printf("0 - выход\n");
        printf("Ваш выбор: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Ошибка: нужно ввести число.\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            if (ch == EOF) return 0;
            choice = -1;
            continue;
        }

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

        printf("\nЧто сделать с функцией %s?\n", (choice == 1) ? "Y(x)" : "V(x)");
        printf("1 - вычислить значение в точке\n");
        printf("2 - таблица значений\n");
        printf("3 - график\n");
        printf("Ваш выбор: ");
        if (scanf("%d", &action) != 1)
        {
            printf("Ошибка: нужно ввести число.\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            if (ch == EOF) return 0;
            continue;
        }
        if (action < 1 || action > 3)
        {
            printf("Нет такого пункта меню.\n");
            continue;
        }

        if (action == 1)
        {
            printf("Введите x: ");
            if (scanf("%lf", &x) != 1)
            {
                printf("Ошибка: нужно ввести число.\n");
                while ((ch = getchar()) != '\n' && ch != EOF);
                if (ch == EOF) return 0;
                continue;
            }
            if (!isfinite(x))
            {
                printf("Ошибка: x должен быть конечным числом.\n");
                continue;
            }

            if (!is_defined(choice, x))
                printf("%s не определена в точке x = %g\n", (choice == 1) ? "Y(x)" : "V(x)", x);
            else if (!in_range(choice, x))
                printf("Значение %s в точке x = %g не помещается в double (переполнение)\n", (choice == 1) ? "Y(x)" : "V(x)", x);
            else
                printf("%s = %.6f при x = %g\n", (choice == 1) ? "Y(x)" : "V(x)", func_value(choice, x), x);
            continue;
        }

        printf("Тип промежутка:\n");
        printf("1 - отрезок      [xmin; xmax]\n");
        printf("2 - интервал     (xmin; xmax)\n");
        printf("3 - полуинтервал [xmin; xmax)\n");
        printf("4 - полуинтервал (xmin; xmax]\n");
        printf("Ваш выбор: ");
        if (scanf("%d", &type) != 1)
        {
            printf("Ошибка: нужно ввести число.\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            if (ch == EOF) return 0;
            continue;
        }
        if (type < 1 || type > 4)
        {
            printf("Нет такого типа промежутка.\n");
            continue;
        }

        printf("Введите начало и конец промежутка (xmin xmax): ");
        if (scanf("%lf %lf", &xmin, &xmax) != 2)
        {
            printf("Ошибка: нужно ввести два числа.\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            if (ch == EOF) return 0;
            continue;
        }
        printf("Введите шаг табуляции dx: ");
        if (scanf("%lf", &dx) != 1)
        {
            printf("Ошибка: нужно ввести число.\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            if (ch == EOF) return 0;
            continue;
        }

        if (!isfinite(xmin) || !isfinite(xmax) || !isfinite(dx))
        {
            printf("Ошибка: xmin, xmax и dx должны быть конечными числами.\n");
            continue;
        }
        if (xmin >= xmax)
        {
            printf("Ошибка: xmin должен быть меньше xmax.\n");
            continue;
        }
        if (dx <= 0.0)
        {
            printf("Ошибка: шаг должен быть больше нуля.\n");
            continue;
        }

        n = count_points(xmin, xmax, dx);
        if (n > MAX_POINTS)
        {
            printf("Слишком много точек (больше %d). Увеличьте шаг.\n", MAX_POINTS);
            continue;
        }

        i_first = first_index(type);
        i_last = last_index(type, xmin, xmax, dx, n);
        if (i_first > i_last)
        {
            printf("На этом промежутке с таким шагом нет ни одной точки. Уменьшите шаг.\n");
            continue;
        }


        if (action == 2)
        {
            printf("\n  Таблица значений функции %s\n", (choice == 1) ? "Y(x)" : "V(x)");
            printf("+------------+-----------------+\n");
            printf("|      x     |      %s       |\n", (choice == 1) ? "Y(x)" : "V(x)");
            printf("+------------+-----------------+\n");
            for (i = i_first; i <= i_last; i++)
            {
                x = xmin + i * dx;
                if (!is_defined(choice, x))
                {
                    printf("| %10.4f | %15s |\n", x, "не определена");
                }
                else if (!in_range(choice, x))
                {
                    printf("| %10.4f | %15s |\n", x, "переполнение");
                }
                else
                {
                    y = func_value(choice, x);
                    printf("| %10.4f | %15.6f |\n", x, y);
                }
            }
            printf("+------------+-----------------+\n");
        }

        else
        {
            ymin = find_min(choice, xmin, dx, i_first, i_last);
            ymax = find_max(choice, xmin, dx, i_first, i_last);
            if (ymin > ymax)
            {
                printf("Функцию нельзя вычислить ни в одной точке промежутка.\n");
                continue;
            }

            axis_col = axis_column(ymin, ymax, GRAPH_WIDTH);

            printf("\n  График %s (ось x - вниз, ось y - вправо)\n", (choice == 1) ? "Y(x)" : "V(x)");
            printf("  y от %.4f до %.4f, '|' - линия y = 0\n", ymin, ymax);
            printf("  пустая строка - функция в точке не определена или переполнение\n\n");

            for (i = i_first; i <= i_last; i++)
            {
                x = xmin + i * dx;
                if (can_compute(choice, x))
                    point_col = to_column(func_value(choice, x), ymin, ymax, GRAPH_WIDTH);
                else
                    point_col = -1;

                printf("%10.4f :", x);
                for (col = 0; col < GRAPH_WIDTH; col++)
                    putchar(plot_char(col, point_col, axis_col));
                printf("\n");
            }
        }

    } while (choice != 0);

    return 0;
}


double func_y(double x)
{
    return log(x * x) - x + 4.0;
}

double func_v(double x)
{
    if (x > 1.0)
        return fabs(x);
    else if (x <= -1.0)
        return 1.0 + x;
    else
        return sin(x);
}


int y_is_defined(double x)
{
    return (fabs(x) > EPS);
}

int v_is_defined(double x)
{
    (void)x;
    return 1;
}

int is_defined(int choice, double x)
{
    return (choice == 1) ? y_is_defined(x) : v_is_defined(x);
}

int y_in_range(double x)
{
    double val = func_y(x);
    return isfinite(val); 
}

int v_in_range(double x)
{
    double val = func_v(x);
    return isfinite(val);
}

int in_range(int choice, double x)
{
    return (choice == 1) ? y_in_range(x) : v_in_range(x);
}

int can_compute(int choice, double x)
{
    return is_defined(choice, x) && in_range(choice, x);
}

double func_value(int choice, double x)
{
    return (choice == 1) ? func_y(x) : func_v(x);
}

int count_points(double xmin, double xmax, double dx)
{
    double steps = (xmax - xmin) / dx + EPS;
    if (!isfinite(steps) || steps >= MAX_POINTS)
        return MAX_POINTS + 1;
    return (int)steps + 1;
}

int first_index(int type)
{
    return (type == 2 || type == 4) ? 1 : 0;
}

int last_index(int type, double xmin, double xmax, double dx, int n)
{
    int last = n - 1;
    if ((type == 2 || type == 3) && fabs(xmin + (n - 1) * dx - xmax) < dx * 1e-6)
        last = n - 2;
    return last;
}

double find_min(int choice, double xmin, double dx, int i_first, int i_last)
{
    double min = DBL_MAX;
    for (int i = i_first; i <= i_last; i++)
    {
        double x = xmin + i * dx;
        if (can_compute(choice, x))
        {
            double y = func_value(choice, x);
            if (y < min) min = y;
        }
    }
    return min;
}

double find_max(int choice, double xmin, double dx, int i_first, int i_last)
{
    double max = -DBL_MAX;
    for (int i = i_first; i <= i_last; i++)
    {
        double x = xmin + i * dx;
        if (can_compute(choice, x))
        {
            double y = func_value(choice, x);
            if (y > max) max = y;
        }
    }
    return max;
}

int to_column(double y, double ymin, double ymax, int width)
{
    if (ymax * 0.5 - ymin * 0.5 < EPS)
        return width / 2;
    return (int)((y * 0.5 - ymin * 0.5) / (ymax * 0.5 - ymin * 0.5) * (width - 1) + 0.5);
}

int axis_column(double ymin, double ymax, int width)
{
    if (ymin > 0.0 || ymax < 0.0)
        return -1;
    return to_column(0.0, ymin, ymax, width);
}

char plot_char(int col, int point_col, int axis_col)
{
    if (col == point_col) return '*';
    if (col == axis_col) return '|';
    return ' ';
}