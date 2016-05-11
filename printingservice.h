#ifndef PRINTINGSERVICE_H
#define PRINTINGSERVICE_H
#include <QTableView>
#include <QString>
#include <QTextDocument>
#include <QTextStream>
#include <QPrintDialog>
#include <QPrinter>


class PrintingService
{
public:
    void printThis(QTableView *tableToPrint);

};

#endif // PRINTINGSERVICE_H
