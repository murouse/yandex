#include <iostream>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <string>
#include <vector>

#define INF 100000 //�������� �������������
#define SEP "------------------------------------------------------------------------------------------------------------------"

using namespace std;

class Timer //������ ��� �������� �������
{
private:
	using clock_t = std::chrono::high_resolution_clock;
	using second_t = std::chrono::duration<double, std::ratio<1> >;

	std::chrono::time_point<clock_t> m_beg;

public:
	Timer() : m_beg(clock_t::now()) //�����������
	{
	}

	void reset() //�����
	{
		m_beg = clock_t::now();
	}

	double elapsed() const //�������� �����
	{
		return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
	}
};

template<class T>
class Vertex { //������� �����
	T data; //������ � �������
public:
	Vertex(T d = 0) : data(d) {} //�����������
	Vertex(const Vertex<T>& v) : data(v.data) { //����������� �����������
	}

	T getData() { //��������
		return data;
	}
	template<class T> friend istream& operator>> (istream& in, Vertex<T>& v); //���������� ��������� ����� 
	template<class T> friend ostream& operator<< (ostream& out, const Vertex<T>& v); //���������� ��������� ������ 
};

template<class T>
istream& operator>>(istream& in, Vertex<T>& v) //���������� ��������� ����� 
{
	in >> v.data;
	return in;
}

template<class T>
ostream& operator<< (ostream& out, const Vertex<T>& v) { //���������� ��������� ������ 
	out << v.data;
	return out;
}

class Edge { //����� �����
public:
	int start, end; //��������� � �������� �������
	double weigth; //��� �����
	Edge(int w1 = -1, int w2 = -1, double w = 0) : start(w1), end(w2), weigth(w) {}; //�����������
	friend ostream& operator<< (ostream& out, const Edge& obj); //���������� ��������� ������ 
	friend istream& operator>> (istream& in, Edge& e); //���������� ��������� ����� 
};

ostream& operator << (ostream& out, const Edge& e) //���������� ��������� ������ 
{
	if (typeid(out).name() == typeid(ofstream).name()) //����� � ����
	{
		out << e.start << " " << e.end << " " << e.weigth;
		return out;
	}

	out << e.start << "---" << e.weigth << "--->" << e.end; //����� � �������
	return out;
}
istream& operator>>(istream& in, Edge& e) //���������� ��������� ����� 
{
	in >> e.start >> e.end >> e.weigth;
	return in;
}

template<class T>
class GraphVE { //����
	Vertex<T>* verts; //��������� �� ������ ������
	Edge* edges; //��������� �� ������ �����
	int vn; //���������� ������
	int en; //���������� �����
	double* arr; //��������� �� ������� ��������� � ��������� �������������
public:
	GraphVE() : verts(NULL), edges(NULL), arr(NULL), vn(0), en(0) {}; //�����������
	GraphVE(string path); //����������� � �������������� ����� ����������� �� �����
	~GraphVE(); //����������
	void print_matr(); //����� ������� ���������
	void init(); //������������� ������� ���������
	int get_vn() { return vn; }; //�������� ���������� ������

	//������������������ ������
	void floid_unoptimized(int start, int stop, bool flag = false); //������ ��������� ������, �� ��������� ����� ������������ ���� ��������
	void floyd_cycle_unoptimized(vector<vector<double>>& a, vector<vector<int>>& way); //�������� ���� ���������
	void find_path_unoptimized(vector<vector<int>>& way, vector<int>& path, int start, int stop); //����� ����

	//���������������� ������
	void floid_optimized(int start, int stop, bool flag = false); //������ ��������� ������, �� ��������� ����� ������������ ���� ��������
	void floyd_cycle_optimized(double* a, int** way); //�������� ���� ���������
	void find_path_optimized(int** way, vector<int>& path, int start, int stop); //����� ����

	template<class T> friend istream& operator>> (istream& in, GraphVE<T>& g); //���������� ��������� ����� 
	template<class T> friend ostream& operator<< (ostream& out, const GraphVE<T>& g); //���������� ��������� ������ 

	vector <int> path_min; //���� ����������� ���������
	vector <int> path_safe; //���� ������������ ����������

	void print_path(vector <int>& path) { //����� ����
		for (auto i = path.begin(); i < path.end() - 1; i++) {
			cout << *i << "--->";
		}
		cout << *(path.end() - 1);
	}

	double time_unoptimized; //����� ������������������� ������
	double time_optimized; //����� ����������������� ������
};




template<class T>
GraphVE<T>::GraphVE(string path) :verts(NULL), edges(NULL), arr(NULL), vn(0), en(0) //�����������
{
	ifstream file(path);
	if (!file) {
		cout << "������ ������ �����";
		return;
	}
	file >> *this;
	file.close();
}

template<class T>
GraphVE<T>::~GraphVE() //����������
{
	delete[] verts;
	delete[] edges;
	delete[] arr;

}

template<class T>
void GraphVE<T>::print_matr() { //����� ������� ���������
	if (arr == NULL) {
		cout << "���� �� ���������������";
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
void GraphVE<T>::init() //������������������� ������� ���������
{
	delete[] arr; //��������, ���� ���-�� ����
	arr = new double[vn * vn];

	double* end = arr + vn * vn;
	for (double* i = arr; i < end; i++) *i = INF;

	Edge* ende = edges + en;

	for (Edge* i = edges; i < ende; i++) {
		arr[(i->start - 1) * vn + (i->end - 1)] = i->weigth;
		//�������� �� 1, ����������� ��������� ������ � 1
	}

	for (double* i = arr; i < end; i += vn + 1) {
		*i = 0;
		//�������� ����� � ������������� �� ������� ���������
	}
}

template<class T>
istream& operator>>(istream& in, GraphVE<T>& g) //���������� ��������� ����� 
{
	in >> g.vn;
	delete[] g.verts; //��������, ���� ���-�� ����
	g.verts = new Vertex<T>[g.vn];
	Vertex<T>* endv = g.verts + g.vn;
	for (Vertex<T>* i = g.verts; i < endv; i++) {
		in >> *i;
	}

	in >> g.en;
	delete[] g.edges; //��������, ���� ���-�� ����
	g.edges = new Edge[g.en];
	Edge* ende = g.edges + g.en;
	for (Edge* i = g.edges; i < ende; i++) {
		in >> *i;
	}
	g.init();
	return in;
}

template<class T>
ostream& operator<< (ostream& out, const GraphVE<T>& g) { //���������� ��������� ������ 
	Edge* ende = g.edges + g.en;

	if (typeid(out).name() == typeid(ofstream).name()) //����� � ����
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
	out << "���� ������� �� " << g.vn << " ������ � " << g.en << " ����" << endl; //����� � �������

	if (g.en < 100) { //���� ���� ����� 100, ������� ���������� � ���
		int j = 1;
		out << "� �����: ��������� �������---���--->�������� �������" << endl;
		for (Edge* i = g.edges; i < ende; i++) {
			out << j++ << ": " << *i << endl;
		}
	}

	return out;
}


template<class T>
void GraphVE<T>::floyd_cycle_unoptimized(vector<vector<double>>& a, vector<vector<int>>& way) //�������� ���� ��������� (�����)
{
	for (int k = 0; k < vn; k++) { //��������
		for (int i = 0; i < vn; i++) { //����� ������
			for (int j = 0; j < vn; j++) { //����� �������
				if (a[i][j] > a[i][k] + a[k][j]) {
					a[i][j] = a[i][k] + a[k][j]; //��������� �����������
					way[i][j] = k; //��������� ����
				}
			}
		}
	}
}

template<class T>
void GraphVE<T>::floyd_cycle_optimized(double* a, int** way) //�������� ���� ��������� (���)
{
	double* ik; //����� ������� ��������
	double* r; //������ ������� ��������
	int iw, jw; //������� ��� ���������� ����

	for (int k = 0; k < vn; k++) { //��������
		ik = a + k; //����� ������� ��������
		iw = 0;
		for (double* i = a; i < a + vn * vn; i += vn) {
			r = a + vn * k; //������ ������� ��������
			jw = 0;
			for (double* j = i; j < i + vn; j++) {
				if (*j > *ik + *r) {
					*j = *ik + *r; //��������� �����������
					way[iw][jw] = k; //��������� ����
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
void GraphVE<T>::find_path_unoptimized(vector<vector<int>>& way, vector<int>& path, int start, int stop) //����� ���� (�����)
{
	path.clear(); //�������� ����
	int w = way[start - 1][stop - 1]; //�������� ������ ��������� �������
	path.push_back(start); //�������� ��������� �������
	while (w != -1) { //���� ���������� ���������� � ��������� �������
		w = way[start - 1][stop - 1]; //�������� ������ ��������� �������
		if (w == -1) break; //���� ���������� �� ����, �������
		start = w + 1; //������������� �� ��������� �������
		path.push_back(start); //�������� ������� �������
	}
	path.push_back(stop); //�������� �������� �������
}

template<class T>
void GraphVE<T>::find_path_optimized(int** way, vector<int>& path, int start, int stop) //����� ���� (���)
{
	path.clear(); //�������� ����
	int w = way[start - 1][stop - 1]; //�������� ������ ��������� �������
	path.push_back(start); //�������� ��������� �������
	while (w != -1) { //���� ���������� ���������� � ��������� �������
		w = way[start - 1][stop - 1]; //�������� ������ ��������� �������
		if (w == -1) break; //���� ���������� �� ����, �������
		start = w + 1; //������������� �� ��������� �������
		path.push_back(start); //�������� ������� �������
	}
	path.push_back(stop); //�������� �������� �������
}




template<class T>
void GraphVE<T>::floid_unoptimized(int start, int stop, bool flag)
{
	cout << endl << endl << endl << "���������� ������������������� ������..." << endl << endl;
	//�����������������
	Timer t;

	//���������� ������� ����������� ����� � ����������, ������� -1
	vector < vector <int > > way_min(vn, vector<int>(vn, -1));

	//���������� ������� ����������� ����� � ����������, ������� -1
	vector < vector <int > > way_safe(vn, vector<int>(vn, -1));

	//���������� ������� ���������� ����������
	vector < vector <double> > a(vn, vector<double>(vn));

	//���������� ������� ���������� ��������
	vector < vector <double> > c(vn, vector<double>(vn));

	//����������� ������� ��� ��������� ����������� �����
	for (int i = 0; i < vn; i++) {
		for (int j = 0; j < vn; j++) {
			a[i][j] = arr[i * vn + j];
		}
	}

	//����������� ������� ��� ��������� ������� �����
	if (flag)
		for (int i = 0; i < vn; i++) {
			for (int j = 0; j < vn; j++) {
				c[i][j] = (arr[i * vn + j] == INF || arr[i * vn + j] == 0) ? INF : -log10(arr[i * vn + j]);
			}
		}

	//���������� ��������� ������
	floyd_cycle_unoptimized(a, way_min); //��� ������������ ����
	if (flag) floyd_cycle_unoptimized(c, way_safe); //��� ������������ ����

	time_unoptimized = t.elapsed(); //�������� ����� ����������
	cout << scientific << "����� ������������������� ������: " << endl << "\t" << time_unoptimized << endl;

	bool f1 = false; //��� ������������� ������
	bool f2 = false; //���� �� start � stop �� ����� ���� ���������� �����

	int l = 0;
	while (!f2 && l < vn) {
		if (a[l][l] < 0) {
			f1 = true; //������ ������������� ����
			if (a[start - 1][l] < INF / 2 && a[l][stop - 1] < INF / 2) {
				f2 = true; //������ ���������� ����� ���� �� start � stop
			}
		}
		l++;
	}

	if (f1) { //���� ������ ������������� ����
		cout << "���������(�) ����(�) �������������� ����, � �������(-��) ������ �������: " << endl << "\t";
		for (int i = 0; i < vn; i++) {
			if (a[i][i] < 0) cout << i + 1 << " ";
		}
		cout << endl;
	}

	cout << "����������� ����: " << endl << "\t";
	if (f2) { //���� ������ ���������� ����� ���� �� start � stop
		cout << ">---���� ����� ����� ���������� ����� �����---<" << endl;
	}
	else {
		//���������� �����
		if (a[start - 1][stop - 1] < INF / 2) { //���� ���� ����������
			find_path_unoptimized(way_min, path_min, start, stop); //����� ����������� ����
			print_path(path_min); //������� ����������� ����
			cout << endl << "��������� ��� ������������ ����: " << endl << "\t";
			cout.unsetf(ios_base::floatfield);
			cout << a[start - 1][stop - 1] << endl;

			if (flag) { //���� ������� ����� ��������� ����
				cout << "������� ����: " << endl << "\t";
				find_path_unoptimized(way_safe, path_safe, start, stop); //����� �������� ����
				print_path(path_safe); //������� �������� ����
				cout << endl << "�������� ��������� ����: " << endl << "\t";
				cout << pow(10, -c[start - 1][stop - 1]);
			}
		}
		else {
			cout << ">---���� ���---<" << endl;
		}
	}
}


template<class T>
void GraphVE<T>::floid_optimized(int start, int stop, bool flag)
{
	cout << endl << endl << endl << "���������� ����������������� ������..." << endl << endl;
	//���������������
	Timer t;

	//���������� ������� ����������� ����� � ����������, ������� -1
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

	//���������� ������� ����������� ����� � ����������, ������� -1
	int** way_safe = new int* [vn];
	endi = way_safe + vn;

	for (int** i = way_safe; i < endi; i++) {
		*i = new int[vn];
		endj = *i + vn;
		for (int* j = *i; j < endj; j++) {
			*j = -1;
		}
	}

	//���������� ������� ���������� ����������
	double* a = new double[vn * vn];

	//���������� ������� ���������� ��������
	double* c = new double[vn * vn];

	//����������� ������� ��� ��������� ����������� �����
	double* p = arr;
	double* endk = a + vn * vn;
	for (double* k = a; k < endk; k++) {
		*k = *p++;
	}

	//����������� ������� ��� ��������� ������� �����
	p = arr;
	endk = c + vn * vn;
	for (double* k = c; k < endk; k++) {
		*k = (*p == INF || *p == 0) ? INF : -log10(*p);
		p++;
	}

	//���������� ��������� ������
	floyd_cycle_optimized(a, way_min); //��� ������������ ����
	if (flag) floyd_cycle_optimized(c, way_safe); //��� ������������ ����

	time_optimized = t.elapsed(); //�������� ����� ����������
	cout << scientific << "����� ����������������� ������: " << endl << "\t" << time_optimized << endl;

	bool f1 = false; //��� ������������� ������
	bool f2 = false; //���� �� start � stop �� ����� ���� ���������� �����

	int l = 0;
	while (!f2 && l < vn) {
		if (a[l * vn + l] < 0) {
			f1 = true; //������ ������������� ����
			if (a[(start - 1) * vn + l] < INF / 2 && a[l * vn + stop - 1] < INF / 2) {
				f2 = true; //������ ���������� ����� ���� �� start � stop
			}
		}
		l++;
	}

	if (f1) { //���� ������ ������������� ����
		cout << "���������(�) ����(�) �������������� ����, � �������(-��) ������ �������: " << endl << "\t";
		for (int i = 0; i < vn; i++) {
			if (a[i * vn + i] < 0) cout << i + 1 << " ";
		}
		cout << endl;
	}

	cout << "����������� ����: " << endl << "\t";
	if (f2) { //���� ������ ���������� ����� ���� �� start � stop
		cout << ">---���� ����� ����� ���������� ����� �����---<" << endl;
	}
	else {
		//���������� ����
		if (a[(start - 1) * vn + stop - 1] < INF / 2) { //���� ���� ����������
			find_path_optimized(way_min, path_min, start, stop);
			print_path(path_min); //������� ����������� ����
			cout << endl << "��������� ��� ������������ ����: " << endl << "\t";
			cout.unsetf(ios_base::floatfield);
			cout << a[(start - 1) * vn + stop - 1] << endl;

			if (flag) { //���� ������� ����� ��������� ����
				cout << "������� ����: " << endl << "\t";
				find_path_optimized(way_safe, path_safe, start, stop); //����� �������� ����
				print_path(path_safe); //������� �������� ����
				cout << endl << "�������� ��������� ����: " << endl << "\t";
				cout << pow(10, -c[(start - 1) * vn + stop - 1]);
			}
		}
		else {
			cout << ">---���� ���---<" << endl;
		}
	}

}

int main()
{
	setlocale(LC_ALL, "Russian");

	GraphVE<int> g("input.txt"); //������������� ����� ����������� �� �����
	cout << SEP << endl << g; //����� ���������� � �����

	int start, stop;
	do {
		cout << "������� ��������� � �������� �������: (�� " << 1 << " �� " << g.get_vn() << ") " << endl;
		cin >> start >> stop;
	} while (start<1 || stop<1 || start>g.get_vn() || stop > g.get_vn());

	//ofstream ofile("output.txt"); //����������� ��������� ���� � ����� ����
	//if (!ofile) {
	//	cout << "������ ������ �����";
	//	exit(2);
	//}
	//ofile << g;
	//ofile.close();

	//g.print_matr(); //����������� ������� ������� ���������

	g.floid_unoptimized(start, stop); //����� ������������������� ������
	g.floid_optimized(start, stop); //����� ����������������� ������
	cout << fixed << endl << endl << "���������������� ����� ���������� � " << g.time_unoptimized / g.time_optimized << " ��� ������� �������������������" << endl << SEP << endl;

	cout << "������� enter...";
	cin.get(); cin.get();

	//g.path_min; //����������� ���������� � ������������ ����


	GraphVE<int> h("input_300_10.txt"); //������������� ����� ����������� �� �����
	cout << h; //����� ���������� � �����

	h.floid_unoptimized(1, 6); //����� ������������������� ������
	h.floid_optimized(1, 6); //����� ����������������� ������
	cout << fixed << endl << endl << "���������������� ����� ���������� � " << h.time_unoptimized / h.time_optimized << " ��� ������� �������������������" << endl << SEP << endl;



	GraphVE<int> z("otriz.txt"); //������������� ����� ����������� �� �����
	cout << z; //����� ���������� � �����

	z.floid_unoptimized(1, 2); //����� ������������������� ������
	z.floid_optimized(1, 2); //����� ����������������� ������
	cout << fixed << endl << endl << "���������������� ����� ���������� � " << z.time_unoptimized / z.time_optimized << " ��� ������� �������������������" << endl << SEP << endl;



	GraphVE<int> p("probabilities.txt"); //������������� ����� ����������� �� �����
	cout << p; //����� ���������� � �����

	p.floid_unoptimized(1, 6, true); //����� ������������������� ������
	p.floid_optimized(1, 6, true); //����� ����������������� ������
	cout << fixed << endl << endl << "���������������� ����� ���������� � " << p.time_unoptimized / p.time_optimized << " ��� ������� �������������������" << endl << SEP << endl;


	return 0;
}

