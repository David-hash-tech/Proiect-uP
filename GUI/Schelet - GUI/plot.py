from PySide6.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QHBoxLayout, QWidget, QGroupBox, QLabel, QPushButton, QTextEdit
from PySide6.QtGui import QIcon, QFont
from PySide6.QtCore import Qt, QTimer
import pyqtgraph as pg
import serial

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Proiect Microprocesoare 2023-2024")
        self.setWindowIcon(QIcon("./icon.png"))

        # Serial communication setup
        self.serObj = serial.Serial('COM7')
        self.serObj.baudrate = 9600

        # Team details
        self.team = [
            "Bureacă Angela-Emilia",
            "Huțanu David",
        ]

        # Layouts
        primary_layout = QVBoxLayout()
        secondary_layout = QHBoxLayout()
        tertiary_layout = QVBoxLayout()

        # Team information box
        team_box = QGroupBox("Membrii echipei")
        bold_font = QFont()
        bold_font.setBold(True)
        team_box.setFont(bold_font)

        first_member = QLabel(f"Membru 1: {self.team[0]}")
        second_member = QLabel(f"Membru 2: {self.team[1]}")
        team_box_layout = QVBoxLayout()
        team_box_layout.addWidget(first_member)
        team_box_layout.addWidget(second_member)
        team_box.setLayout(team_box_layout)

        # Control Panel box
        control_panel_box = QGroupBox("Control Panel")
        control_panel_box.setFont(bold_font)

        button1 = QPushButton("Switch order color")
        button1.clicked.connect(self.send_to_microcontroller)
        control_panel_box_layout = QVBoxLayout()
        control_panel_box_layout.addWidget(button1)
        control_panel_box_layout.addStretch()
        control_panel_box.setLayout(control_panel_box_layout)

        tertiary_layout.addWidget(team_box)
        tertiary_layout.addWidget(control_panel_box)

        # Graph setup
        self.graphWidget = pg.PlotWidget()
        self.graphWidget.setBackground('w')
        self.graphWidget.setTitle("Grafic cu valorile senzorului de rotatie")
        self.graphWidget.setLabel('left', 'Rotation Values', color='k', size=10)
        self.graphWidget.setLabel('bottom', 'Time', color='k', size=10)
        self.graphWidget.showGrid(x=True, y=True)
        self.graphWidget.setYRange(-20, 300, padding=0)

        self.rot = 0
        self.lastValue = 0

        self.x = list(range(200))
        self.y = [0] * 200

        self.data_line = self.graphWidget.plot(self.x, self.y, name="Rot", pen=pg.mkPen(color=(0, 0, 255)))

        secondary_layout.addWidget(self.graphWidget)
        secondary_layout.addLayout(tertiary_layout)

        primary_layout.addLayout(secondary_layout)
        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)

        widget = QWidget()
        widget.setLayout(primary_layout)
        
        self.setCentralWidget(widget)

        # Timer for updating graph data
        self.timer = QTimer(self)
        self.timer.setInterval(1)
        self.timer.timeout.connect(self.update_plot_data)
        self.timer.start()

    def send_to_microcontroller(self):
        # Sending command to the microcontroller
        command = b'1'
        self.serObj.write(command)

    def update_plot_data(self):
        self.x = self.x[1:] # Remove the first y element.
        self.x.append(self.x[-1] + 1) # Add a new value 1 higher than the last.

        received_string = self.serObj.readline()

        received_string = received_string.decode('utf', errors='replace').strip("\n").strip("\r")

        try:
            self.rot = int(received_string)

            if self.rot < self.lastValue - 1 or self.rot > self.lastValue + 1 or self.lastValue < 1:
                self.lastValue = self.rot
            else:
               self.rot = self.lastValue
        except (ValueError, IndexError):
            self.rot = 0

        print(self.rot)
        self.y = self.y[1:]
        self.y.append(self.rot)
        self.data_line.setData(self.x, self.y)


def main():
    app = QApplication([])
    window = MainWindow()
    window.setGeometry(0, 0, 800, 600)
    screen_geometry = window.screen().availableGeometry()
    screenX = (screen_geometry.width() - window.width()) / 2
    screenY = (screen_geometry.height() - window.height()) / 2
    window.move(screenX, screenY)
    window.show()
    app.exec()


if __name__ == "__main__":
    main()
