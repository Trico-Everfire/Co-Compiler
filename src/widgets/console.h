#pragma once

#include <QTextEdit>

class Console : public QTextEdit
{

    signals:
            void getData(const QByteArray &data);

public:
    explicit Console(QWidget *parent = nullptr);

    void putPlainData(const QByteArray &data);
    void putHTMLData(const QByteArray &data);

    void setLocalEchoEnabled(bool set);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;

private:
    bool m_localEchoEnabled = false;
};