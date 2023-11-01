/**
 * 2 章 Observer パターン
 * 
 * 直訳で観察者を意味する。
 * 
 * パブリッシャ ＋ サブスクライバ ＝ Observer パターン
 * 
 * Observer パターンでは、あるオブジェクトの状態が変化すると、そのオブジェクトに依存している
 * すべてのオブジェクトに自動的に通知され更新されるようにするという、オブジェクト間の１ 対 多
 * の依存関係が定義されている。
 * 
 * Subject インタフェース
 * このSubject インタフェースを利用して、オブジェクトはオブザーバとしての登録やオブザーバからの削除を行う。
 * - registerObserver()
 * - removeObserver()
 * - notifyObserers()
 * 
 * ConcreteSubject
 * 具象サブジェクトは必ずSubject インタフェースを実装する。具象サブジェクトは登録メソッドと削除メソッドの他に、
 * 状態が変わるたびに現在のすべてのオブザーバを更新する notifiObservers() メソッドを実装する。
 * - registerObserver() {...}
 * - removeObserver() {...}
 * - notifyObserers() {...}
 * - getState()
 * - setState()
 * 
 * Observer インタフェース
 * オブザーバになる可能性のあるオブジェクトはすべて、Observer インタフェースを実装する必要がある。
 * Observer インタフェースには、Subject の状態が変わると呼び出される update() というメソッドが１つだけある。
 * - update() 
 * 
 * ConcreteObserver
 * 具象オブザーバは、Observer インタフェースを実装した任意のクラスです。オブザーバは更新情報を受け取るために
 * 具象サブジェクトに登録します。
 * 
 * ソース上では、気象観測所を実装していく。
 * 
 * ```
 * e.g. コンパイル
 * g++ -O3 -std=c++20 -pedantic-errors -Wall -Werror chapter_2_observer.cpp -o ../bin/main
 * ```
*/
#include <iostream>
#include <vector>

using namespace std;

template <class M, class D>
void (*ptr_labmda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

class Observer {
protected:
public:
    Observer() {}
    Observer(const Observer& own) {
        *this = own;
    }
    virtual ~Observer() {}
    // TODO 次の仮引数はひとつの構造体にまとめた方がいいと思う。
    virtual void update(const double temp, const double humidity, const double pressure) const = 0;     // 気温、湿度、気圧 ... 気象測定値に変化があるとObserver が Subject から取得される測定値です。
};

class Subject {
public:
    Subject() {}
    Subject(const Subject& own) {
        *this = own;
    }
    virtual ~Subject() {}
    virtual void registerObserver(Observer* o) const = 0;
    virtual void removeObserver(Observer* o) const = 0;
    virtual void notifyObservers() const = 0;   // Subject の状態が変化すると、すべてのオブザーバに通知するためのメソッドが呼び出される。
};

class Display {     // Display インタフェースには display() メソッドが１つあり、表示要素を表示する必要のあるときに呼び出される。
public:
    virtual ~Display() {}
    virtual void display() const = 0;
};

/**
 * WeatherData で Subject インタフェースを実装する。
*/
class WeatherData final : public virtual Subject {
private:
    vector<Observer*>* observers = nullptr;     // vector とインタフェース ... この扱いが難しい、現状の解釈ではコンパイルと通すため、インタフェースはポインタで扱い、 vector オブジェクトは new する必要がある。
    double temperature, humidity, pressure;

public:
    WeatherData() {
        observers = new vector<Observer*>();
    }
    WeatherData(const WeatherData& own) {
        *this = own;
    }
    ~WeatherData() {
        delete observers;
        ptr_labmda_debug<const char*,const int&>("DONE. WeatherData Destructor.",0);
    }
    virtual void registerObserver(Observer* o) const override {
        observers->push_back(o);
    }
    virtual void removeObserver(Observer* o) const override {
        // TODO vector erase の調査
        // for(std::vector<Observer*>::iterator ite = observers->begin(); ite != observers->end();) {
        // }
        size_t size = observers->size();
        for(size_t i = 0; i < size; i++) {
           Observer* po = observers->at(i);
           if(po == o) {
                observers->erase(observers->begin() + i);
           }
        }
    }
    virtual void notifyObservers() const override {
        size_t size = observers->size();
        for(size_t i=0; i < size; i++) {
            observers->at(i)->update(temperature, humidity, pressure);
        }
    }
    void changedMessurements() {
        notifyObservers();
    }
    void setMessurements(const double tmp, const double humid, const double press) {
        temperature = tmp;
        humidity = humid;
        pressure = press;
        changedMessurements();
    }

};

class CurrentConditionsDisplay final : public virtual Observer, Display {
private:
    double temperature, humidity, pressure;
    WeatherData weatherData;
    CurrentConditionsDisplay() {}

public:
    CurrentConditionsDisplay(const WeatherData& wd) {
        weatherData = wd;
        weatherData.registerObserver(this);
    }
    CurrentConditionsDisplay(const CurrentConditionsDisplay& own) {
        *this = own;
    }
    ~CurrentConditionsDisplay() {}
    virtual void update(const double temp, const double humidity, const double pressure) const override {
        // TODO 実装
    }
    virtual void display() const override {
        // TODO 実装
    }
};

int test_WeatherData() {
    puts("--- test_WeatherData");
    try {
        WeatherData wd;

    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
    return 0;
}

int main(void) {
    puts(" START ========= 2 章 Observer パターン");
    if(1) {
        ptr_labmda_debug<const char*,const int&>("debugger test.",0);
    }
    if(1.01) {
        ptr_labmda_debug<const char*,const int&>("Play and Result ... ",test_WeatherData());
    }
    puts("2 章 Observer パターン ========= END");
    return 0;
}