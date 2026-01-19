//Idea 
//#Package -> pojedynczy półprodukt, który posiada iD
//IPackageStockpile -> abstrakcyjny "magazyn na paczki"
//IPackageQueue -> magazyn z możliwością zdejmowania elementów
//PackageQueue -> implementacja IPackageQueue (FIFO/LIFO)

#pragma once

#include <list>
#include <set>
#include <cstddef>

//Alias zamiast pisać int mamy ElementID
using ElementID = int;

enum class PackageQueueType {
    FIFO,
    LIFO
};


//Klasa Package -> pojedynczy półprodukt z unikalnym ID
class Package {
public:
    Package(); //automatyczne ID

    explicit Package(ElementID id); //konkretne ID

    Package(Package&& other) noexcept; //konstruktor przenoszący
    Package& operator=(Package&& other) noexcept; //operator przenoszący

    Package(const Package&) = delete; //usuń konstruktor kopiujący
    Package& operator=(const Package&) = delete; //usuń operator kopiujący

    ~Package() = default; //domyślny destruktor -> zwalnia ID 

    ElementID getID() const; //getter zwraca ID paczki
private:
    ElementID id_; //ID paczki

    static std::set<ElementID> assigned_ids_; //zbiór przypisanych ID
    static std::set<ElementID> freed_ids_; //zbiór zwolnionych ID
    static ElementID generate_id(); //generuje unikalne ID
};

//Klasa IPackageStockpile -> abstrakcyjny magazyn na paczki
class IPackageStockpile {
public:
    using const_iterator = std::list<Package>::const_iterator;

    virtual void push(Package&& package) = 0; //dodaje paczkę do magazynu
    virtual bool empty() const = 0; //sprawdza czy magazyn jest pusty
    virtual std::size_t size() const = 0; //zwraca rozmiar magazynu

    virtual const_iterator begin() const = 0; //iterator na początek
    virtual const_iterator end() const = 0; //iterator na koniec
    virtual const_iterator cbegin() const = 0; //const iterator na początek
    virtual const_iterator cend() const = 0; //const iterator na koniec

    virtual ~IPackageStockpile() = default; //wirtualny destruktor
};

//Klasa IPackageQueue -> magazyn z możliwością zdejmowania elementów
class IPackageQueue : public IPackageStockpile {
public:
    virtual Package pop() = 0; //usuwa i zwraca paczkę z magazynu
    virtual PackageQueueType getQueueType() const = 0; //zwraca typ kolejki (FIFO/LIFO)
    virtual ~IPackageQueue() = default; //wirtualny destruktor
};

//Klasa PackageQueue -> implementacja IPackageQueue (FIFO/LIFO)
class PackageQueue : public IPackageQueue {
public:
    explicit PackageQueue(PackageQueueType type); //konstruktor z typem kolejki

    void push(Package&& package) override; //dodaje paczkę do magazynu
    bool empty() const override; //sprawdza czy magazyn jest pusty
    std::size_t size() const override; //zwraca rozmiar magazynu

    const_iterator begin() const override; //iterator na początek
    const_iterator end() const override; //iterator na koniec
    const_iterator cbegin() const override; //const iterator na początek
    const_iterator cend() const override; //const iterator na koniec

    Package pop() override; //usuwa i zwraca paczkę z magazynu
    PackageQueueType getQueueType() const override; //zwraca typ kolejki (FIFO/LIFO)

    ~PackageQueue() override = default; //domyślny destruktor
private:
    PackageQueueType type_; //typ kolejki (FIFO/LIFO)
    std::list<Package> container_; //lista paczek w magazynie
};