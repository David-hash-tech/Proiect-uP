from PySide6.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QHBoxLayout, QGraphicsView, QGraphicsScene, QGraphicsProxyWidget, QWidget, QGroupBox, QLabel, QPushButton
from PySide6.QtGui import QIcon, QFont, QPainter
from PySide6.QtCore import QTimer
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
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

        # Matplotlib plot
        self.figure = Figure(figsize=(5, 4), dpi=100)
        self.canvas = FigureCanvas(self.figure)
        self.ax = self.figure.add_subplot(111)
        self.ax.set_ylabel('Rotation Values')
        self.ax.set_ylim(0, 300)  # Set y-axis limits from 0 to 300


        self.rot_values = []  # Store received values
        self.bar_positions = []  # Store bar positions
        self.num_values_to_display = 20  # Number of values to display on the plot

        for i in range(self.num_values_to_display):
            self.rot_values.append(0)
            self.bar_positions.append(i + 1)

        self.bars = self.ax.bar(self.bar_positions, self.rot_values)  # Initial empty bar plot
        self.canvas.draw()

        # QGraphicsView for embedding Matplotlib plot
        self.graphicsView = QGraphicsView()
        self.scene = QGraphicsScene(self)
        self.proxy = self.scene.addWidget(self.canvas)
        self.graphicsView.setScene(self.scene)
        self.graphicsView.setRenderHint(QPainter.Antialiasing)

        secondary_layout.addWidget(self.graphicsView)
        secondary_layout.addLayout(tertiary_layout)

        primary_layout.addLayout(secondary_layout)
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
        for _ in range(50):
            received_string = self.serObj.readline()

        received_string = received_string.decode('utf', errors='replace').strip("\n").strip("\r")

        try:
            rot = int(received_string)

            #if rot < self.rot_values[-1] - 1 or rot > self.rot_values[-1] + 1 or self.rot_values[-1] < 1:
                
                
            self.rot_values.pop(0)
            self.rot_values.append(rot)
            for i, bar in enumerate(self.bars):
                bar.set_height(self.rot_values[i])
                if self.rot_values[i] <= 100:
                    bar.set_color('green')
                elif 100 < self.rot_values[i] <= 200:
                    bar.set_color('yellow')
                else:
                    bar.set_color('red')
                    
            #else:
             #   rot = self.rot_values[-1]

            self.ax.set_xticklabels([])
            self.canvas.draw()
            print(rot)

        except (ValueError, IndexError):
            pass

def main():
    app = QApplication([])
    window = MainWindow()
    window.setGeometry(0, 0, 800, 600)
    window.show()
    app.exec()

if __name__ == "__main__":
    main()
