#ifndef QWCFILEDIALOG_H
#define QWCFILEDIALOG_H

#include <QFileDialog>
#include <QCheckBox>


//QFileDialog extended to house cmdseq.wc specific checkboxes
class QWCFileDialog : public QFileDialog
{
    Q_OBJECT
    Q_PROPERTY(bool wcBinary MEMBER wcBinary);
    Q_PROPERTY(bool wcCSGO MEMBER wcCSGO);
  public:
    QWCFileDialog(QWidget *parent,
                  const QString &caption,
                  const QString &directory,
                  const QString &filter,bool isWritingFile);
    bool wcBinary = false;
    bool wcCSGO = false;

  private:
    QCheckBox* binaryWCCheckBox;
    QCheckBox* csgoWCCheckBox;
    bool isWritingFile;

    void onFilterChange(const QString&);
    void onBinaryWCCheckBoxToggle(Qt::CheckState);
    void onCSGOWCCheckBoxToggle(Qt::CheckState);
};

#endif // QWCFILEDIALOG_H
