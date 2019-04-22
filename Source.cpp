#include <iostream>

#include <vector>
#include <queue>

#include <time.h>

struct Point {
    int _row_num;
    int _column_num;

    Point(int row_num, int column_num) : _row_num(row_num), _column_num(column_num) {}

    bool operator == (const Point point) const {
        return (_row_num == point._row_num && _column_num == point._column_num);
    }

    Point operator+ (const Point point) const {
        return Point(_row_num + point._row_num, _column_num + point._column_num);
    }
};

static const int kRowsCount    = 27; //количество строк
static const int kColumnsCount = 59; //количество столбцов
 
static const Point VectorOfOffsets[8] = { { 1, 0 }, { -1, 0  }, { 0, 1  }, { 0, -1 },
                                          { 1, 1 }, { -1, -1 }, { -1, 1 }, { 1, -1 }};

enum FieldObject : int
{
    Path = -2,
    Wall = -1,
    Empty = 0,
    Start = 1
};

class Field {
    int _field[kRowsCount][kColumnsCount];

    Point _start;
    Point _finish;

    bool IsOnField(const Point point) const {
        return (point._row_num    >= 0 && point._row_num    < kRowsCount && 
                point._column_num >= 0 && point._column_num < kColumnsCount);
    }
    
    bool IsStart(const Point point) const {
        return (point == _start);
    }

    bool IsFinish(const Point point) const {
        return (point == _finish);
    }

    bool IsWall(const Point point) const {
        return (_field[point._row_num][point._column_num] == FieldObject::Wall);
    }

    bool IsPath(const Point point) const {
        return (_field[point._row_num][point._column_num] == FieldObject::Path);
    }

    bool IsUnprocessed(const Point point) const {
        return (_field[point._row_num][point._column_num] == FieldObject::Empty);
    }

    bool IsProper(const Point point) const {
        return (!IsStart(point) && !IsWall(point) && (IsUnprocessed(point)));
    }

    int  Value(const Point point) const {
        return _field[point._row_num][point._column_num];
    }

    void SetValue(const Point point, int value) {
        _field[point._row_num][point._column_num] = value;
    }

    void MarkPath(const std::vector<Point>& path) {
        for (auto point : path) {
            SetValue(point, FieldObject::Path);
        }
    }
    
    bool ProceedPoint(const Point point_to_check, int current_value, std::queue <Point>& processing_order) {
        if (!IsOnField(point_to_check)) {
            return false;
        }

        //точка лежит в поле
        if (IsFinish(point_to_check)) {
            //цель найдена, повышаем значение на единицу и возвращаем true
            SetValue(point_to_check, current_value + 1);
            return true;
        }

        //на поле и не финиш
        if (IsProper(point_to_check)) {
            //не старт, не препятствие, не обрабатывалась раньше
            SetValue(point_to_check, current_value + 1);
            processing_order.push(point_to_check);
        }
        return false;
    }

    bool FindFinish() {
        std::queue <Point> processing_order;
        processing_order.push(_start); 

        while (!processing_order.empty()) {
            const Point processed_point = processing_order.front();
            processing_order.pop();

            const auto current_value = Value(processed_point);
            for (auto offset_point : VectorOfOffsets) {
                const Point point_to_check = processed_point + offset_point;
                if (ProceedPoint(point_to_check, current_value, processing_order)) {
                    return true;
                }
            }
        }
        return false;
    }

    std::vector <Point> GetWayToStart() const {
        std::vector <Point> path;
        path.push_back(_finish);

        while (!IsStart(path.back())) {
            const Point processed_point = path.back();

            for (auto offset : VectorOfOffsets) {
                 const Point point_to_check(processed_point + offset);

                if (!IsOnField(point_to_check)) {
                    continue;
                }

                if (Value(processed_point) - 1 == Value(point_to_check)) {    
                    path.push_back(point_to_check);
                    break;
                }
            }
        }
        return path;
    }
public:

    Field(const Point start, const Point finish) 
        : _start(start),
          _finish(finish)
        {
        for (int i = 0; i < kRowsCount; ++i) {
            for (int j = 0; j < kColumnsCount; ++j) {
                _field[i][j] = FieldObject::Empty;
            }
        }
        SetValue(_start, FieldObject::Start);
    }

    bool SetWall(const Point point) {
        if (IsProper(point)) {
            _field[point._row_num][point._column_num] = FieldObject::Wall;
            return true;
        }
        return false;
    }

    void Print() const {
        for (int i = 0; i < kRowsCount; ++i) {
            for (int j = 0; j < kColumnsCount; ++j) {
                if (IsStart(Point(i, j))) {
                    std::cout << "S" << " ";
                    continue;
                }
                if (IsFinish(Point(i, j))) {
                    std::cout << "F" << " ";
                    continue;
                }
                if (IsWall(Point(i, j))) { 
                    std::cout << "#" << " ";
                    continue;
                }
                if (IsPath(Point(i, j))) {
                    std::cout << "^" << " ";
                    continue;
                }
                std::cout << " " << " ";
            }
            std::cout << std::endl;
        }
    }

    void GetPath() {
        if (FindFinish()) {
            const auto path = GetWayToStart();
            MarkPath(path);
        }
    }
};

void Test() {
    const Point start_point(rand() % kRowsCount, rand() % kColumnsCount);

    Point finish_point(rand() % kRowsCount, rand() % kColumnsCount);

    while (start_point == finish_point) {
        finish_point._row_num = rand() % kRowsCount;
        finish_point._column_num = rand() % kColumnsCount;
    }

    Field tested_field(start_point, finish_point);
    int number_of_walls = rand() % ((kRowsCount * kColumnsCount) / 2);
    int walls_counter = 0;
    while (walls_counter != number_of_walls) {
        const Point wall_point(rand() % kRowsCount, rand() % kColumnsCount);
        if (tested_field.SetWall(wall_point)) {
            walls_counter++;
        }
    }

    tested_field.GetPath();
    tested_field.Print();
    std::cout << std::endl;
}

int main() {

    srand(static_cast<unsigned int>(time(0)));

    for (int i = 0; i < 1000; ++i) {
        Test();
        std::cout << i << std::endl;
        system("pause");
    }
    system("pause");
}