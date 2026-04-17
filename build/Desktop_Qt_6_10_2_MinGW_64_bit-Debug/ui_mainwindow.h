/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter;
    QScrollArea *leftScrollArea;
    QWidget *leftPanel;
    QVBoxLayout *leftLayout;
    QLabel *titleLabel;
    QGroupBox *inputGroup;
    QVBoxLayout *inputLayout;
    QHBoxLayout *fileLayout;
    QPushButton *loadFileBtn;
    QSpacerItem *horizontalSpacer;
    QGridLayout *manualLayout;
    QLabel *fromLabel;
    QLineEdit *fromEdit;
    QLabel *toLabel;
    QLineEdit *toEdit;
    QLabel *weightLabel;
    QLineEdit *weightEdit;
    QPushButton *addEdgeBtn;
    QPushButton *clearBtn;
    QLabel *nodeCountLabel;
    QGroupBox *edgesGroup;
    QVBoxLayout *edgesLayout;
    QTableWidget *edgesTable;
    QGroupBox *searchGroup;
    QHBoxLayout *searchLayout;
    QLabel *startLabel;
    QLineEdit *startNodeEdit;
    QLabel *endLabel;
    QLineEdit *endNodeEdit;
    QPushButton *findPathBtn;
    QGroupBox *resultGroup;
    QVBoxLayout *resultLayout;
    QLabel *resultLabel;
    QGroupBox *viewGroup;
    QHBoxLayout *viewLayout;
    QPushButton *resetViewBtn;
    QPushButton *saveSolutionBtn;
    QSpacerItem *verticalSpacer;
    QWidget *rightPanel;
    QVBoxLayout *rightLayout;
    QWidget *graphContainer;
    QVBoxLayout *graphContainerLayout;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1400, 900);
        MainWindow->setStyleSheet(QString::fromUtf8("\n"
"    QMainWindow { background-color: #FFF0F5; }\n"
"    QWidget { background-color: #FFF0F5; }\n"
"    QScrollArea { background-color: #FFF0F5; border: none; }\n"
"    QScrollBar:vertical { border: none; background-color: #FFF0F5; width: 10px; border-radius: 5px; }\n"
"    QScrollBar::handle:vertical { background-color: #FF69B4; border-radius: 5px; min-height: 20px; }\n"
"    QScrollBar::handle:vertical:hover { background-color: #FF1493; }\n"
"    QGroupBox { font-weight: bold; border: 2px solid #FF69B4; border-radius: 8px; margin-top: 10px; padding-top: 10px; background-color: #FFF5F8; color: #8B0045; }\n"
"    QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; color: #FF1493; }\n"
"    QPushButton { background-color: #FF69B4; border: none; border-radius: 6px; padding: 8px; font-weight: bold; color: white; }\n"
"    QPushButton:hover { background-color: #FF1493; }\n"
"    QPushButton:pressed { background-color: #C71585; }\n"
"    QLineEdit { border: 2px solid #FFB6C1; border-radius:"
                        " 5px; padding: 5px; background-color: white; color: #8B0045; }\n"
"    QLineEdit:focus { border-color: #FF69B4; }\n"
"    QTableWidget { border: 2px solid #FFB6C1; border-radius: 5px; background-color: white; alternate-background-color: #FFF0F5; gridline-color: #FFB6C1; }\n"
"    QTableWidget::item { padding: 5px; }\n"
"    QHeaderView::section { background-color: #FF69B4; color: white; padding: 5px; border: none; font-weight: bold; }\n"
"    QLabel { color: #8B0045; }\n"
"    QLabel#resultLabel { border: 2px solid #FF69B4; border-radius: 8px; padding: 10px; background-color: #FFF0F5; color: #8B0045; }\n"
"    QSplitter::handle { background-color: #FFB6C1; }\n"
"   "));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName("horizontalLayout");
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Horizontal);
        leftScrollArea = new QScrollArea(splitter);
        leftScrollArea->setObjectName("leftScrollArea");
        leftScrollArea->setWidgetResizable(true);
        leftScrollArea->setMaximumSize(QSize(450, 16777215));
        leftPanel = new QWidget();
        leftPanel->setObjectName("leftPanel");
        leftLayout = new QVBoxLayout(leftPanel);
        leftLayout->setObjectName("leftLayout");
        titleLabel = new QLabel(leftPanel);
        titleLabel->setObjectName("titleLabel");
        titleLabel->setAlignment(Qt::AlignCenter);

        leftLayout->addWidget(titleLabel);

        inputGroup = new QGroupBox(leftPanel);
        inputGroup->setObjectName("inputGroup");
        inputLayout = new QVBoxLayout(inputGroup);
        inputLayout->setObjectName("inputLayout");
        fileLayout = new QHBoxLayout();
        fileLayout->setObjectName("fileLayout");
        loadFileBtn = new QPushButton(inputGroup);
        loadFileBtn->setObjectName("loadFileBtn");

        fileLayout->addWidget(loadFileBtn);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        fileLayout->addItem(horizontalSpacer);


        inputLayout->addLayout(fileLayout);

        manualLayout = new QGridLayout();
        manualLayout->setObjectName("manualLayout");
        fromLabel = new QLabel(inputGroup);
        fromLabel->setObjectName("fromLabel");

        manualLayout->addWidget(fromLabel, 0, 0, 1, 1);

        fromEdit = new QLineEdit(inputGroup);
        fromEdit->setObjectName("fromEdit");

        manualLayout->addWidget(fromEdit, 0, 1, 1, 1);

        toLabel = new QLabel(inputGroup);
        toLabel->setObjectName("toLabel");

        manualLayout->addWidget(toLabel, 0, 2, 1, 1);

        toEdit = new QLineEdit(inputGroup);
        toEdit->setObjectName("toEdit");

        manualLayout->addWidget(toEdit, 0, 3, 1, 1);

        weightLabel = new QLabel(inputGroup);
        weightLabel->setObjectName("weightLabel");

        manualLayout->addWidget(weightLabel, 0, 4, 1, 1);

        weightEdit = new QLineEdit(inputGroup);
        weightEdit->setObjectName("weightEdit");

        manualLayout->addWidget(weightEdit, 0, 5, 1, 1);

        addEdgeBtn = new QPushButton(inputGroup);
        addEdgeBtn->setObjectName("addEdgeBtn");

        manualLayout->addWidget(addEdgeBtn, 1, 0, 1, 6);


        inputLayout->addLayout(manualLayout);

        clearBtn = new QPushButton(inputGroup);
        clearBtn->setObjectName("clearBtn");

        inputLayout->addWidget(clearBtn);

        nodeCountLabel = new QLabel(inputGroup);
        nodeCountLabel->setObjectName("nodeCountLabel");

        inputLayout->addWidget(nodeCountLabel);


        leftLayout->addWidget(inputGroup);

        edgesGroup = new QGroupBox(leftPanel);
        edgesGroup->setObjectName("edgesGroup");
        edgesLayout = new QVBoxLayout(edgesGroup);
        edgesLayout->setObjectName("edgesLayout");
        edgesTable = new QTableWidget(edgesGroup);
        if (edgesTable->columnCount() < 3)
            edgesTable->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        edgesTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        edgesTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        edgesTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        edgesTable->setObjectName("edgesTable");
        edgesTable->setMinimumSize(QSize(0, 150));
        edgesTable->setMaximumSize(QSize(16777215, 250));
        edgesTable->horizontalHeader()->setStretchLastSection(true);

        edgesLayout->addWidget(edgesTable);


        leftLayout->addWidget(edgesGroup);

        searchGroup = new QGroupBox(leftPanel);
        searchGroup->setObjectName("searchGroup");
        searchLayout = new QHBoxLayout(searchGroup);
        searchLayout->setObjectName("searchLayout");
        startLabel = new QLabel(searchGroup);
        startLabel->setObjectName("startLabel");

        searchLayout->addWidget(startLabel);

        startNodeEdit = new QLineEdit(searchGroup);
        startNodeEdit->setObjectName("startNodeEdit");
        startNodeEdit->setMaximumSize(QSize(60, 16777215));

        searchLayout->addWidget(startNodeEdit);

        endLabel = new QLabel(searchGroup);
        endLabel->setObjectName("endLabel");

        searchLayout->addWidget(endLabel);

        endNodeEdit = new QLineEdit(searchGroup);
        endNodeEdit->setObjectName("endNodeEdit");
        endNodeEdit->setMaximumSize(QSize(60, 16777215));

        searchLayout->addWidget(endNodeEdit);

        findPathBtn = new QPushButton(searchGroup);
        findPathBtn->setObjectName("findPathBtn");

        searchLayout->addWidget(findPathBtn);


        leftLayout->addWidget(searchGroup);

        resultGroup = new QGroupBox(leftPanel);
        resultGroup->setObjectName("resultGroup");
        resultLayout = new QVBoxLayout(resultGroup);
        resultLayout->setObjectName("resultLayout");
        resultLabel = new QLabel(resultGroup);
        resultLabel->setObjectName("resultLabel");
        resultLabel->setAlignment(Qt::AlignTop);
        resultLabel->setWordWrap(true);
        resultLabel->setMinimumSize(QSize(0, 120));

        resultLayout->addWidget(resultLabel);


        leftLayout->addWidget(resultGroup);

        viewGroup = new QGroupBox(leftPanel);
        viewGroup->setObjectName("viewGroup");
        viewLayout = new QHBoxLayout(viewGroup);
        viewLayout->setObjectName("viewLayout");
        resetViewBtn = new QPushButton(viewGroup);
        resetViewBtn->setObjectName("resetViewBtn");

        viewLayout->addWidget(resetViewBtn);


        leftLayout->addWidget(viewGroup);

        saveSolutionBtn = new QPushButton(leftPanel);
        saveSolutionBtn->setObjectName("saveSolutionBtn");

        leftLayout->addWidget(saveSolutionBtn);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        leftLayout->addItem(verticalSpacer);

        leftScrollArea->setWidget(leftPanel);
        splitter->addWidget(leftScrollArea);
        rightPanel = new QWidget(splitter);
        rightPanel->setObjectName("rightPanel");
        rightLayout = new QVBoxLayout(rightPanel);
        rightLayout->setObjectName("rightLayout");
        rightLayout->setContentsMargins(0, 0, 0, 0);
        graphContainer = new QWidget(rightPanel);
        graphContainer->setObjectName("graphContainer");
        graphContainerLayout = new QVBoxLayout(graphContainer);
        graphContainerLayout->setObjectName("graphContainerLayout");
        graphContainerLayout->setContentsMargins(0, 0, 0, 0);

        rightLayout->addWidget(graphContainer);

        splitter->addWidget(rightPanel);

        horizontalLayout->addWidget(splitter);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\320\224\320\270\320\275\320\260\320\274\320\270\321\207\320\265\321\201\320\272\320\276\320\265 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\270\321\200\320\276\320\262\320\260\320\275\320\270\320\265 - \320\222\320\270\320\267\321\203\320\260\320\273\320\270\320\267\320\260\321\206\320\270\321\217 \320\272\321\200\320\260\321\202\321\207\320\260\320\271\321\210\320\265\320\263\320\276 \320\277\321\203\321\202\320\270", nullptr));
        titleLabel->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\270\321\201\320\272 \320\272\321\200\320\260\321\202\321\207\320\260\320\271\321\210\320\265\320\263\320\276 \320\277\321\203\321\202\320\270\n"
"\320\234\320\265\321\202\320\276\320\264 \320\264\320\270\320\275\320\260\320\274\320\270\321\207\320\265\321\201\320\272\320\276\320\263\320\276 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\270\321\200\320\276\320\262\320\260\320\275\320\270\321\217", nullptr));
        titleLabel->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 16px; font-weight: bold; margin: 10px; color: #FF1493;", nullptr));
        inputGroup->setTitle(QCoreApplication::translate("MainWindow", "\320\222\320\262\320\276\320\264 \320\264\320\260\320\275\320\275\321\213\321\205", nullptr));
        loadFileBtn->setText(QCoreApplication::translate("MainWindow", "\320\227\320\260\320\263\321\200\321\203\320\267\320\270\321\202\321\214 \320\270\320\267 \321\204\320\260\320\271\320\273\320\260", nullptr));
        fromLabel->setText(QCoreApplication::translate("MainWindow", "\320\236\321\202:", nullptr));
        fromLabel->setStyleSheet(QCoreApplication::translate("MainWindow", "color: #8B0045; font-weight: bold;", nullptr));
        fromEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "1", nullptr));
        toLabel->setText(QCoreApplication::translate("MainWindow", "\320\224\320\276:", nullptr));
        toLabel->setStyleSheet(QCoreApplication::translate("MainWindow", "color: #8B0045; font-weight: bold;", nullptr));
        toEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "2", nullptr));
        weightLabel->setText(QCoreApplication::translate("MainWindow", "\320\222\320\265\321\201:", nullptr));
        weightLabel->setStyleSheet(QCoreApplication::translate("MainWindow", "color: #8B0045; font-weight: bold;", nullptr));
        weightEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "5", nullptr));
        addEdgeBtn->setText(QCoreApplication::translate("MainWindow", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214 \321\200\320\265\320\261\321\200\320\276", nullptr));
        clearBtn->setText(QCoreApplication::translate("MainWindow", "\320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214 \320\263\321\200\320\260\321\204", nullptr));
        nodeCountLabel->setText(QCoreApplication::translate("MainWindow", "\320\243\320\267\320\273\320\276\320\262: 0", nullptr));
        nodeCountLabel->setStyleSheet(QCoreApplication::translate("MainWindow", "color: #FF1493; font-weight: bold; font-size: 12px;", nullptr));
        edgesGroup->setTitle(QCoreApplication::translate("MainWindow", "\320\241\320\277\320\270\321\201\320\276\320\272 \321\200\320\265\320\261\320\265\321\200", nullptr));
        QTableWidgetItem *___qtablewidgetitem = edgesTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "\320\236\321\202", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = edgesTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "\320\224\320\276", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = edgesTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "\320\222\320\265\321\201", nullptr));
        searchGroup->setTitle(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\270\321\201\320\272 \320\277\321\203\321\202\320\270", nullptr));
        startLabel->setText(QCoreApplication::translate("MainWindow", "\320\241\321\202\320\260\321\200\321\202:", nullptr));
        startLabel->setStyleSheet(QCoreApplication::translate("MainWindow", "color: #8B0045; font-weight: bold;", nullptr));
        startNodeEdit->setText(QCoreApplication::translate("MainWindow", "1", nullptr));
        endLabel->setText(QCoreApplication::translate("MainWindow", "\320\244\320\270\320\275\320\270\321\210:", nullptr));
        endLabel->setStyleSheet(QCoreApplication::translate("MainWindow", "color: #8B0045; font-weight: bold;", nullptr));
        findPathBtn->setText(QCoreApplication::translate("MainWindow", "\320\235\320\260\320\271\321\202\320\270 \320\277\321\203\321\202\321\214", nullptr));
        resultGroup->setTitle(QCoreApplication::translate("MainWindow", "\320\240\320\265\320\267\321\203\320\273\321\214\321\202\320\260\321\202", nullptr));
        resultLabel->setText(QCoreApplication::translate("MainWindow", "\320\236\320\266\320\270\320\264\320\260\320\275\320\270\320\265 \320\277\320\276\320\270\321\201\320\272\320\260...", nullptr));
        viewGroup->setTitle(QCoreApplication::translate("MainWindow", "\320\243\320\277\321\200\320\260\320\262\320\273\320\265\320\275\320\270\320\265", nullptr));
        resetViewBtn->setText(QCoreApplication::translate("MainWindow", "\320\241\320\261\321\200\320\276\321\201 \320\277\320\276\320\267\320\270\321\206\320\270\320\271", nullptr));
        saveSolutionBtn->setText(QCoreApplication::translate("MainWindow", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214 \321\200\320\265\321\210\320\265\320\275\320\270\320\265", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
