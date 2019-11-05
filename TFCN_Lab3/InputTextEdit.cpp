#include "InputTextEdit.h"
#include <QByteArray>
InputTextEdit::InputTextEdit(QWidget *frame)
    :QTextEdit(frame)
{

}

void InputTextEdit::keyPressEvent(QKeyEvent *event)
{
    QString oldText = toPlainText();
    QString str = event->text();
    QByteArray arr;
    arr.append(str.toUtf8());

    if (arr.length() < 2 && !(event->key() == Qt::Key::Key_Backspace))
    {
        setText(oldText+str);
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        setTextCursor(cursor);
    }
}
