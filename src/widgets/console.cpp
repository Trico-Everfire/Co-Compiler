#include <QFontDatabase>
#include <QScrollBar>
#include "console.h"

Console::Console(QWidget *parent) :
        QTextEdit(parent)
{

    int id = QFontDatabase::addApplicationFont(":resource/fonts/ttf/JetBrainsMono-Light.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont jbm(family);

    document()->setMaximumBlockCount(200);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::white);
    setPalette(p);
    document()->setDefaultFont(jbm);
}

void Console::putPlainData(const QByteArray &data)
{
    insertPlainText(data);

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::putHTMLData(const QByteArray &data)
{
    insertHtml(data);

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::setLocalEchoEnabled(bool set)
{
    m_localEchoEnabled = set;
}

void Console::keyPressEvent(QKeyEvent *e)
{
    Q_UNUSED(e)
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}
