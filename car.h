#ifndef CAR_H
#define CAR_H

#include <string>
#include <iostream>
#include <cctype>
#include <algorithm>
using namespace std;

// --- COMPOSITION: The Engine Class ---
class Engine {
    string type;
public:
    Engine(string t = "Standard") : type(t) {}
    string getType() const { return type; }
};

class Car {
protected:
    string make;
    string model;
    double price;
    bool isSold; // NEW: Track status from Console Logic

    // Car HAS-A Engine (Composition)
    Engine engine;

public:
    // Updated Constructor to include isSold (defaults to false)
    Car(string mk, string mdl, double p, string engineType = "Standard", bool sold = false)
        : make(mk), model(mdl), price(p), isSold(sold), engine(engineType) {}

    // Getters
    string getMake() const { return make; }
    string getModel() const { return model; }
    double getPrice() const { return price; }
    string getEngineType() const { return engine.getType(); }

    // NEW: Status Getters/Setters
    bool getStatus() const { return isSold; }
    void markSold() { isSold = true; }

    // Search Logic (Case insensitive)
    int isMatch(string query) {
        string m = make;
        string mo = model;
        string q = query;
        auto toLower = [](unsigned char c){ return std::tolower(c); };
        std::transform(m.begin(), m.end(), m.begin(), toLower);
        std::transform(mo.begin(), mo.end(), mo.begin(), toLower);
        std::transform(q.begin(), q.end(), q.begin(), toLower);

        // Logic update: If sold, it shouldn't show up in search for customers?
        // We will handle that filtering in the System class.
        if (m.find(q) != string::npos || mo.find(q) != string::npos)
            return 1;
        return 0;
    }

    virtual ~Car() {}
};

#endif
