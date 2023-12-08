from PySide6.QtWidgets import QApplication
from PySide6.QtGui import QScreen
from PySide6.QtWidgets import QMainWindow, QHBoxLayout, QVBoxLayout, QWidget, QGroupBox, QLabel, QPushButton, QLineEdit, QTextEdit
from PySide6.QtGui import QIcon, QPalette, QColor, QFont
from PySide6.QtCore import Qt
import pyqtgraph as pg
import serial

SerObj = serial.Serial('COM7')
class MainWindow(QMainWindow):
    SerObj.baudrate = 9600

    promotie: str = "2023-2024"
    team: list[str] = [
        "Bureacă Angela-Emilia",
        "Huțanu David",
    ]

    def send_to_microcontroller(self):
        # Trimite comanda către microcontroler
        command = b'1'
        SerObj.write(command)

    def __init__(self):
        super().__init__()
        self.setWindowTitle(f"Proiect Microprocesoare {self.promotie}")
        self.setWindowIcon(QIcon("./icon.png"))
        

        primary_layout = QVBoxLayout()
        secondary_layout = QHBoxLayout()
        tertiary_layout = QVBoxLayout()

        team_box = QGroupBox("Membrii echipei")
        bold_font = QFont()
        bold_font.setBold(True)
        team_box.setFont(bold_font)

        first_member = QLabel(f"Membru 1: {self.team[0]}")
        second_member = QLabel(f"Membru 2: {self.team[1]}")
        team_box_layout = QVBoxLayout()
        team_box_layout.addWidget(first_member,1)
        team_box_layout.addWidget(second_member,1)
        team_box.setLayout(team_box_layout)

        control_panel_box = QGroupBox("Control Panel")
        control_panel_box.setFont(bold_font)

        button1 = QPushButton("Switch order color")
        button1.clicked.connect(self.send_to_microcontroller)
        control_panel_box_layout = QVBoxLayout()
        control_panel_box_layout.setSpacing(5)
        control_panel_box_layout.addWidget(button1,1)
    
        control_panel_box_layout.addStretch()
        
        control_panel_box.setLayout(control_panel_box_layout)

        tertiary_layout.addWidget(team_box, 1)
        tertiary_layout.addWidget(control_panel_box,5)

        plot_widget = pg.PlotWidget()
        hour = [1,2,3,4,5,6,7,8,9,10]
        temperature = [30,32,34,32,33,31,29,32,35,45] # value senzor

        plot_widget.plot(hour, temperature)

        secondary_layout.addWidget(plot_widget, 3)
        secondary_layout.addLayout(tertiary_layout, 1)

        primary_layout.addLayout(secondary_layout, 4)
        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)

        widget = QWidget()
        widget.setLayout(primary_layout)
        
        self.setCentralWidget(widget)

app = QApplication([])
window = MainWindow()

window.setGeometry(0, 0, 800, 600)
screen_geometry = QScreen.availableGeometry(QApplication.primaryScreen())
screenX = (screen_geometry.width() - window.width()) / 2
screenY = (screen_geometry.height() - window.height()) / 2
window.move(screenY, screenY)
window.show()


app.exec()