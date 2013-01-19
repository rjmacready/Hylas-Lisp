/********************************************************************************
** Form generated from reading UI file 'tabsM10779.ui'
**
** Created: Sat Jan 19 15:39:45 2013
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef TABSM10779_H
#define TABSM10779_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QToolButton>
#include <QtGui/QWidget>
#include <QtWebKit/QWebView>

QT_BEGIN_NAMESPACE

class Ui_tabs
{
public:
    QWidget *repl_tab;
    QGridLayout *gridLayout;
    QWebView *console;
    QToolButton *bRestart;
    QToolButton *bSend;
    QSpacerItem *horizontalSpacer;
    QToolButton *bHistory;
    QTextEdit *input;
    QWidget *tab;
    QGridLayout *gridLayout_2;
    QToolButton *bBack;
    QToolButton *bHome;
    QSpacerItem *horizontalSpacer2;
    QToolButton *bForward;
    QWebView *docview;

    void setupUi(QTabWidget *tabs)
    {
        if (tabs->objectName().isEmpty())
            tabs->setObjectName(QString::fromUtf8("tabs"));
        tabs->resize(510, 548);
        repl_tab = new QWidget();
        repl_tab->setObjectName(QString::fromUtf8("repl_tab"));
        gridLayout = new QGridLayout(repl_tab);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        console = new QWebView(repl_tab);
        console->setObjectName(QString::fromUtf8("console"));
        console->setUrl(QUrl(QString::fromUtf8("about:blank")));

        gridLayout->addWidget(console, 0, 0, 1, 4);

        bRestart = new QToolButton(repl_tab);
        bRestart->setObjectName(QString::fromUtf8("bRestart"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/UI/syntagma/icons/restart.png"), QSize(), QIcon::Normal, QIcon::Off);
        bRestart->setIcon(icon);

        gridLayout->addWidget(bRestart, 2, 0, 1, 1);

        bSend = new QToolButton(repl_tab);
        bSend->setObjectName(QString::fromUtf8("bSend"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/UI/syntagma/icons/send.png"), QSize(), QIcon::Normal, QIcon::Off);
        bSend->setIcon(icon1);

        gridLayout->addWidget(bSend, 2, 3, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 2, 1, 1);

        bHistory = new QToolButton(repl_tab);
        bHistory->setObjectName(QString::fromUtf8("bHistory"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/UI/syntagma/icons/history.png"), QSize(), QIcon::Normal, QIcon::Off);
        bHistory->setIcon(icon2);

        gridLayout->addWidget(bHistory, 2, 1, 1, 1);

        input = new QTextEdit(repl_tab);
        input->setObjectName(QString::fromUtf8("input"));

        gridLayout->addWidget(input, 1, 0, 1, 4);

        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/UI/syntagma/icons/console.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabs->addTab(repl_tab, icon3, QString());
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout_2 = new QGridLayout(tab);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        bBack = new QToolButton(tab);
        bBack->setObjectName(QString::fromUtf8("bBack"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/UI/syntagma/icons/back.png"), QSize(), QIcon::Normal, QIcon::Off);
        bBack->setIcon(icon4);

        gridLayout_2->addWidget(bBack, 0, 2, 1, 1);

        bHome = new QToolButton(tab);
        bHome->setObjectName(QString::fromUtf8("bHome"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/UI/syntagma/icons/home.png"), QSize(), QIcon::Normal, QIcon::Off);
        bHome->setIcon(icon5);

        gridLayout_2->addWidget(bHome, 0, 0, 1, 1);

        horizontalSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer2, 0, 1, 1, 1);

        bForward = new QToolButton(tab);
        bForward->setObjectName(QString::fromUtf8("bForward"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/UI/syntagma/icons/fwd.png"), QSize(), QIcon::Normal, QIcon::Off);
        bForward->setIcon(icon6);

        gridLayout_2->addWidget(bForward, 0, 3, 1, 1);

        docview = new QWebView(tab);
        docview->setObjectName(QString::fromUtf8("docview"));
        docview->setUrl(QUrl(QString::fromUtf8("file:///mnt/D/Code/Hylas/docs/html/index.html")));

        gridLayout_2->addWidget(docview, 1, 0, 1, 4);

        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/UI/syntagma/icons/docs.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabs->addTab(tab, icon7, QString());

        retranslateUi(tabs);

        tabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(tabs);
    } // setupUi

    void retranslateUi(QTabWidget *tabs)
    {
        tabs->setWindowTitle(QApplication::translate("tabs", "TabWidget", 0, QApplication::UnicodeUTF8));
        bRestart->setText(QApplication::translate("tabs", "...", 0, QApplication::UnicodeUTF8));
        bSend->setText(QString());
        bHistory->setText(QApplication::translate("tabs", "...", 0, QApplication::UnicodeUTF8));
        tabs->setTabText(tabs->indexOf(repl_tab), QApplication::translate("tabs", "Console", 0, QApplication::UnicodeUTF8));
        bBack->setText(QApplication::translate("tabs", "...", 0, QApplication::UnicodeUTF8));
        bHome->setText(QApplication::translate("tabs", "...", 0, QApplication::UnicodeUTF8));
        bForward->setText(QApplication::translate("tabs", "...", 0, QApplication::UnicodeUTF8));
        tabs->setTabText(tabs->indexOf(tab), QApplication::translate("tabs", "Documentation", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class tabs: public Ui_tabs {};
} // namespace Ui

QT_END_NAMESPACE

#endif // TABSM10779_H
