#include <iostream>
#include <iomanip> //setw();
#include <stdlib.h> //rand();
#include <time.h>
#include <math.h>
#include <vector>
#include <stack>
#include <conio.h> //getch();
#include <fstream>
#include <string>

#define HEUR 1

using namespace std;

const int N=3, M=3;
const int MAX=1000;

//const int N=3, M=3;
//const int MAX=1000;

class matr{
    private:
        int *p; //указатель на массив
    public:
        matr(); //конструктор 
        matr(const matr &r); //конструктор копиравания
        matr(const int *r); //конструктор инициализации
        ~matr(); //деструктор
        
        void print(); //вывод состояния
        void fprint(ofstream &f); //вывод состояния в файл
        void info(matr &r); //информация
        int& operator ()(int i, int j); //индекс координат
        matr& operator =(const matr &r); //присваивание
        bool operator ==(const matr &r); //сравнение
        bool operator !=(const matr &r); //сравнение
        static matr random(); //случайное состояние
        static matr right(); //упорядоченное состояние
        
        float h_distance(matr &r); //скольжений нужно совершить
        float h_place(matr &r); //меток не на своих местах
        float h_part(matr &r); //строк/столбцов не собрано
        float heuristic(matr &r); //эвристики
        bool parity(); //четность
        int ind(int k); //индекс
        int di(int k); //индекс i
        int dj(int k); //индекс j
        bool search(); //поиск
        matr move(int k); //перемещение
        matr mix(); //перемешать
};

vector<matr> arr;
stack<matr> way;
stack<int> hist;

matr matr::mix(){ //перемешать
    matr tmp(*this);

    int i,j,k;
    
    for (int g=0; g<100000; g++){
        k=rand()%4;
        i=tmp.di(0);
        j=tmp.dj(0);
        
        switch(k){
            case 0: //вверх
            {
                if (i!=0) swap(tmp(i,j),tmp(i-1,j));
                break;    
            }
            case 1: //вниз
            {
                if (i!=N-1) swap(tmp(i,j),tmp(i+1,j));
                break;     
            }
            case 2: //влево
            {
                if (j!=0) swap(tmp(i,j),tmp(i,j-1));
                break;    
            }
            case 3: //вправо
            {
                if (j!=M-1) swap(tmp(i,j),tmp(i,j+1));
                break;     
            }  
            default:
            {
                cout<<"Error: Ошибка перемешивания"; 
                exit(1);
            }
        }
    }
    return tmp;
}

void matr::info(matr &r){ //информаиця
    cout<<"= "<<parity()<<" и "<<r.parity()<<" четности"<<endl;
    cout<<"= "<<h_distance(r)<<" скольжений нужно совершить"<<endl;
    cout<<"= "<<h_place(r)<<" меток не на своих местах"<<endl;
    cout<<"= "<<h_part(r)<<" строк/столбцов не собрано"<<endl;
    cout<<"= "<<heuristic(r)<<" оценка эвристиками"<<endl;
}

bool matr::search(){ //true-нашлось; false-не нашлось, добавилось
    bool f=true;
    int i=0;
    while (f && i<arr.size()){
        if (arr[i]==*this){
            f=false;
            return true;
        }
        i++;
    }
    if (f){
        arr.push_back(*this);
        return false;
    }
}

matr::matr(){ //конструктор
    p=new int[N*M];  
}

matr::matr(const matr &r){ //конструктор копирования
    p=new int[N*M];
    for (int i=0; i<N*M; i++) p[i]=r.p[i];
}

matr::matr(const int *r){ //конструктор инициализации
    p=new int[N*M];
    for (int i=0; i<N*M; i++) p[i]=r[i];
}

matr::~matr(){ //деструктор
    delete[] p;
}

void matr::print(){ //вывод состояния
    for (int i=0; i<M; i++) cout<<"-----"; cout<<"-"<<endl;
    
    for (int i=0; i<N; i++){
        cout<<"| ";
        for (int j=0; j<M; j++)
            cout<<setw(2)<<(*this)(i,j)<<" | ";
        cout<<endl;    
        for (int h=0; h<M; h++) cout<<"-----"; cout<<"-"<<endl;
    }    
}

void matr::fprint(ofstream &f){ //вывод состояния в файл
    for (int i=0; i<M; i++) f<<"-----"; f<<"-"<<endl;
    
    for (int i=0; i<N; i++){
        f<<"| ";
        for (int j=0; j<M; j++)
            f<<setw(2)<<(*this)(i,j)<<" | ";
        f<<endl;    
        for (int h=0; h<M; h++) f<<"-----"; f<<"-"<<endl;
    } 
    
}

void fout(ofstream &f){ //вывод данных в файл
    
    f.open("way.txt");
    
    if (!f.is_open()) cout<<"Error: Ошибка открытия файла";
    else{
        int k,i=0;
        while (!hist.empty()){
            i++;
            f<<i<<") ";
            switch (hist.top()){
                case 0: f<<"вверх"<<endl; break;
                case 1: f<<"вниз"<<endl; break;
                case 2: f<<"влево"<<endl; break;
                case 3: f<<"вправо"<<endl; break;
            }
        hist.pop();
        }
        
        i=0;
        while (!way.empty()){
            i++;
            f<<"Шаг "<<i<<":"<<endl;
            way.top().fprint(f);
            f<<endl;
            way.pop();
        }
        
        f.close();
        cout<<"Данные успешно записаны в файл: "<<"way.txt";
        
    }
    
}

int& matr::operator()(int i, int j){ //индекс координат
    if (i<0 || i>N-1 || j<0 || j>M-1){
        cout<<"Error: Некорректный индекс"; 
        exit(1);
    }  
    return p[i*M+j];
}

matr& matr::operator =(const matr &r){ //присваивание 
    for (int i=0; i<N*M; i++) p[i]=r.p[i]; 
    return *this;
}

bool matr::operator ==(const matr &r){ //сравнение
    for (int i=0; i<N*M; i++) if (p[i]!=r.p[i]) return false;
    return true;
}

bool matr::operator !=(const matr &r){ //сравнение
    return !(*this==r);
}

matr matr::random(){ //случайное состояние
    int arr[N*M];
    
    for (int i=0; i<N*M; i++) arr[i]=i;
    for (int i=0; i<N*M; i++) swap(arr[i],arr[rand()%(N*M)]);

    return matr(arr);
}

matr matr::right(){ //упорядоченное состояние
    int arr[N*M];
    
    arr[N*M-1]=0;
    for (int i=0; i<N*M-1; i++) arr[i]=i+1;

    return matr(arr);    
}

int matr::ind(int k){ //индекс    
    for (int i=0; i<N*M; i++) if (p[i]==k) return i;
    cout<<"Error: Ошибка поиска"; 
    exit(1);
}

int matr::di(int k){ //индекс i
    return ind(k)/M;
}

int matr::dj(int k){ //индекс j
    return ind(k)%M;
}

matr matr::move(int k){ //перемещение
    matr tmp(*this);
    int i=tmp.di(0);
    int j=tmp.dj(0);
    
    switch(k){
        case 0: //вверх
        {
            if (i!=0) swap(tmp(i,j),tmp(i-1,j));
            break;    
        }
        case 1: //вниз
        {
            if (i!=N-1) swap(tmp(i,j),tmp(i+1,j));
            break;     
        }
        case 2: //влево
        {
            if (j!=0) swap(tmp(i,j),tmp(i,j-1));
            break;    
        }
        case 3: //вправо
        {
            if (j!=M-1) swap(tmp(i,j),tmp(i,j+1));
            break;     
        }  
        default:
        {
            cout<<"Error: Ошибка перемещения"; 
            exit(1);
        }
    }
    return tmp;
}

bool matr::parity(){ //четность
    int k=0;
    for (int i=0; i<N*M-1; i++)
        for (int j=i+1; j<N*M; j++)
            if (p[i]>p[j] && p[j]!=0) k++;
            
    k+=ind(0)/M+1;
    return k%2; //0-чет, 1-нечет
}

float matr::h_distance(matr &r){ //скольжений нужно совершить
    float sum=0;
    for (int i=0; i<N*M; i++) 
        sum+=abs(di(i)-r.di(i))+abs(dj(i)-r.dj(i));
    return sum;
}

float matr::h_place(matr &r){ //меток не на своих местах
    float k=0;
    for (int i=0; i<N*M; i++) 
        if (p[i]!=r.p[i]) k++;
    return k;
}

float matr::h_part(matr &r){ //строк/столбцов не собрано
    float k=0;
    bool f;
    
    for (int i=0; i<N; i++){
        f=true;
        for (int j=0; j<M; j++){
            if ((*this)(i,j)!=r(i,j)) f=false;
        }
        if (f) k++;
    }
    
    for (int j=0; j<M; j++){
        f=true;
        for (int i=0; i<N; i++){
            if ((*this)(i,j)!=r(i,j)) f=false;
        }
        if (f) k++;
    }
    
    return M+N-k;
}

float matr::heuristic(matr &r){ //эвристики
    float mid=N+M;
    mid/=2;
    float distance=h_distance(r)/(N*M*mid);
    float place=h_place(r)/(N*M);    
    float part=h_part(r)/(N+M); 
    
    //return (part);
    return (distance+place+part)/3;
}

int tmp[9]={0};
matr tm(tmp);
int depth=0;

void sort(float *h, matr *v, int *o){
    bool f=true;
    while (f){
        f=false;
        for (int i=0; i<3; i++)
            if (h[i]>h[i+1]){ //> - вкл
                f=true;
                swap(h[i], h[i+1]);
                swap(v[i], v[i+1]);
                swap(o[i], o[i+1]);
            }        
    }
}

matr &step(matr &l, matr &r){
    
    depth++;

    if (l==r) return r;
        
    if (l.search()) return l;

    if (arr.size()%10000==0) cout<<arr.size()<<" "<<l.heuristic(r)<<" "<<depth<<endl;

    if (depth>MAX) return l;
    
    matr v[4];
    float h[4];
    int o[4]={0,1,2,3}; //направления движения
        
    for (int i=0; i<4; i++){
        v[i]=l.move(i);
        h[i]=v[i].heuristic(r);
    }
    
    if (HEUR) sort(h,v,o);
    
    for (int i=0; i<4; i++){
        if (step(v[i],r)==r){
            way.push(v[i]);
            hist.push(o[i]);
            return r;
        }
        depth--;
    } 
    
}

int main(){
    setlocale(LC_ALL,"");    
    srand(time(NULL));
    
    matr b=matr::right();
    matr a=b.mix();
    
    cout<<"Состояние А: "<<endl;
    a.print();
    cout<<"Состояние В: "<<endl;    
    b.print();
    cout<<"Информация: "<<endl;
    b.info(a);
    
    matr c=step(a,b);
    cout<<"Получено:"<<endl;
    c.print();
    cout<<"Различных состояний просмотрено: "<<arr.size()<<endl;
    cout<<"Количество перестановок: "<<way.size()<<endl;
    
    ofstream file;
    fout(file);
    
    return 0;
}






//    matr c;  
//    int a1[9]={1,2,3,8,0,4,7,6,5};
//    int b1[9]={5,3,4,6,0,7,8,2,1};
//    matr a(a1), b(b1);





//    matr a,b,c;
//    
//    do{
//        a=matr::random();
//        b=matr::right();
//    } while(a.parity()!=b.parity());
//
//    a.print();
//    b.print();
//    b.info(a);
//    c=step(a,b);
//    c.print();
//    cout<<arr.size()<<endl;
