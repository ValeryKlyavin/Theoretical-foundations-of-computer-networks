#ifndef INPUTTEXTEDIT_H
#define INPUTTEXTEDIT_H

#include <QObject>
#include <QTextEdit>
#include <QKeyEvent>

class InputTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit InputTextEdit(QWidget *parent = nullptr);

protected slots:
    virtual void keyPressEvent(QKeyEvent *event);

};

#endif // INPUTTEXTEDIT_H


