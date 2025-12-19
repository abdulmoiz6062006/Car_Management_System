#ifndef SYSTEM_H
#define SYSTEM_H

#include "Car.h"
#include <fstream>
#include <vector>
#include <stdexcept> // For Exceptions

class System {
private:
    static const int MAX_CARS = 10;
    Car* cars[MAX_CARS];
    int carCount;

public:
    System() {
        carCount = 0;
        for(int i = 0; i < MAX_CARS; i++) cars[i] = nullptr;
        loadFromFile();
    }

    ~System() {
        for(int i = 0; i < carCount; i++) if(cars[i]) delete cars[i];
    }

    // --- LOGIC: Add Car ---
    bool addCar(string make, string model, double price, string engineType) {
        if (carCount >= MAX_CARS) return false;
        cars[carCount] = new Car(make, model, price, engineType, false);
        carCount++;
        saveToFile();
        return true;
    }

    // --- LOGIC: Purchase Car (With Exception Handling) ---
    void purchaseCar(int index) {
        // Exception 1: Range Error
        if (index < 0 || index >= carCount) {
            throw out_of_range("Invalid selection!");
        }
        // Exception 2: Logic Error
        if (cars[index]->getStatus() == true) {
            throw logic_error("This car has already been sold!");
        }

        // Process Purchase
        cars[index]->markSold();
        saveToFile();
    }

    // --- LOGIC: Delete Car ---
    void deleteCar(int index) {
        if (index < 0 || index >= carCount) return;

        delete cars[index];
        for (int i = index; i < carCount - 1; i++) {
            cars[i] = cars[i + 1];
        }
        cars[carCount - 1] = nullptr;
        carCount--;
        saveToFile();
    }

    // --- LOGIC: Financial Report ---
    struct SalesReport {
        int count;
        double revenue;
    };

    SalesReport getSalesReport() {
        SalesReport report = {0, 0.0};
        for(int i=0; i<carCount; i++) {
            if(cars[i]->getStatus()) {
                report.count++;
                report.revenue += cars[i]->getPrice();
            }
        }
        return report;
    }

    // --- UTILS ---
    int getCount() const { return carCount; }
    Car* getCar(int index) const {
        if(index >= 0 && index < carCount) return cars[index];
        return nullptr;
    }

    // --- FILE I/O (Updated for Sold Status) ---
    void loadFromFile() {
        ifstream inFile("cars.txt");
        if (!inFile) return;

        string make, model, engineType;
        double price;
        bool sold;

        for(int i=0; i<carCount; i++) delete cars[i];
        carCount = 0;

        while (carCount < MAX_CARS && getline(inFile, make)) {
            if(make.empty()) continue;
            getline(inFile, model);
            inFile >> price;
            inFile >> sold; // Read Status
            inFile.ignore();
            getline(inFile, engineType);

            cars[carCount] = new Car(make, model, price, engineType, sold);
            carCount++;
        }
        inFile.close();
    }

    void saveToFile() {
        ofstream outFile("cars.txt");
        for (int i=0; i<carCount; i++) {
            outFile << cars[i]->getMake() << endl;
            outFile << cars[i]->getModel() << endl;
            outFile << cars[i]->getPrice() << endl;
            outFile << cars[i]->getStatus() << endl; // Save Status
            outFile << cars[i]->getEngineType() << endl;
        }
        outFile.close();
    }
};

#endif
