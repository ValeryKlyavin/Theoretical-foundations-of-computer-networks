from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(586, 593)
        MainWindow.setWindowTitle("Lab4")
        self.centralWidget = QtWidgets.QWidget(MainWindow)
        self.centralWidget.setObjectName("centralWidget")
        self.gridLayout = QtWidgets.QGridLayout(self.centralWidget)
        self.gridLayout.setContentsMargins(11, 11, 11, 11)
        self.gridLayout.setSpacing(6)
        self.gridLayout.setObjectName("gridLayout")
        self.inputTextLabel = QtWidgets.QLabel(self.centralWidget)
        font = QtGui.QFont()
        font.setPointSize(12)
        self.inputTextLabel.setFont(font)
        self.inputTextLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.inputTextLabel.setObjectName("inputTextLabel")
        self.gridLayout.addWidget(self.inputTextLabel, 0, 0, 1, 1)
        self.textEdit = TextEdit(self.centralWidget)
        self.textEdit.setObjectName("textEdit")
        self.gridLayout.addWidget(self.textEdit, 1, 0, 1, 1)
        self.outputTextLabel = QtWidgets.QLabel(self.centralWidget)
        font = QtGui.QFont()
        font.setPointSize(12)
        self.outputTextLabel.setFont(font)
        self.outputTextLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.outputTextLabel.setObjectName("outputTextLabel")
        self.gridLayout.addWidget(self.outputTextLabel, 2, 0, 1, 1)
        self.outputTextEdit = QtWidgets.QTextEdit(self.centralWidget)
        self.outputTextEdit.setReadOnly(True)
        self.outputTextEdit.setObjectName("outputTextEdit")
        self.gridLayout.addWidget(self.outputTextEdit, 3, 0, 1, 1)
        self.DebugAndControlLabel = QtWidgets.QLabel(self.centralWidget)
        font = QtGui.QFont()
        font.setPointSize(12)
        self.DebugAndControlLabel.setFont(font)
        self.DebugAndControlLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.DebugAndControlLabel.setObjectName("DebugAndControlLabel")
        self.gridLayout.addWidget(self.DebugAndControlLabel, 4, 0, 1, 1)
        self.horizontalLayout = QtWidgets.QHBoxLayout()
        self.horizontalLayout.setSpacing(6)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.burstModeLabel = QtWidgets.QLabel(self.centralWidget)
        self.burstModeLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.burstModeLabel.setObjectName("burstModeLabel")
        self.horizontalLayout.addWidget(self.burstModeLabel)
        self.bustModeCheckBox = QtWidgets.QCheckBox(self.centralWidget)
        self.bustModeCheckBox.setObjectName("bustModeCheckBox")
        self.horizontalLayout.addWidget(self.bustModeCheckBox)
        self.gridLayout.addLayout(self.horizontalLayout, 5, 0, 1, 1)
        self.debugTextEdit = QtWidgets.QTextEdit(self.centralWidget)
        self.debugTextEdit.setReadOnly(True)
        self.debugTextEdit.setObjectName("debugTextEdit")
        self.gridLayout.addWidget(self.debugTextEdit, 6, 0, 1, 1)
        MainWindow.setCentralWidget(self.centralWidget)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "TFCN_Lab4"))
        self.inputTextLabel.setText(_translate("MainWindow", "Input:"))
        self.outputTextLabel.setText(_translate("MainWindow", "Output:"))
        self.DebugAndControlLabel.setText(_translate("MainWindow", "Debug & Control:"))
        self.burstModeLabel.setText(_translate("MainWindow", "Burst mode"))
        self.bustModeCheckBox.setText(_translate("MainWindow", "Disabled"))


class TextEdit(QtWidgets.QTextEdit):
    def __init__(self, parent=None):
        super().__init__(parent)

    def keyPressEvent(self, e: QtGui.QKeyEvent):
        oldText = self.toPlainText()
        str = e.text()

        if not (e.key() == QtCore.Qt.Key_Backspace):
            self.setText(oldText + str)
            cursor = self.textCursor()
            cursor.movePosition(QtGui.QTextCursor.End, QtGui.QTextCursor.MoveAnchor)
            self.setTextCursor(cursor)
