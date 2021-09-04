#include <iostream>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <string>
#include <vector>

#define INF 100000 //значение бесконечности
#define SEP "------------------------------------------------------------------------------------------------------------------"

using namespace std;

class Timer //таймер для подсчета времени
{
private:
	using clock_t = std::chrono::high_resolution_clock;
	using second_t = std::chrono::duration<double, std::ratio<1> >;

	std::chrono::time_point<clock_t> m_beg;

public:
	Timer() : m_beg(clock_t::now()) //конструктор
	{
	}

	void reset() //сброс
	{
		m_beg = clock_t::now();
	}

	double elapsed() const //получить время
	{
		return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
	}
};

template<class T>
class Vertex { //вершина графа
	T data; //данные в вершине
public:
	Vertex(T d = 0) : data(d) {} //конструктор
	Vertex(const Vertex<T>& v) : data(v.data) { //конструктор копирования
	}

	T getData() { //селектор
		return data;
	}
	template<class T> friend istream& operator>> (istream& in, Vertex<T>& v); //перегрузка оператора ввода 
	template<class T> friend ostream& operator<< (ostream& out, const Vertex<T>& v); //перегрузка оператора вывода 
};

template<class T>
istream& operator>>(istream& in, Vertex<T>& v) //перегрузка оператора ввода 
{
	in >> v.data;
	return in;
}

template<class T>
ostream& operator<< (ostream& out, const Vertex<T>& v) { //перегрузка оператора вывода 
	out << v.data;
	return out;
}

class Edge { //ребро графа
public:
	int start, end; //начальная и конечная вершина
	double weigth; //вес ребра
	Edge(int w1 = -1, int w2 = -1, double w = 0) : start(w1), end(w2), weigth(w) {}; //конструктор
	friend ostream& operator<< (ostream& out, const Edge& obj); //перегрузка оператора вывода 
	friend istream& operator>> (istream& in, Edge& e); //перегрузка оператора ввода 
};

ostream& operator << (ostream& out, const Edge& e) //перегрузка оператора вывода 
{
	if (typeid(out).name() == typeid(ofstream).name()) //вывод в файл
	{
		out << e.start << " " << e.end << " " << e.weigth;
		return out;
	}

	out << e.start << "---" << e.weigth << "--->" << e.end; //вывод в консоль
	return out;
}
istream& operator>>(istream& in, Edge& e) //перегрузка оператора ввода 
{
	in >> e.start >> e.end >> e.weigth;
	return in;
}

template<class T>
class GraphVE { //граф
	Vertex<T>* verts; //указатель на массив вершин
	Edge* edges; //указатель на массив ребер
	int vn; //количество вершин
	int en; //количество ребер
	double* arr; //указатель на матрицу смежности в двумерном представлении
public:
	GraphVE() : verts(NULL), edges(NULL), arr(NULL), vn(0), en(0) {}; //конструктор
	GraphVE(string path); //конструктор с инициализацией графа информацией из файла
	~GraphVE(); //деструктор
	void print_matr(); //вывод матрицы смежности
	void init(); //инициализация матрицы смежности
	int get_vn() { return vn; }; //селектор количества вершин

	//неоптимизированные методы
	void floid_unoptimized(int start, int stop, bool flag = false); //запуск алгоритма Флойда, по умолчанию поиск надежнейшего пути выключен
	void floyd_cycle_unoptimized(vector<vector<double>>& a, vector<vector<int>>& way); //основной цикл алгоритма
	void find_path_unoptimized(vector<vector<int>>& way, vector<int>& path, int start, int stop); //поиск пути

	//оптимизированные методы
	void floid_optimized(int start, int stop, bool flag = false); //запуск алгоритма Флойда, по умолчанию поиск надежнейшего пути выключен
	void floyd_cycle_optimized(double* a, int** way); //основной цикл алгоритма
	void find_path_optimized(int** way, vector<int>& path, int start, int stop); //поиск пути

	template<class T> friend istream& operator>> (istream& in, GraphVE<T>& g); //перегрузка оператора ввода 
	template<class T> friend ostream& operator<< (ostream& out, const GraphVE<T>& g); //перегрузка оператора вывода 

	vector <int> path_min; //путь минимальной стоимости
	vector <int> path_safe; //путь максимальной надежности

	void print_path(vector <int>& path) { //вывод пути
		for (auto i = path.begin(); i < path.end() - 1; i++) {
			cout << *i << "--->";
		}
		cout << *(path.end() - 1);
	}

	double time_unoptimized; //время неоптимизированного метода
	double time_optimized; //время оптимизированного метода
};




template<class T>
GraphVE<T>::GraphVE(string path) :verts(NULL), edges(NULL), arr(NULL), vn(0), en(0) //конструктор
{
	ifstream file(path);
	if (!file) {
		cout << "Ошибка чтения файла";
		return;
	}
	file >> *this;
	file.close();
}

template<class T>
GraphVE<T>::~GraphVE() //деструктор
{
	delete[] verts;
	delete[] edges;
	delete[] arr;

}

template<class T>
void GraphVE<T>::print_matr() { //вывод матрицы смежности
	if (arr == NULL) {
		cout << "Граф не инициализирован";
		return;
	}
	double* end = arr + vn * vn;
	double* endi;

	for (double* i = arr; i < end; i += vn) {
		endi = i + vn;
		for (double* j = i; j < endi; j++) {
			cout << setw(10) << *j;
		}
		cout << endl;
	}
}

template<class T>
void GraphVE<T>::init() //проинициализировать матрицу смежности
{
	delete[] arr; //очистить, если что-то было
	arr = new double[vn * vn];

	double* end = arr + vn * vn;
	for (double* i = arr; i < end; i++) *i = INF;

	Edge* ende = edges + en;

	for (Edge* i = edges; i < ende; i++) {
		arr[(i->start - 1) * vn + (i->end - 1)] = i->weigth;
		//смещение на 1, предполагая нумерацию вершин с 1
	}

	for (double* i = arr; i < end; i += vn + 1) {
		*i = 0;
		//обнулить петли и бесконечности на главной диагонали
	}
}

template<class T>
istream& operator>>(istream& in, GraphVE<T>& g) //перегрузка оператора ввода 
{
	in >> g.vn;
	delete[] g.verts; //очистить, если что-то было
	g.verts = new Vertex<T>[g.vn];
	Vertex<T>* endv = g.verts + g.vn;
	for (Vertex<T>* i = g.verts; i < endv; i++) {
		in >> *i;
	}

	in >> g.en;
	delete[] g.edges; //очистить, если что-то было
	g.edges = new Edge[g.en];
	Edge* ende = g.edges + g.en;
	for (Edge* i = g.edges; i < ende; i++) {
		in >> *i;
	}
	g.init();
	return in;
}

template<class T>
ostream& operator<< (ostream& out, const GraphVE<T>& g) { //перегрузка оператора вывода 
	Edge* ende = g.edges + g.en;

	if (typeid(out).name() == typeid(ofstream).name()) //вывод в файл
	{
		out << g.vn << endl;

		Vertex<T>* endv = g.verts + g.vn;
		for (Vertex<T>* i = g.verts; i < endv; i++) {
			out << *i << " ";
		}

		out << endl << g.en << endl;

		for (Edge* i = g.edges; i < ende; i++) {
			out << *i << endl;
		}
		return out;
	}

	cout.unsetf(ios_base::floatfield);
	out << "Граф состоит из " << g.vn << " вершин и " << g.en << " рёбер" << endl; //вывод в консоль

	if (g.en < 100) { //если рёбер менее 100, вывести информацию о них
		int j = 1;
		out << "№ ребра: Начальная вершина---Вес--->Конечная вершина" << endl;
		for (Edge* i = g.edges; i < ende; i++) {
			out << j++ << ": " << *i << endl;
		}
	}

	return out;
}


template<class T>
void GraphVE<T>::floyd_cycle_unoptimized(vector<vector<double>>& a, vector<vector<int>>& way) //основной цикл алгоритма (неопт)
{
	for (int k = 0; k < vn; k++) { //итерации
		for (int i = 0; i < vn; i++) { //номер строки
			for (int j = 0; j < vn; j++) { //номер столбца
				if (a[i][j] > a[i][k] + a[k][j]) {
					a[i][j] = a[i][k] + a[k][j]; //присвоить минимальное
					way[i][j] = k; //сохранить путь
				}
			}
		}
	}
}

template<class T>
void GraphVE<T>::floyd_cycle_optimized(double* a, int** way) //основной цикл алгоритма (опт)
{
	double* ik; //левый операнд сложения
	double* r; //правый операнд сложения
	int iw, jw; //индексы для сохранения пути

	for (int k = 0; k < vn; k++) { //итерации
		ik = a + k; //левый операнд сложения
		iw = 0;
		for (double* i = a; i < a + vn * vn; i += vn) {
			r = a + vn * k; //правый операнд сложения
			jw = 0;
			for (double* j = i; j < i + vn; j++) {
				if (*j > *ik + *r) {
					*j = *ik + *r; //присвоить минимальное
					way[iw][jw] = k; //сохранить путь
				}
				r++;
				jw++;
			}
			iw++;
			ik += vn;
		}
	}
}

template<class T>
void GraphVE<T>::find_path_unoptimized(vector<vector<int>>& way, vector<int>& path, int start, int stop) //поиск пути (неопт)
{
	path.clear(); //очистить путь
	int w = way[start - 1][stop - 1]; //получить индекс следующей вершины
	path.push_back(start); //записать начальную вершину
	while (w != -1) { //пока необходимо переходить в следующую вершину
		w = way[start - 1][stop - 1]; //получить индекс следующей вершины
		if (w == -1) break; //если переходить не надо, выходим
		start = w + 1; //переместиться на следующую вершину
		path.push_back(start); //записать текущую вершину
	}
	path.push_back(stop); //записать конечную вершину
}

template<class T>
void GraphVE<T>::find_path_optimized(int** way, vector<int>& path, int start, int stop) //поиск пути (опт)
{
	path.clear(); //очистить путь
	int w = way[start - 1][stop - 1]; //получить индекс следующей вершины
	path.push_back(start); //записать начальную вершину
	while (w != -1) { //пока необходимо переходить в следующую вершину
		w = way[start - 1][stop - 1]; //получить индекс следующей вершины
		if (w == -1) break; //если переходить не надо, выходим
		start = w + 1; //переместиться на следующую вершину
		path.push_back(start); //записать текущую вершину
	}
	path.push_back(stop); //записать конечную вершину
}




template<class T>
void GraphVE<T>::floid_unoptimized(int start, int stop, bool flag)
{
	cout << endl << endl << endl << "выполнение неоптимизированного метода..." << endl << endl;
	//НЕОПТИМИЗИРОВАННО
	Timer t;

	//объявление матрицы минимальных путей с элементами, равными -1
	vector < vector <int > > way_min(vn, vector<int>(vn, -1));

	//объявление матрицы надежнейших путей с элементами, равными -1
	vector < vector <int > > way_safe(vn, vector<int>(vn, -1));

	//объявление матрицы кратчайших расстояний
	vector < vector <double> > a(vn, vector<double>(vn));

	//объявление матрицы надёжнейших значений
	vector < vector <double> > c(vn, vector<double>(vn));

	//копирование матрицы для обработки минимальных путей
	for (int i = 0; i < vn; i++) {
		for (int j = 0; j < vn; j++) {
			a[i][j] = arr[i * vn + j];
		}
	}

	//копирование матрицы для обработки надёжных путей
	if (flag)
		for (int i = 0; i < vn; i++) {
			for (int j = 0; j < vn; j++) {
				c[i][j] = (arr[i * vn + j] == INF || arr[i * vn + j] == 0) ? INF : -log10(arr[i * vn + j]);
			}
		}

	//реализация алгоритма Флойда
	floyd_cycle_unoptimized(a, way_min); //для минимального пути
	if (flag) floyd_cycle_unoptimized(c, way_safe); //для надежнейшего пути

	time_unoptimized = t.elapsed(); //получить время выполнения
	cout << scientific << "Время неоптимизированного метода: " << endl << "\t" << time_unoptimized << endl;

	bool f1 = false; //нет отрицательных циклов
	bool f2 = false; //путь из start в stop не может быть бесконечно малым

	int l = 0;
	while (!f2 && l < vn) {
		if (a[l][l] < 0) {
			f1 = true; //найден отрицательный цикл
			if (a[start - 1][l] < INF / 2 && a[l][stop - 1] < INF / 2) {
				f2 = true; //найден бесконечно малый путь из start в stop
			}
		}
		l++;
	}

	if (f1) { //если найден отрицательный цикл
		cout << "Обнаружен(ы) цикл(ы) отрицательного веса, в который(-ые) входят вершины: " << endl << "\t";
		for (int i = 0; i < vn; i++) {
			if (a[i][i] < 0) cout << i + 1 << " ";
		}
		cout << endl;
	}

	cout << "Минимальный путь: " << endl << "\t";
	if (f2) { //если найден бесконечно малый путь из start в stop
		cout << ">---путь может иметь бесконечно малую длину---<" << endl;
	}
	else {
		//нахождение путей
		if (a[start - 1][stop - 1] < INF / 2) { //если путь существует
			find_path_unoptimized(way_min, path_min, start, stop); //найти минимальный путь
			print_path(path_min); //вывести минимальный путь
			cout << endl << "Суммарный вес минимального пути: " << endl << "\t";
			cout.unsetf(ios_base::floatfield);
			cout << a[start - 1][stop - 1] << endl;

			if (flag) { //если включен поиск надежного пути
				cout << "Надёжный путь: " << endl << "\t";
				find_path_unoptimized(way_safe, path_safe, start, stop); //найти надежный путь
				print_path(path_safe); //вывести надежный путь
				cout << endl << "Значение надёжности пути: " << endl << "\t";
				cout << pow(10, -c[start - 1][stop - 1]);
			}
		}
		else {
			cout << ">---пути нет---<" << endl;
		}
	}
}


template<class T>
void GraphVE<T>::floid_optimized(int start, int stop, bool flag)
{
	cout << endl << endl << endl << "выполнение оптимизированного метода..." << endl << endl;
	//ОПТИМИЗИРОВАННО
	Timer t;

	//объявление матрицы минимальных путей с элементами, равными -1
	int** way_min = new int* [vn];
	int** endi = way_min + vn;
	int* endj;

	for (int** i = way_min; i < endi; i++) {
		*i = new int[vn];
		endj = *i + vn;
		for (int* j = *i; j < endj; j++) {
			*j = -1;
		}
	}

	//объявление матрицы надежнейших путей с элементами, равными -1
	int** way_safe = new int* [vn];
	endi = way_safe + vn;

	for (int** i = way_safe; i < endi; i++) {
		*i = new int[vn];
		endj = *i + vn;
		for (int* j = *i; j < endj; j++) {
			*j = -1;
		}
	}

	//объявление матрицы кратчайших расстояний
	double* a = new double[vn * vn];

	//объявление матрицы надёжнейших значений
	double* c = new double[vn * vn];

	//копирование матрицы для обработки минимальных путей
	double* p = arr;
	double* endk = a + vn * vn;
	for (double* k = a; k < endk; k++) {
		*k = *p++;
	}

	//копирование матрицы для обработки надёжных путей
	p = arr;
	endk = c + vn * vn;
	for (double* k = c; k < endk; k++) {
		*k = (*p == INF || *p == 0) ? INF : -log10(*p);
		p++;
	}

	//реализация алгоритма Флойда
	floyd_cycle_optimized(a, way_min); //для минимального пути
	if (flag) floyd_cycle_optimized(c, way_safe); //для надежнейшего пути

	time_optimized = t.elapsed(); //получить время выполнения
	cout << scientific << "Время оптимизированного метода: " << endl << "\t" << time_optimized << endl;

	bool f1 = false; //нет отрицательных циклов
	bool f2 = false; //путь из start в stop не может быть бесконечно малым

	int l = 0;
	while (!f2 && l < vn) {
		if (a[l * vn + l] < 0) {
			f1 = true; //найден отрицательный цикл
			if (a[(start - 1) * vn + l] < INF / 2 && a[l * vn + stop - 1] < INF / 2) {
				f2 = true; //найден бесконечно малый путь из start в stop
			}
		}
		l++;
	}

	if (f1) { //если найден отрицательный цикл
		cout << "Обнаружен(ы) цикл(ы) отрицательного веса, в который(-ые) входят вершины: " << endl << "\t";
		for (int i = 0; i < vn; i++) {
			if (a[i * vn + i] < 0) cout << i + 1 << " ";
		}
		cout << endl;
	}

	cout << "Минимальный путь: " << endl << "\t";
	if (f2) { //если найден бесконечно малый путь из start в stop
		cout << ">---путь может иметь бесконечно малую длину---<" << endl;
	}
	else {
		//нахождение пути
		if (a[(start - 1) * vn + stop - 1] < INF / 2) { //если путь существует
			find_path_optimized(way_min, path_min, start, stop);
			print_path(path_min); //вывести минимальный путь
			cout << endl << "Суммарный вес минимального пути: " << endl << "\t";
			cout.unsetf(ios_base::floatfield);
			cout << a[(start - 1) * vn + stop - 1] << endl;

			if (flag) { //если включен поиск надежного пути
				cout << "Надёжный путь: " << endl << "\t";
				find_path_optimized(way_safe, path_safe, start, stop); //найти надежный путь
				print_path(path_safe); //вывести надежный путь
				cout << endl << "Значение надёжности пути: " << endl << "\t";
				cout << pow(10, -c[(start - 1) * vn + stop - 1]);
			}
		}
		else {
			cout << ">---пути нет---<" << endl;
		}
	}

}

int main()
{
	setlocale(LC_ALL, "Russian");

	GraphVE<int> g("input.txt"); //инициализация графа информацией из файла
	cout << SEP << endl << g; //вывод информации о графе

	int start, stop;
	do {
		cout << "Введите начальную и конечную вершину: (от " << 1 << " до " << g.get_vn() << ") " << endl;
		cin >> start >> stop;
	} while (start<1 || stop<1 || start>g.get_vn() || stop > g.get_vn());

	//ofstream ofile("output.txt"); //возможность сохранить граф в новый файл
	//if (!ofile) {
	//	cout << "Ошибка чтения файла";
	//	exit(2);
	//}
	//ofile << g;
	//ofile.close();

	//g.print_matr(); //возможность вывести матрицу смежности

	g.floid_unoptimized(start, stop); //вызов неоптимизированного метода
	g.floid_optimized(start, stop); //вызов оптимизированного метода
	cout << fixed << endl << endl << "Оптимизированный метод выполнился в " << g.time_unoptimized / g.time_optimized << " раз быстрее неоптимизированного" << endl << SEP << endl;

	cout << "нажмите enter...";
	cin.get(); cin.get();

	//g.path_min; //возможность обратиться к минимальному пути


	GraphVE<int> h("input_300_10.txt"); //инициализация графа информацией из файла
	cout << h; //вывод информации о графе

	h.floid_unoptimized(1, 6); //вызов неоптимизированного метода
	h.floid_optimized(1, 6); //вызов оптимизированного метода
	cout << fixed << endl << endl << "Оптимизированный метод выполнился в " << h.time_unoptimized / h.time_optimized << " раз быстрее неоптимизированного" << endl << SEP << endl;



	GraphVE<int> z("otriz.txt"); //инициализация графа информацией из файла
	cout << z; //вывод информации о графе

	z.floid_unoptimized(1, 2); //вызов неоптимизированного метода
	z.floid_optimized(1, 2); //вызов оптимизированного метода
	cout << fixed << endl << endl << "Оптимизированный метод выполнился в " << z.time_unoptimized / z.time_optimized << " раз быстрее неоптимизированного" << endl << SEP << endl;



	GraphVE<int> p("probabilities.txt"); //инициализация графа информацией из файла
	cout << p; //вывод информации о графе

	p.floid_unoptimized(1, 6, true); //вызов неоптимизированного метода
	p.floid_optimized(1, 6, true); //вызов оптимизированного метода
	cout << fixed << endl << endl << "Оптимизированный метод выполнился в " << p.time_unoptimized / p.time_optimized << " раз быстрее неоптимизированного" << endl << SEP << endl;


	return 0;
}

