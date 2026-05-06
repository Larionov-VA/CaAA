import sys
import os
import subprocess
from PySide6.QtWidgets import (QApplication,QWidget,QVBoxLayout,QHBoxLayout,
                               QLineEdit,QPushButton,QLabel,QTextEdit,QTextBrowser)
from PySide6.QtCore import Qt

def get_resource_path(relative_path):
    try:
        # Путь, создаваемый PyInstaller во временной папке
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")

    return os.path.join(base_path, relative_path)

class WagnerFischerApp(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle("Wagner-Fischer Algorithm")
        self.resize(400, 400)

        layout = QVBoxLayout()

        # Текст-описание в верхней части утилиты
        text_discription = ("Утилита вычисляет расстояние Левенштейна"
                            " и редакционное предписание между двумя строками.")
        text_area = QTextBrowser()
        text_area.setPlainText(text_discription)

        # Поля для строк
        self.s1_input = QLineEdit(placeholderText="Первая строка")
        self.s2_input = QLineEdit(placeholderText="Вторая строка")

        # Настройки особых символов
        grid_layout = QVBoxLayout()

        # Группа для замены
        replace_layout = QHBoxLayout()
        self.target_char = QLineEdit(placeholderText="Символ")
        self.target_char.setMaxLength(1)
        self.replace_cost = QLineEdit(placeholderText="Цена замены")
        replace_layout.addWidget(QLabel("Особая замена на:"))
        replace_layout.addWidget(self.target_char)
        replace_layout.addWidget(self.replace_cost)

        # Группа для удаления
        delete_layout = QHBoxLayout()
        self.delete_char = QLineEdit(placeholderText="Символ")
        self.delete_char.setMaxLength(1)
        self.delete_cost = QLineEdit(placeholderText="Цена удаления")
        delete_layout.addWidget(QLabel("Особое удаление: "))
        delete_layout.addWidget(self.delete_char)
        delete_layout.addWidget(self.delete_cost)

        # Кнопка запуска
        self.run_btn = QPushButton("Вычичислить")
        self.run_btn.setFixedHeight(40)
        self.run_btn.setStyleSheet("background-color: #2b5797; color: white; font-weight: bold;")
        self.run_btn.clicked.connect(self.run_algorithm)

        # Вывод
        self.result_display = QTextEdit()
        self.result_display.setReadOnly(True)

        # Сборка интерфейса
        layout.addWidget(text_area)
        layout.addSpacing(10)
        layout.addWidget(QLabel("Введите строки:"))
        layout.addWidget(self.s1_input)
        layout.addWidget(self.s2_input)
        layout.addSpacing(10)
        layout.addLayout(replace_layout)
        layout.addLayout(delete_layout)
        layout.addSpacing(10)
        layout.addWidget(self.run_btn)
        layout.addWidget(QLabel("Вывод:"))
        layout.addWidget(self.result_display)

        self.setLayout(layout)

    def run_algorithm(self):
        # Собираем данные. Если поля пустые, ставим значения по умолчанию
        s1 = self.s1_input.text() or " "
        s2 = self.s2_input.text() or " "
        t_char = self.target_char.text() or "0"
        r_cost = self.replace_cost.text() or "1"
        d_char = self.delete_char.text() or "0"
        del_cost = self.delete_cost.text() or "1"

        input_str = f"{s1}\n{s2}\n{t_char} {r_cost} {d_char} {del_cost}\n"

        # Путь к бинарнику
        algo_path = get_resource_path("algo")

        if not os.path.exists(algo_path):
            self.result_display.setText(f"Ошибка: Файл 'algo' не найден по пути {algo_path}")
            return

        try:
            process = subprocess.Popen(
                [algo_path],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            stdout, stderr = process.communicate(input=input_str)

            if stderr:
                self.result_display.setText(f"Ошибка выполнения:\n{stderr}")
            else:
                self.result_display.setText(stdout)
        except Exception as e:
            self.result_display.setText(f"Критическая ошибка: {str(e)}")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = WagnerFischerApp()
    window.show()
    sys.exit(app.exec())