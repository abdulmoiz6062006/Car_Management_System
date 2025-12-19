#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QMessageBox>
#include <QGroupBox>
#include <vector>
#include "carmanager.h" // Renamed from CarManager to System
#include "style.h"

// Global System Instance
System appSystem;

// --- FORWARD DECLARATION ---
class LoginWindow;
LoginWindow* globalLoginRef = nullptr; // NEW: Global pointer to access login window

// --- LOGIN WINDOW ---
class LoginWindow : public QWidget {
    QLineEdit *userIn, *passIn;
public:
    void (*onSuccess)(int);

    LoginWindow() {
        setWindowTitle("Login");
        resize(320, 200);
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setSpacing(15);

        QLabel *title = new QLabel("System Login");
        title->setObjectName("loginTitle");
        title->setAlignment(Qt::AlignCenter);
        layout->addWidget(title);

        userIn = new QLineEdit(); userIn->setPlaceholderText("Username");
        passIn = new QLineEdit(); passIn->setPlaceholderText("Password");
        passIn->setEchoMode(QLineEdit::Password);

        layout->addWidget(userIn);
        layout->addWidget(passIn);

        QPushButton *btn = new QPushButton("Login");
        layout->addWidget(btn);

        connect(btn, &QPushButton::clicked, this, &LoginWindow::processLogin);
    }

    // NEW: Function to clear fields when logging out
    void reset() {
        userIn->clear();
        passIn->clear();
    }

    void processLogin() {
        QString u = userIn->text().toLower();
        QString p = passIn->text();

        // 1=Admin, 2=Staff, 3=Customer
        if (u == "admin" && p == "123") { onSuccess(1); close(); }
        else if (u == "staff" && p == "456") { onSuccess(2); close(); }
        else if (u == "user" && p == "123") { onSuccess(3); close(); } // Differentiated User as 3
        else { QMessageBox::warning(this, "Error", "Invalid Login!"); }
    }
};

// --- MAIN DASHBOARD WINDOW ---
class MainWindow : public QWidget {
    QListWidget *listWidget;
    QLineEdit *inMake, *inModel, *inPrice, *inEngine, *inSearch;
    QGroupBox *adminBox;
    QPushButton *btnPurchase;
    QPushButton *btnReport;
    QPushButton *btnLogout; // NEW: Logout Button declaration

    int userRole; // 1=Admin, 2=Staff, 3=Customer

    // Helper to store actual indices of cars displayed (for filtering)
    std::vector<int> displayedIndices;

public:
    MainWindow(int role) : userRole(role) {
        // NEW: Ensure window is deleted from memory when closed (logout)
        setAttribute(Qt::WA_DeleteOnClose);

        setWindowTitle("Car Management System");
        resize(600, 700);
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // --- 1. TOP BAR (Search & Reports & Logout) ---
        QHBoxLayout *topLayout = new QHBoxLayout();
        inSearch = new QLineEdit();
        inSearch->setPlaceholderText("Search inventory...");

        QPushButton *btnSearch = new QPushButton("Search");
        btnSearch->setObjectName("btnPurple");

        QPushButton *btnViewAll = new QPushButton("Refresh / Show All");

        topLayout->addWidget(inSearch);
        topLayout->addWidget(btnSearch);
        topLayout->addWidget(btnViewAll);

        // Admin Only: Financial Report Button
        if (userRole == 1) {
            btnReport = new QPushButton("$$$ Sales Report");
            btnReport->setObjectName("btnOrange");
            topLayout->addWidget(btnReport);

            connect(btnReport, &QPushButton::clicked, [=](){
                System::SalesReport r = appSystem.getSalesReport();
                QString msg = "Total Cars Sold: " + QString::number(r.count) + "\n" +
                              "Total Revenue: $" + QString::number(r.revenue);
                QMessageBox::information(this, "Financial Report", msg);
            });
        }

        // NEW: Logout Button Implementation
        btnLogout = new QPushButton("Logout");
        btnLogout->setObjectName("btnRed"); // Reuse red style for visibility
        topLayout->addWidget(btnLogout);

        connect(btnLogout, &QPushButton::clicked, [=](){
            this->close(); // Close current dashboard
            if(globalLoginRef) {
                globalLoginRef->reset(); // Clear old password
                globalLoginRef->show();  // Show login window again
            }
        });

        mainLayout->addLayout(topLayout);

        // --- 2. CAR LIST ---
        listWidget = new QListWidget();
        mainLayout->addWidget(listWidget);

        // --- 3. ACTION BUTTONS (Purchase) ---
        // Customer Only: Purchase Button
        if (userRole == 3) {
            btnPurchase = new QPushButton("Purchase Selected Car");
            btnPurchase->setObjectName("btnGreen");
            mainLayout->addWidget(btnPurchase);

            connect(btnPurchase, &QPushButton::clicked, this, &MainWindow::handlePurchase);
        }

        // --- 4. ADMIN CONTROLS ---
        adminBox = new QGroupBox("Admin Controls");
        QVBoxLayout *adminLayout = new QVBoxLayout(adminBox);

        // Inputs
        adminLayout->addWidget(new QLabel("Details:"));
        inMake = new QLineEdit(); inMake->setPlaceholderText("Make");
        inModel = new QLineEdit(); inModel->setPlaceholderText("Model");
        inPrice = new QLineEdit(); inPrice->setPlaceholderText("Price");
        inEngine = new QLineEdit(); inEngine->setPlaceholderText("Engine Type");

        adminLayout->addWidget(inMake);
        adminLayout->addWidget(inModel);
        adminLayout->addWidget(inPrice);
        adminLayout->addWidget(inEngine);

        // Admin Buttons
        QHBoxLayout *adminBtns = new QHBoxLayout();
        QPushButton *btnAdd = new QPushButton("Add Car");
        btnAdd->setObjectName("btnGreen");
        QPushButton *btnDel = new QPushButton("Delete Selected");
        btnDel->setObjectName("btnRed");

        adminBtns->addWidget(btnAdd);
        adminBtns->addWidget(btnDel);
        adminLayout->addLayout(adminBtns);
        mainLayout->addWidget(adminBox);

        // Hide Admin Box if not admin
        if (userRole != 1) {
            adminBox->hide();
        }

        // --- LOGIC CONNECTIONS ---
        connect(btnViewAll, &QPushButton::clicked, [=](){ refreshList(""); });

        connect(btnSearch, &QPushButton::clicked, [=](){
            refreshList(inSearch->text().toStdString());
        });

        connect(btnAdd, &QPushButton::clicked, [=](){
            if(inMake->text().isEmpty() || inModel->text().isEmpty()) return;
            bool ok = appSystem.addCar(
                inMake->text().toStdString(),
                inModel->text().toStdString(),
                inPrice->text().toDouble(),
                inEngine->text().toStdString()
                );
            if(ok) {
                QMessageBox::information(this, "Success", "Car Added!");
                inMake->clear(); inModel->clear(); inPrice->clear(); inEngine->clear();
                refreshList("");
            } else {
                QMessageBox::warning(this, "Full", "Inventory Full (Max 10)!");
            }
        });

        connect(btnDel, &QPushButton::clicked, [=](){
            int row = listWidget->currentRow();
            if(row < 0 || row >= displayedIndices.size()) {
                QMessageBox::warning(this, "Error", "Select a car to delete.");
                return;
            }
            // Use the actual index mapping
            appSystem.deleteCar(displayedIndices[row]);
            refreshList("");
        });

        refreshList("");
    }

    // --- PURCHASE LOGIC WITH EXCEPTION HANDLING ---
    void handlePurchase() {
        int row = listWidget->currentRow();
        if(row < 0 || row >= displayedIndices.size()) {
            QMessageBox::warning(this, "Select", "Please select a car first.");
            return;
        }

        int actualIndex = displayedIndices[row];

        try {
            // This will throw if validation fails
            appSystem.purchaseCar(actualIndex);

            QMessageBox::information(this, "Success", "Congratulations! Car Purchased.");
            refreshList(""); // Refresh to update status
        }
        catch (const std::exception& e) {
            // OOP: Catching the exception from System.h
            QMessageBox::critical(this, "Purchase Failed", e.what());
        }
    }

    void refreshList(string query) {
        listWidget->clear();
        displayedIndices.clear();
        int total = appSystem.getCount();

        for(int i = 0; i < total; i++) {
            Car* c = appSystem.getCar(i);
            if (!c) continue;

            // Search Filter
            if (!query.empty()) {
                if (c->isMatch(query) == 0) continue;
            }

            // Role Logic: Customers should NOT see SOLD cars in the list?
            // Or they see them marked as SOLD. Let's show them marked as SOLD.

            string statusStr = "";
            if (c->getStatus()) statusStr = " [SOLD]";

            string info = c->getMake() + " " + c->getModel() +
                          " (" + c->getEngineType() + ") - $" +
                          to_string((int)c->getPrice()) + statusStr;

            listWidget->addItem(QString::fromStdString(info));

            // Store the actual index so we know which car refers to which row
            displayedIndices.push_back(i);
        }
    }
};

MainWindow *win = nullptr;

void startApp(int role) {
    win = new MainWindow(role);
    win->show();
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setStyleSheet(appStyles);

    LoginWindow login;

    // NEW: Assign the global pointer to this specific login instance
    globalLoginRef = &login;

    login.onSuccess = &startApp;
    login.show();

    return a.exec();
}
