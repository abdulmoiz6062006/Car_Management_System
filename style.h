#ifndef STYLES_H
#define STYLES_H

#include <QString>

const QString appStyles = R"(
    QWidget {
        background-color: #2b2b2b;
        color: white;
        font-family: Arial;
        font-size: 14px;
    }
    QLineEdit {
        background-color: #404040;
        border: 1px solid #555;
        border-radius: 4px;
        padding: 5px;
        color: #ddd;
    }
    QListWidget {
        background-color: #333;
        border: 1px solid #555;
        border-radius: 4px;
    }
    QGroupBox {
        border: 1px solid #666;
        border-radius: 6px;
        margin-top: 20px;
        font-weight: bold;
    }
    QGroupBox::title {
        subcontrol-origin: margin;
        subcontrol-position: top center;
        padding: 0 5px;
        color: #aaa;
    }
    QLabel#loginTitle {
        font-size: 18px;
        font-weight: bold;
        color: #3498db;
    }
    QPushButton {
        background-color: #3498db;
        border-radius: 5px;
        padding: 8px;
        font-weight: bold;
        color: white;
    }
    QPushButton:hover { background-color: #2980b9; }

    QPushButton#btnGreen { background-color: #27ae60; }
    QPushButton#btnGreen:hover { background-color: #2ecc71; }

    QPushButton#btnRed { background-color: #c0392b; }
    QPushButton#btnRed:hover { background-color: #e74c3c; }

    QPushButton#btnPurple { background-color: #8e44ad; }
    QPushButton#btnPurple:hover { background-color: #9b59b6; }

    /* NEW: Orange button for Financial Reports */
    QPushButton#btnOrange { background-color: #e67e22; }
    QPushButton#btnOrange:hover { background-color: #d35400; }
)";

#endif
