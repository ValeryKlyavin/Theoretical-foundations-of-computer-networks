import sys
from PyQt5 import QtWidgets, QtCore
import datetime
import time
import random
import TFCN_Lab4_Ui

tries = 0
COLLISION_WINDOW = 100
MAX_TRIES = 10
SLOT_TIME = 220


def usleep(usec):
    time.sleep(usec / 1000000.0)


def isCollision():
    return datetime.datetime.now().time().microsecond % 2 == 0


def isChannelFree():
    return datetime.datetime.now().time().microsecond % 2 == 1


class Window(QtWidgets.QMainWindow, TFCN_Lab4_Ui.Ui_MainWindow):
    burstMode = False
    packet = []
    recievedPacket = []
    dataReady = QtCore.pyqtSignal()

    def changeBurstMode(self):
        self.burstMode = self.bustModeCheckBox.isChecked()
        if self.burstMode:
            self.bustModeCheckBox.setText("Enabled")
        else:
            self.bustModeCheckBox.setText("Disabled")

    def transmitData(self):
        self.recievedPacket = self.packet

    def dataReadyHandler(self):
        tries = 0
        while True:
            channelState = isChannelFree()
            while not channelState:
                channelState = isChannelFree()
            self.transmitData()
            usleep(COLLISION_WINDOW)
            if isCollision():
                usleep(COLLISION_WINDOW)
                tries += 1
                if self.burstMode and isCollision():
                    self.debugTextEdit.setText(self.debugTextEdit.toPlainText() + "X!")
                    break
                if tries < MAX_TRIES:
                    self.debugTextEdit.setText(self.debugTextEdit.toPlainText() + "X")
                    usleep(random.randint(0, pow(2, tries)) * SLOT_TIME)
                    continue
                else:
                    break

            else:
                text = self.outputTextEdit.toPlainText()
                packetContent = ""
                for symbol in self.recievedPacket:
                    packetContent += symbol
                self.outputTextEdit.setText(text + packetContent)

                break
        self.debugTextEdit.append("")
        self.recievedPacket.clear()
        self.packet.clear()
        self.debugTextEdit.verticalScrollBar().setValue(self.debugTextEdit.verticalScrollBar().maximum())
        self.outputTextEdit.verticalScrollBar().setValue(self.outputTextEdit.verticalScrollBar().maximum())


    def inputHandler(self):
        self.packet.append(self.textEdit.toPlainText()[len(self.textEdit.toPlainText()) - 1])
        if self.burstMode:
            if len(self.packet) < 4:
                self.bustModeCheckBox.setEnabled(False)
            else:
                self.dataReady.emit()
        else:
            self.dataReady.emit()

    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.textEdit.textChanged.connect(self.inputHandler)
        self.bustModeCheckBox.stateChanged.connect(self.changeBurstMode)
        self.dataReady.connect(self.dataReadyHandler)


def main():
    app = QtWidgets.QApplication(sys.argv)
    window = Window()
    window.show()
    app.exec()


if __name__ == '__main__':
    main()
