#include <stdio.h>
#include <string.h>
#include <iostream>
#include <malloc.h>

using namespace std;

struct Point
{
	double x;
	double y;
	double z;

	Point(double x, double y, double z) : x(x), y(y), z(z) {}
	Point(const Point &point){
		x = point.x;
		y = point.y;
		z = point.z;
	}
	Point() {
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}
};

struct Plane
{
    double A;
	double B;
	double C;
	double D;

	Plane(double a, double b, double c, double d) : A(a), B(b), C(c), D(d) {}
};

double pointToPlaneDistance(Point point, Plane plane) {
	//функция считает расстояние от точки до плоскости
	//если плоскость задана таким образом, что в знаменателе уравнения получится 0, вернёт -1
	double minusOne = -1.0;

	//подпрограмма могла быть написана с использованием цикла, но в итоге он всё равно преобразуется в набор последовательных команд, а так по крайней мере повышается читаемость кода
	__asm {
		finit

		fld [plane].A              // загрузили в стек число plane.A
		fmul st(0), st(0)          // получили plane.A*plane.A

		fld [plane].A              // загрузили в стек число plane.A
		fld [point].x              // загрузили в стек число point.x
		fmulp st(1), st(0)         // получили plane.A*point.x
		//сейчас 
		//st(0) = plane.A*point.x 
		//st(1) =  plane.A*plane.A

		fld[plane].B               // загрузили в стек число plane.B
		fmul st(0), st(0)          // получили plane.B*plane.B

		fld[plane].B               // загрузили в стек число plane.A
		fld[point].y               // загрузили в стек число point.x
		fmulp st(1), st(0)         // получили plane.A*point.x
		//сейчас стек выглядит так: 
		//st(0) = B*y 
		//st(1) = B*B
		//st(2) = A*x 
		//st(3) = A*A

		faddp st(2), st(0)
		//сейчас стек выглядит так:
		//st(0) = B*B
		//st(1) = A*x + B*y
		//st(2) = A*A

		faddp st(2), st(0)
		//сейчас стек выглядит так:
		//st(1) = A*x + B*y
		//st(2) = A*A + B*B

		fld[plane].C               // загрузили в стек число plane.C
		fmul st(0), st(0)          // получили plane.C*plane.C

		fld[plane].C               // загрузили в стек число plane.C
		fld[point].z               // загрузили в стек число point.z
		fmulp st(1), st(0)         // получили plane.C*point.z
		//сейчас стек выглядит так:
		//st(0) = C*z 
		//st(1) = C*C
		//st(2) = A*x + B*y
		//st(3) = A*A + B*B

		faddp st(2), st(0)
		//сейчас стек выглядит так:  
		//st(0) = C*C
		//st(1) = A*x + B*y + C*z
		//st(2) = A*A + B*B

		faddp st(2), st(0)
		//сейчас стек выглядит так:  
		//st(0) = A*x + B*y + C*z
		//st(1) = A*A + B*B + C*C

		fld [plane].D
		faddp st(1), st(0)
		//сейчас стек выглядит так:  
		//st(0) = A*x + B*y + C*z + D
		//st(1) = A*A + B*B + C*C

		ftst                        // if st(0) < 0
		fstsw ax                    // хотел использовать ftsti
		sahf                        // ftsti не работает в asm вставках
		jb get_Module               // true -> получим модуль числа
		jae EX1                     // false -> число останется неизменным

		get_Module:
		fld minusOne
		fmulp st(1), st(0) // умножаем число st(1) на st(0) 

		EX1:                        
		//сейчас стек выглядит так:  
		//st(0) = |A*x + B*y + C*z + D|
		//st(1) = A*A + B*B + C*C
		
		fld st(1)
		ftst                        // if st(0) == 0
		fstsw ax                    // хотел использовать ftsti
		sahf
		jne NonError

		fld minusOne //просто добавим в стек -1 и сразу выйдем из программы, если знаменатель == 0
		jmp EXIT
		NonError:
		fsqrt 
		//сейчас стек выглядит так:  
		//st(0) = (A*A + B*B + C*C)^1/2
		//st(1) = A*x + B*y + C*z + D
		//st(2) = (A*A + B*B + C*C)

		fdivp st(1), st(0)
		//сейчас стек выглядит так:  
		//st(0) = |A*x + B*y + C*z + D| / ((A*A + B*B + C*C)^1/2)
		//st(1) = (A*A + B*B + C*C)
		// в вершине st(0) лежит число, обозначающее расстояние от точки до плоскости, можем выходить
		EXIT:
	}
}

Point findNearestPointToPlane(Point* pointArray, size_t sizePointArray, Plane plane) {
	Point res = pointArray[0];
	double minimalDistance = pointToPlaneDistance(pointArray[0], plane);
	if (minimalDistance == -1) throw "Не корректно введена плоскость";

	for (size_t i = 1;i < sizePointArray;i++) {
		if (pointToPlaneDistance(pointArray[i], plane) < minimalDistance) {
			//cout << minimalDistance << "\n";
			minimalDistance = pointToPlaneDistance(pointArray[i], plane);
			res = pointArray[i];
		}
	}
	return res;
}

int main() {
	setlocale(LC_ALL, "Russian");
	size_t sizePointArray = 0;
	cout << "Данная программа находит точку из заданного массива, которая ближе всего находится к заданной плоскости\n"
		<< "Пожалуйста, введите желаемое число точек\n";
	try {
		cin >> sizePointArray;
		Point* pointArray = new Point[sizePointArray];
		for (size_t i = 0;i < sizePointArray;i++) {
			cout << "Введите 3 числа, для того, чтобы задать " << i + 1 << "-ую точку массива\n";
			cin >> pointArray[i].x >> pointArray[i].y >> pointArray[i].z;
		}

		Plane plane = Plane(1.0, 1.0, 1.0, 1.0);
		cout << "Введите 4 числа, для того, чтобы задать плоскость через уравнение Ax+By+Cx+D=0\n";
		cin >> plane.A >> plane.B >> plane.C >> plane.D;
		Point res = findNearestPointToPlane(pointArray, sizePointArray, plane);
		cout << "Ближайшая точка это:\n" << res.x << " " << res.y << " " << res.z << "\n";
	}
	catch (const char* msg) {
		cerr << msg << endl;
	}
	
}