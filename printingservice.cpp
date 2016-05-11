#include "printingservice.h"

void PrintingService::printThis(QTableView *tableToPrint){
    QString strStream;
    QTextStream out(&strStream);
    QString strTitle;

    const int rowCount = tableToPrint->model()->rowCount();
    const int columnCount = tableToPrint->model()->columnCount();

    out <<  "<html>\n <head>\n <meta Content=\"Text/html; charset=Windows-1251\">\n"
         <<  QString("<title>%1</title>\n").arg(strTitle)
          <<  "</head>\n"
              "<body bgcolor=#ffffff link=#5000A0>\n"
              "<table border=1 cellspacing=0 cellpadding=2>\n";

    // headers
    out << "<thead><tr bgcolor=#f0f0f0>";
    for (int column = columnCount-1; column >= 0 ; column--)
        if (!tableToPrint->isColumnHidden(column))
            out << QString("<th>%1</th>").arg(tableToPrint->model()->headerData(column, Qt::Horizontal).toString());
    out << "</tr></thead>\n";

    // data table
    for (int row = 0; row < rowCount; row++) {
        out << "<tr>";
        for (int column = columnCount-1; column >=0 ; column--) {
            if (!tableToPrint->isColumnHidden(column)) {
                QString data = tableToPrint->model()->data(tableToPrint->model()->index(row, column)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
            }
        }
        out << "</tr>\n";
    }
    out <<  "</table>\n"
            "</body>\n"
            "</html>\n";

    QTextDocument *document = new QTextDocument();
    document->setHtml(strStream);

    QPrinter printer;

    QPrintDialog *dialog = new QPrintDialog(&printer, NULL);
    if (dialog->exec() == QDialog::Accepted) {
        document->print(&printer);
    }

    delete document;
}



