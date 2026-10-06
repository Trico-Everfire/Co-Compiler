#include "qwcfiledialog.h"
#include <QFileDialog>
#include <QGridLayout>
QWCFileDialog::QWCFileDialog(QWidget *parent,
                             const QString &caption,
                             const QString &directory,
                             const QString &filter,bool isWritingFile) : QFileDialog(parent,caption,directory,filter)
{
    setOption(QFileDialog::DontUseNativeDialog); //this early beacuse we need to append two things.
    QGridLayout* mainLayout{ dynamic_cast<QGridLayout*>(this->layout()) };
    binaryWCCheckBox = new QCheckBox();
    binaryWCCheckBox->setText("Is WC Binary?");
    //this->addWidgets( 0, binaryWCCheckBox, 0 );
    csgoWCCheckBox = new QCheckBox();
    csgoWCCheckBox->setText("Is WC for CS:GO?");
    //HACK: This is tail of the widget. Make this more robust.
    if(isWritingFile)
    {
        mainLayout->addWidget(binaryWCCheckBox,8,0);
        mainLayout->addWidget(csgoWCCheckBox,8,1);
    }
    else
    {
        mainLayout->addWidget(csgoWCCheckBox,8,0);
    }

    connect(this,&QFileDialog::filterSelected,this,&QWCFileDialog::onFilterChange);
    connect(binaryWCCheckBox,&QCheckBox::checkStateChanged,this,&QWCFileDialog::onBinaryWCCheckBoxToggle);
    connect(csgoWCCheckBox,&QCheckBox::checkStateChanged,this,&QWCFileDialog::onCSGOWCCheckBoxToggle);

    binaryWCCheckBox->setEnabled(false);
    csgoWCCheckBox->setEnabled(false);
    this->isWritingFile = isWritingFile;
    //this->addWidgets( 0, csgoWCCheckBox, 0 );
}

void QWCFileDialog::onFilterChange(const QString & filter)
{
    if (filter == "Hammer Config (cmdseq.wc)")
    {
        if(isWritingFile)
        {
            binaryWCCheckBox->setEnabled(true);
            emit binaryWCCheckBox->checkStateChanged(binaryWCCheckBox->checkState());
        }
        else
        {
            csgoWCCheckBox->setEnabled(true);
        }
    }
    else
    {
        binaryWCCheckBox->setEnabled(false);
        csgoWCCheckBox->setEnabled(false);
    }
}

void QWCFileDialog::onBinaryWCCheckBoxToggle(Qt::CheckState state)
{

    switch(state){
    case Qt::CheckState::Checked:
        csgoWCCheckBox->setEnabled(true);
        wcBinary=true;
        break;
    case Qt::CheckState::Unchecked:
        csgoWCCheckBox->setEnabled(false);
        wcBinary=false;
        break;
    default:
        throw;
    }

}

void QWCFileDialog::onCSGOWCCheckBoxToggle(Qt::CheckState state)
{
    switch(state){
    case Qt::CheckState::Checked:
        wcCSGO=true;
        break;
    case Qt::CheckState::Unchecked:
        wcCSGO=false;
        break;
    default:
        throw;
    }
}
