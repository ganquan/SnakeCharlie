#pragma once
#include <memory>
#include <string>
#include <vector>

using namespace std;

template <typename T>
class Observer {
public:
    virtual void field_changed(T &source, const string &field_name) = 0;
    virtual ~Observer() = default;
};

template <typename T>
class Observable {
public:
    void notify(T &source, const string &field_name) {
        for (auto observer : m_observers)
            observer->field_changed(source, field_name);
    }
    void addObserver(Observer<T> &observer) { m_observers.push_back(&observer); }
    void removeObserver(Observer<T> &observer) {
        m_observers.erase(remove(m_observers.begin(), m_observers.end(), &observer), m_observers.end());
    }

private:
    vector<Observer<T> *> m_observers;
};
