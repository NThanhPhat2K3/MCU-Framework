#!/usr/bin/env python3
"""
MCU-Framework Flasher v4 — STM32-style with Serial Monitor.
Works on Linux and Windows.
"""

from __future__ import annotations

import os
import platform
import sys
import threading
import time
from pathlib import Path

from PyQt5.QtCore import Qt, QTimer, pyqtSignal, pyqtSlot
from PyQt5.QtGui import QColor, QLinearGradient, QPainter, QPen
from PyQt5.QtWidgets import (
    QApplication,
    QCheckBox,
    QComboBox,
    QFileDialog,
    QFrame,
    QGridLayout,
    QGroupBox,
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QMainWindow,
    QMessageBox,
    QProgressBar,
    QPushButton,
    QSizePolicy,
    QSplitter,
    QTabWidget,
    QTextEdit,
    QVBoxLayout,
    QWidget,
)

# ── Serial port enumeration ─────────────────────────────────────────
try:
    from serial.tools import list_ports as _list_ports

    def _port_sort_key(info) -> tuple:
        dev = info.device
        if "ttyUSB" in dev:
            return (0, dev)
        if "ttyACM" in dev:
            return (0, dev)
        if platform.system() == "Windows" and hasattr(info, "vid") and info.vid:
            return (0, dev)
        return (1, dev)

    def get_serial_ports() -> list[str]:
        ports = sorted(_list_ports.comports(), key=_port_sort_key)
        return [p.device for p in ports]
except ImportError:
    def get_serial_ports() -> list[str]:
        return []


sys.path.insert(0, str(Path(__file__).resolve().parent))
from update_client import UpdateConfig, flash_image  # noqa: E402
import serial  # noqa: E402

TARGETS = {
    "STM32F411CE (BlackPill)": "0x08010000",
    "STM32F103 (BluePill)": "0x08008000",
}

BAUD_RATES = ["9600", "19200", "38400", "57600", "115200", "230400", "460800", "921600"]

# ── Shared light blue/white stylesheet ──────────────────────────────
STYLESHEET = """
QMainWindow, QWidget {
    background-color: #ecf0f5;
    color: #2c3e50;
    font-family: "Segoe UI", "Cantarell", "Noto Sans", Arial, sans-serif;
    font-size: 13px;
}

QGroupBox {
    background-color: #ffffff;
    border: 1px solid #d5dbe3;
    border-radius: 10px;
    margin-top: 18px;
    padding: 18px 14px 14px 14px;
    font-weight: 600;
    font-size: 13px;
}

QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    padding: 3px 12px;
    color: #2980b9;
    background-color: #ffffff;
    border: 1px solid #d5dbe3;
    border-bottom: none;
    border-radius: 4px;
    font-weight: 700;
}

QLabel { color: #2c3e50; font-size: 13px; background: transparent; border: none; }

QLabel#title_label { color: #ffffff; font-size: 22px; font-weight: 700; background: transparent; }
QLabel#subtitle_label { color: #d4e6f1; font-size: 12px; background: transparent; }

QComboBox, QLineEdit {
    background-color: #ffffff;
    border: 1.5px solid #bdc3c7;
    border-radius: 6px;
    padding: 7px 12px;
    color: #2c3e50;
    min-height: 24px;
    selection-background-color: #2980b9;
    selection-color: #ffffff;
}

QComboBox:hover, QLineEdit:hover { border: 1.5px solid #3498db; }
QComboBox:focus, QLineEdit:focus { border: 2px solid #2980b9; background-color: #eaf2f8; }

QComboBox::drop-down {
    subcontrol-origin: padding;
    subcontrol-position: top right;
    width: 30px;
    border: none;
    border-top-right-radius: 6px;
    border-bottom-right-radius: 6px;
}

QComboBox QAbstractItemView {
    background-color: #ffffff;
    border: 1px solid #bdc3c7;
    border-radius: 6px;
    color: #2c3e50;
    selection-background-color: #d4e6f1;
    selection-color: #2980b9;
    outline: none;
    padding: 4px;
    min-height: 120px;
}

QPushButton {
    background-color: #ffffff;
    border: 1.5px solid #bdc3c7;
    border-radius: 6px;
    padding: 8px 20px;
    color: #2c3e50;
    font-weight: 600;
    min-height: 24px;
}

QPushButton:hover { background-color: #eaf2f8; border: 1.5px solid #3498db; color: #2980b9; }
QPushButton:pressed { background-color: #d4e6f1; border: 1.5px solid #2980b9; }
QPushButton:disabled { background-color: #f0f0f0; color: #bdc3c7; border: 1.5px solid #e0e0e0; }

QPushButton#flash_button {
    background-color: #3498db; color: #ffffff; border: none;
    border-radius: 8px; font-size: 14px; font-weight: 700;
    min-height: 40px; padding: 10px 28px;
}
QPushButton#flash_button:hover { background-color: #2980b9; }
QPushButton#flash_button:pressed { background-color: #2471a3; }
QPushButton#flash_button:disabled { background-color: #d5dbe3; color: #ffffff; }

QPushButton#erase_button {
    background-color: #e74c3c; color: #ffffff; border: none;
    border-radius: 8px; font-weight: 700; min-height: 40px; padding: 10px 20px;
}
QPushButton#erase_button:hover { background-color: #c0392b; }
QPushButton#erase_button:pressed { background-color: #a93226; }
QPushButton#erase_button:disabled { background-color: #d5dbe3; color: #ffffff; }

QPushButton#browse_button {
    background-color: #3498db; color: #ffffff; border: none;
    border-radius: 6px; font-weight: 700; min-width: 80px;
}
QPushButton#browse_button:hover { background-color: #2980b9; }

QPushButton#refresh_button {
    background-color: #ffffff; border: 1.5px solid #bdc3c7;
    border-radius: 6px; min-width: 34px; max-width: 34px;
    min-height: 34px; max-height: 34px; padding: 0px; font-size: 16px;
}
QPushButton#refresh_button:hover { border: 2px solid #3498db; background-color: #eaf2f8; }

QPushButton#clear_button {
    background-color: transparent; color: #95a5a6;
    border: 1px solid #d5dbe3; border-radius: 4px;
    padding: 3px 12px; min-height: 20px; max-height: 26px;
    font-size: 11px; font-weight: normal;
}
QPushButton#clear_button:hover { color: #e74c3c; border: 1px solid #e74c3c; background-color: #fdf2f2; }
QPushButton#clear_button:pressed { background-color: #e74c3c; color: #ffffff; }

QPushButton#connect_button {
    background-color: #27ae60; color: #ffffff; border: none;
    border-radius: 6px; font-weight: 700; min-height: 32px; padding: 6px 20px;
}
QPushButton#connect_button:hover { background-color: #219a52; }
QPushButton#connect_button:pressed { background-color: #1e8449; }

QPushButton#disconnect_button {
    background-color: #e74c3c; color: #ffffff; border: none;
    border-radius: 6px; font-weight: 700; min-height: 32px; padding: 6px 20px;
}
QPushButton#disconnect_button:hover { background-color: #c0392b; }

QPushButton#send_button {
    background-color: #3498db; color: #ffffff; border: none;
    border-radius: 6px; font-weight: 700; min-width: 80px; min-height: 32px;
}
QPushButton#send_button:hover { background-color: #2980b9; }

QProgressBar {
    background-color: #d5dbe3; border: none; border-radius: 8px;
    min-height: 22px; max-height: 22px; text-align: center;
    font-weight: 700; font-size: 11px; color: #2c3e50;
}
QProgressBar::chunk { background-color: #3498db; border-radius: 8px; }

QTextEdit#log_view, QTextEdit#monitor_view {
    background-color: #1a1a2e; border: none;
    border-bottom-left-radius: 9px; border-bottom-right-radius: 9px;
    padding: 10px;
    font-family: "Cascadia Code", "JetBrains Mono", "Fira Code", "Consolas", monospace;
    font-size: 12px; color: #ecf0f1;
    selection-background-color: #2980b9;
}

QTabWidget::pane {
    border: 1px solid #d5dbe3; border-radius: 8px;
    background-color: #ecf0f5; top: -1px;
}

QTabBar::tab {
    background-color: #d5dbe3; color: #2c3e50;
    border: 1px solid #d5dbe3; border-bottom: none;
    border-top-left-radius: 6px; border-top-right-radius: 6px;
    padding: 8px 28px; margin-right: 2px; font-weight: 600; min-width: 132px;
}
QTabBar::tab:selected {
    background-color: #ecf0f5; color: #2980b9;
    border-bottom: 2px solid #2980b9;
}
QTabBar::tab:hover:!selected { background-color: #eaf2f8; }

QCheckBox { color: #2c3e50; font-size: 12px; spacing: 6px; }
QCheckBox::indicator {
    width: 16px; height: 16px; border: 1.5px solid #bdc3c7;
    border-radius: 3px; background-color: #ffffff;
}
QCheckBox::indicator:checked { background-color: #3498db; border-color: #2980b9; }

QLineEdit#send_input {
    background-color: #ffffff; border: 1.5px solid #bdc3c7;
    border-radius: 6px; padding: 8px 12px; color: #2c3e50;
    min-height: 20px; font-family: "Cascadia Code", "Consolas", monospace;
}

QScrollBar:vertical {
    background-color: #1a1a2e; width: 8px; margin: 0; border-radius: 4px;
}
QScrollBar::handle:vertical {
    background-color: #5d6d7e; border-radius: 4px; min-height: 30px;
}
QScrollBar::handle:vertical:hover { background-color: #3498db; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }

QStatusBar {
    background-color: #2980b9; color: #ffffff; font-size: 11px; padding: 2px 8px;
}

QSplitter::handle { background-color: #d5dbe3; height: 2px; }
QSplitter::handle:horizontal { width: 2px; }
"""


# ── Widgets ──────────────────────────────────────────────────────────
class StatusIndicator(QWidget):
    """Bigger status indicator with colored dot + label."""
    def __init__(self, color: str = "#e74c3c", size: int = 16, parent=None):
        super().__init__(parent)
        self._color = color
        self._size = size
        self.setFixedSize(size, size)

    def set_color(self, color: str) -> None:
        self._color = color
        self.update()

    def paintEvent(self, _event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        p.setPen(Qt.NoPen)
        p.setBrush(QColor(self._color))
        p.drawEllipse(0, 0, self._size, self._size)
        p.end()


class HeaderBanner(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setFixedHeight(76)
        layout = QHBoxLayout(self)
        layout.setContentsMargins(24, 0, 24, 0)

        text_layout = QVBoxLayout()
        text_layout.setSpacing(1)
        title = QLabel("MCU-Framework Flasher")
        title.setObjectName("title_label")
        text_layout.addWidget(title)
        subtitle = QLabel("UART Firmware Update  ·  Serial Monitor  ·  STM32 Bootloader Protocol")
        subtitle.setObjectName("subtitle_label")
        text_layout.addWidget(subtitle)
        layout.addLayout(text_layout)
        layout.addStretch()

        # Status indicator + label
        self.status_box = QHBoxLayout()
        self.status_box.setSpacing(8)
        self.indicator = StatusIndicator("#e74c3c", 16)
        self.status_box.addWidget(self.indicator)
        self.status_text = QLabel("Idle")
        self.status_text.setStyleSheet(
            "color: #d4e6f1; font-size: 12px; font-weight: 600; background: transparent;"
        )
        self.status_box.addWidget(self.status_text)
        layout.addLayout(self.status_box)

    def set_status(self, text: str, color: str) -> None:
        self.indicator.set_color(color)
        self.status_text.setText(text)
        if color == "#e74c3c":
            self.status_text.setStyleSheet(
                "color: #f5b7b1; font-size: 12px; font-weight: 600; background: transparent;"
            )
        elif color == "#f39c12":
            self.status_text.setStyleSheet(
                "color: #fdebd0; font-size: 12px; font-weight: 600; background: transparent;"
            )
        else:
            self.status_text.setStyleSheet(
                "color: #abebc6; font-size: 12px; font-weight: 600; background: transparent;"
            )

    def paintEvent(self, _event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        grad = QLinearGradient(0, 0, self.width(), 0)
        grad.setColorAt(0.0, QColor("#2471a3"))
        grad.setColorAt(0.5, QColor("#2980b9"))
        grad.setColorAt(1.0, QColor("#2471a3"))
        p.fillRect(self.rect(), grad)
        p.setPen(QPen(QColor("#1a5276"), 1))
        p.drawLine(0, self.height() - 1, self.width(), self.height() - 1)
        p.end()


class LogPanel(QFrame):
    """Reusable terminal-style panel."""
    def __init__(self, title: str, view_name: str, parent=None):
        super().__init__(parent)
        self.setObjectName("log_panel")
        self.setStyleSheet(
            "QFrame#log_panel { background-color: #ffffff; border: 1px solid #d5dbe3; border-radius: 8px; }"
        )
        self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)

        title_bar = QFrame()
        title_bar.setObjectName("log_title_bar")
        title_bar.setFixedHeight(36)
        title_bar.setStyleSheet(
            "QFrame#log_title_bar { background-color: #eaf2f8; border: none; "
            "border-bottom: 1px solid #d5dbe3; "
            "border-top-left-radius: 8px; border-top-right-radius: 8px; }"
        )
        tl = QHBoxLayout(title_bar)
        tl.setContentsMargins(14, 0, 8, 0)

        icon_lbl = QLabel("\U0001F4CB")
        icon_lbl.setStyleSheet("background: transparent; border: none; font-size: 14px;")
        icon_lbl.setFixedWidth(22)
        tl.addWidget(icon_lbl)

        t_lbl = QLabel(title)
        t_lbl.setStyleSheet(
            "color: #2980b9; font-weight: 700; font-size: 13px; background: transparent; border: none;"
        )
        tl.addWidget(t_lbl)
        tl.addStretch(1)

        self.clear_btn = QPushButton("\u2715  Clear")
        self.clear_btn.setObjectName("clear_button")
        self.clear_btn.setFixedHeight(24)
        self.clear_btn.setCursor(Qt.PointingHandCursor)
        tl.addWidget(self.clear_btn)
        layout.addWidget(title_bar)

        self.text_edit = QTextEdit()
        self.text_edit.setObjectName(view_name)
        self.text_edit.setReadOnly(True)
        self.text_edit.setMinimumHeight(180)
        layout.addWidget(self.text_edit, 1)

    def append(self, text: str) -> None:
        self.text_edit.append(text)
        sb = self.text_edit.verticalScrollBar()
        sb.setValue(sb.maximum())

    def clear(self) -> None:
        self.text_edit.clear()


# ── Helper: populate combo preserving selection ─────────────────────
def _update_combo(combo: QComboBox, new_items: list[str], known: set[str]) -> bool:
    """Update combo items if port list changed. Returns True if changed."""
    new_set = set(new_items)
    if new_set == known:
        return False
    current = combo.currentText()
    combo.blockSignals(True)
    combo.clear()
    if new_items:
        combo.addItems(new_items)
        if current in new_items:
            combo.setCurrentText(current)
    else:
        combo.addItem("(no ports found)")
    combo.blockSignals(False)
    return True


# ── Flasher tab ──────────────────────────────────────────────────────
class FlasherTab(QWidget):
    log_signal = pyqtSignal(str)
    progress_signal = pyqtSignal(int, int)
    done_signal = pyqtSignal(str)
    error_signal = pyqtSignal(str)

    def __init__(self, header: HeaderBanner, status_bar, parent=None):
        super().__init__(parent)
        self._header = header
        self._status_bar = status_bar
        self._is_flashing = False
        self._known_ports: set[str] = set()

        layout = QVBoxLayout(self)
        layout.setSpacing(14)
        layout.setContentsMargins(20, 16, 20, 16)

        splitter = QSplitter(Qt.Horizontal)
        splitter.setChildrenCollapsible(False)

        # ── Connection ──────────────────────────────────────────
        conn_group = QGroupBox("Connection")
        cg = QGridLayout(conn_group)
        cg.setSpacing(12)

        cg.addWidget(QLabel("Serial Port"), 0, 0)
        port_row = QHBoxLayout()
        port_row.setSpacing(8)
        self.port_combo = QComboBox()
        self.port_combo.setMinimumWidth(220)
        self.port_combo.setEditable(True)
        self.port_combo.setInsertPolicy(QComboBox.NoInsert)
        port_row.addWidget(self.port_combo, 1)
        self.refresh_btn = QPushButton("\u21BB")
        self.refresh_btn.setObjectName("refresh_button")
        self.refresh_btn.setToolTip("Scan & refresh serial ports")
        port_row.addWidget(self.refresh_btn)
        cg.addLayout(port_row, 0, 1)

        cg.addWidget(QLabel("Baud Rate"), 1, 0)
        self.baud_combo = QComboBox()
        self.baud_combo.addItems(BAUD_RATES)
        self.baud_combo.setCurrentText("115200")
        self.baud_combo.setEditable(True)
        cg.addWidget(self.baud_combo, 1, 1)

        cg.addWidget(QLabel("Target MCU"), 2, 0)
        self.target_combo = QComboBox()
        self.target_combo.addItems(TARGETS.keys())
        cg.addWidget(self.target_combo, 2, 1)

        cg.setColumnStretch(1, 1)
        splitter.addWidget(conn_group)

        # ── Firmware ────────────────────────────────────────────
        fw_group = QGroupBox("Firmware")
        fg = QGridLayout(fw_group)
        fg.setSpacing(12)

        fg.addWidget(QLabel("Binary File"), 0, 0)
        file_row = QHBoxLayout()
        file_row.setSpacing(8)
        self.file_edit = QLineEdit()
        self.file_edit.setPlaceholderText("Select firmware .bin file ...")
        file_row.addWidget(self.file_edit, 1)
        self.browse_btn = QPushButton("Browse")
        self.browse_btn.setObjectName("browse_button")
        file_row.addWidget(self.browse_btn)
        fg.addLayout(file_row, 0, 1)

        fg.addWidget(QLabel("App Address"), 1, 0)
        self.addr_edit = QLineEdit("0x08010000")
        self.addr_edit.setReadOnly(True)
        self.addr_edit.setStyleSheet(
            "QLineEdit { color: #2980b9; font-weight: 700; "
            "background-color: #eaf2f8; border: 1.5px solid #aed6f1; }"
        )
        fg.addWidget(self.addr_edit, 1, 1)

        fg.addWidget(QLabel("File Size"), 2, 0)
        self.size_label = QLabel("\u2014")
        self.size_label.setStyleSheet("color: #95a5a6;")
        fg.addWidget(self.size_label, 2, 1)

        fg.setColumnStretch(1, 1)
        splitter.addWidget(fw_group)
        splitter.setSizes([400, 420])
        layout.addWidget(splitter)

        # ── Actions ─────────────────────────────────────────────
        btn_row = QHBoxLayout()
        btn_row.setSpacing(12)
        self.flash_btn = QPushButton("\u26A1  Flash Firmware")
        self.flash_btn.setObjectName("flash_button")
        btn_row.addWidget(self.flash_btn)
        self.erase_btn = QPushButton("\U0001F5D1  Erase App")
        self.erase_btn.setObjectName("erase_button")
        btn_row.addWidget(self.erase_btn)
        btn_row.addStretch()
        layout.addLayout(btn_row)

        self.progress_bar = QProgressBar()
        self.progress_bar.setRange(0, 100)
        self.progress_bar.setValue(0)
        self.progress_bar.setFormat("%p%")
        layout.addWidget(self.progress_bar)

        self.status_label = QLabel("Ready")
        self.status_label.setStyleSheet("color: #95a5a6; font-size: 12px;")
        layout.addWidget(self.status_label)

        self.log_panel = LogPanel("Log Output", "log_view")
        layout.addWidget(self.log_panel, 1)

        # ── Connections ─────────────────────────────────────────
        self.refresh_btn.clicked.connect(self._refresh_ports)
        self.browse_btn.clicked.connect(self._browse_file)
        self.file_edit.textChanged.connect(self._update_file_info)
        self.file_edit.textChanged.connect(self._validate)
        self.target_combo.currentTextChanged.connect(self._on_target_change)
        self.flash_btn.clicked.connect(self._start_flash)
        self.erase_btn.clicked.connect(self._start_erase)
        self.log_panel.clear_btn.clicked.connect(self.log_panel.clear)

        self.log_signal.connect(self._on_log)
        self.progress_signal.connect(self._on_progress)
        self.done_signal.connect(self._on_done)
        self.error_signal.connect(self._on_error)

        self._validate()
        self._on_target_change()
        self._refresh_ports()

        self._scan_timer = QTimer(self)
        self._scan_timer.timeout.connect(self._auto_scan)
        self._scan_timer.start(2000)

    def _refresh_ports(self) -> None:
        ports = get_serial_ports()
        _update_combo(self.port_combo, ports, self._known_ports)
        self._known_ports = set(ports)

    def _auto_scan(self) -> None:
        ports = get_serial_ports()
        if _update_combo(self.port_combo, ports, self._known_ports):
            self._known_ports = set(ports)

    def _browse_file(self) -> None:
        path, _ = QFileDialog.getOpenFileName(
            self, "Select Firmware Binary", "", "Binary Images (*.bin);;All Files (*)")
        if path:
            self.file_edit.setText(path)

    def _on_target_change(self) -> None:
        self.addr_edit.setText(TARGETS.get(self.target_combo.currentText(), "0x08010000"))

    def _update_file_info(self, path: str) -> None:
        if path and os.path.isfile(path):
            size = os.path.getsize(path)
            if size < 1024:
                s = f"{size} B"
            elif size < 1024 * 1024:
                s = f"{size / 1024:.1f} KB"
            else:
                s = f"{size / (1024 * 1024):.2f} MB"
            self.size_label.setText(f"{s}  ({size:,} bytes)")
            self.size_label.setStyleSheet("color: #2980b9; font-weight: 600;")
        else:
            self.size_label.setText("\u2014")
            self.size_label.setStyleSheet("color: #95a5a6;")

    def _validate(self) -> None:
        port = self.port_combo.currentText().strip()
        fp = self.file_edit.text().strip()
        port_ok = bool(port) and "(no ports" not in port
        file_ok = bool(fp) and os.path.isfile(fp)
        self.flash_btn.setEnabled(port_ok and file_ok and not self._is_flashing)
        self.erase_btn.setEnabled(port_ok and not self._is_flashing)

    def _set_busy(self, busy: bool) -> None:
        self._is_flashing = busy
        self._validate()
        if busy:
            self._header.set_status("Flashing ...", "#f39c12")
            self.status_label.setText("Flashing in progress ...")
            self.status_label.setStyleSheet("color: #f39c12; font-size: 12px; font-weight: 600;")
        else:
            self._header.set_status("Idle", "#27ae60")

    def _make_config(self) -> UpdateConfig | None:
        return self._make_config_require_file(require_file=True)

    def _make_config_require_file(self, require_file: bool) -> UpdateConfig | None:
        port = self.port_combo.currentText().strip()
        if not port or "(no ports" in port:
            QMessageBox.warning(self, "No Port", "Please select a serial port.")
            return None
        fp = self.file_edit.text().strip()
        if require_file and (not fp or not os.path.isfile(fp)):
            QMessageBox.warning(self, "No Firmware", "Please select a valid .bin file.")
            return None
        try:
            return UpdateConfig(port=port, baud=int(self.baud_combo.currentText()),
                                app_bin=fp, app_addr=int(self.addr_edit.text(), 0))
        except ValueError as e:
            QMessageBox.warning(self, "Invalid Input", str(e))
            return None

    def _start_flash(self) -> None:
        cfg = self._make_config()
        if not cfg:
            return
        self.log_panel.clear()
        self.progress_bar.setValue(0)
        self._set_busy(True)
        self.log_panel.append("\u2550" * 44)
        self.log_panel.append("  Starting firmware update session ...")
        self.log_panel.append("\u2550" * 44)
        threading.Thread(target=self._flash_worker, args=(cfg,), daemon=True).start()

    def _start_erase(self) -> None:
        cfg = self._make_config_require_file(require_file=False)
        if not cfg:
            return
        self.log_panel.clear()
        self._set_busy(True)
        self.log_panel.append("\u2550" * 44)
        self.log_panel.append("  Erasing app region ...")
        self.log_panel.append("\u2550" * 44)
        threading.Thread(target=self._erase_worker, args=(cfg,), daemon=True).start()

    def _flash_worker(self, cfg):
        try:
            flash_image(cfg, log=lambda m: self.log_signal.emit(m),
                        progress=lambda d, t: self.progress_signal.emit(d, t))
            self.done_signal.emit("Firmware flashed successfully!")
        except Exception as e:
            self.error_signal.emit(str(e))

    def _erase_worker(self, cfg):
        try:
            from update_client import _check_response, _normalize_response, _wait_for_programmer, send_packet
            with serial.Serial(cfg.port, cfg.baud, timeout=cfg.timeout) as port:
                time.sleep(0.8)
                self.log_signal.emit("Waiting for programmer ...")
                _wait_for_programmer(port, lambda m: self.log_signal.emit(m))
                port.reset_input_buffer()
                self.log_signal.emit("Sending erase command ...")
                resp = _normalize_response(send_packet(port, b"E", timeout_s=30.0))
                if resp:
                    self.log_signal.emit(resp)
                _check_response(resp, "OK ERASE")
            self.done_signal.emit("App region erased successfully!")
        except Exception as e:
            self.error_signal.emit(str(e))

    @pyqtSlot(str)
    def _on_log(self, msg):
        self.log_panel.append(msg)

    @pyqtSlot(int, int)
    def _on_progress(self, done, total):
        pct = 0 if total == 0 else int(done * 100 / total)
        self.progress_bar.setValue(pct)
        txt = (f"Flashing ... {done / 1024:.1f}/{total / 1024:.1f} KB  ({pct}%)"
               if total >= 1024 else f"Flashing ... {done}/{total} bytes  ({pct}%)")
        self.status_label.setText(txt)
        self._status_bar.showMessage(f"Writing: {done:,} / {total:,} bytes")

    @pyqtSlot(str)
    def _on_done(self, msg):
        self._set_busy(False)
        self.progress_bar.setValue(100)
        self.status_label.setText(msg)
        self.status_label.setStyleSheet("color: #27ae60; font-size: 12px; font-weight: 700;")
        self._header.set_status("Done", "#27ae60")
        self.log_panel.append("")
        self.log_panel.append(f"\u2713  {msg}")
        self._status_bar.showMessage(msg, 5000)
        QMessageBox.information(self, "Success", msg)

    @pyqtSlot(str)
    def _on_error(self, msg):
        self._set_busy(False)
        self.status_label.setText(f"Error: {msg}")
        self.status_label.setStyleSheet("color: #e74c3c; font-size: 12px;")
        self._header.set_status("Error", "#e74c3c")
        self.log_panel.append("")
        self.log_panel.append(f"\u2717  ERROR: {msg}")
        self._status_bar.showMessage(f"Failed: {msg}", 10000)
        QMessageBox.critical(self, "Flash Failed", msg)


# ── Serial Monitor tab ──────────────────────────────────────────────
class MonitorTab(QWidget):
    data_signal = pyqtSignal(str)

    def __init__(self, header: HeaderBanner, status_bar, parent=None):
        super().__init__(parent)
        self._header = header
        self._status_bar = status_bar
        self._serial_port: serial.Serial | None = None
        self._running = False
        self._known_ports: set[str] = set()

        layout = QVBoxLayout(self)
        layout.setSpacing(14)
        layout.setContentsMargins(20, 16, 20, 16)

        # ── Controls ────────────────────────────────────────────
        ctrl_group = QGroupBox("Monitor Settings")
        cg = QGridLayout(ctrl_group)
        cg.setSpacing(12)

        cg.addWidget(QLabel("Serial Port"), 0, 0)
        port_row = QHBoxLayout()
        port_row.setSpacing(8)
        self.port_combo = QComboBox()
        self.port_combo.setMinimumWidth(220)
        self.port_combo.setEditable(True)
        self.port_combo.setInsertPolicy(QComboBox.NoInsert)
        port_row.addWidget(self.port_combo, 1)
        self.refresh_btn = QPushButton("\u21BB")
        self.refresh_btn.setObjectName("refresh_button")
        self.refresh_btn.setToolTip("Scan & refresh")
        port_row.addWidget(self.refresh_btn)
        cg.addLayout(port_row, 0, 1)

        cg.addWidget(QLabel("Baud Rate"), 1, 0)
        self.baud_combo = QComboBox()
        self.baud_combo.addItems(BAUD_RATES)
        self.baud_combo.setCurrentText("115200")
        self.baud_combo.setEditable(True)
        cg.addWidget(self.baud_combo, 1, 1)

        # Status indicator
        cg.addWidget(QLabel("Status"), 2, 0)
        self.conn_status = QLabel("\u26AB  Disconnected")
        self.conn_status.setStyleSheet("color: #e74c3c; font-size: 13px; font-weight: 600;")
        cg.addWidget(self.conn_status, 2, 1)

        # Buttons
        btn_row = QHBoxLayout()
        btn_row.setSpacing(10)
        self.connect_btn = QPushButton("\u25B6  Connect")
        self.connect_btn.setObjectName("connect_button")
        btn_row.addWidget(self.connect_btn)
        self.disconnect_btn = QPushButton("\u25A0  Disconnect")
        self.disconnect_btn.setObjectName("disconnect_button")
        self.disconnect_btn.setEnabled(False)
        btn_row.addWidget(self.disconnect_btn)

        self.auto_scroll_cb = QCheckBox("Auto-scroll")
        self.auto_scroll_cb.setChecked(True)
        btn_row.addWidget(self.auto_scroll_cb)
        self.ts_cb = QCheckBox("Timestamp")
        self.ts_cb.setChecked(True)
        btn_row.addWidget(self.ts_cb)
        self.crlf_cb = QCheckBox("Append CRLF")
        self.crlf_cb.setChecked(False)
        btn_row.addWidget(self.crlf_cb)
        btn_row.addStretch()
        cg.addLayout(btn_row, 3, 0, 1, 2)

        cg.setColumnStretch(1, 1)
        layout.addWidget(ctrl_group)

        # ── Send bar ────────────────────────────────────────────
        send_row = QHBoxLayout()
        send_row.setSpacing(8)
        self.send_edit = QLineEdit()
        self.send_edit.setObjectName("send_input")
        self.send_edit.setPlaceholderText("Type message to send to device ...  (press Enter)")
        send_row.addWidget(self.send_edit, 1)
        self.send_btn = QPushButton("Send")
        self.send_btn.setObjectName("send_button")
        self.send_btn.setFixedWidth(80)
        self.send_btn.setEnabled(False)
        send_row.addWidget(self.send_btn)
        layout.addLayout(send_row)

        # ── Monitor log ─────────────────────────────────────────
        self.monitor_panel = LogPanel("Serial Monitor", "monitor_view")
        layout.addWidget(self.monitor_panel, 1)

        # ── Connections ─────────────────────────────────────────
        self.refresh_btn.clicked.connect(self._refresh_ports)
        self.connect_btn.clicked.connect(self._connect)
        self.disconnect_btn.clicked.connect(self._disconnect)
        self.send_btn.clicked.connect(self._send_data)
        self.send_edit.returnPressed.connect(self._send_data)
        self.monitor_panel.clear_btn.clicked.connect(self.monitor_panel.clear)
        self.data_signal.connect(self._on_data)

        self._refresh_ports()

        self._scan_timer = QTimer(self)
        self._scan_timer.timeout.connect(self._auto_scan)
        self._scan_timer.start(2000)

    def _refresh_ports(self) -> None:
        ports = get_serial_ports()
        _update_combo(self.port_combo, ports, self._known_ports)
        self._known_ports = set(ports)

    def _auto_scan(self) -> None:
        ports = get_serial_ports()
        if _update_combo(self.port_combo, ports, self._known_ports):
            self._known_ports = set(ports)

    def _connect(self) -> None:
        port = self.port_combo.currentText().strip()
        if not port or "(no ports" in port:
            QMessageBox.warning(self, "No Port", "Select a serial port.")
            return
        try:
            baud = int(self.baud_combo.currentText())
        except ValueError:
            QMessageBox.warning(self, "Invalid Baud", "Enter a valid baud rate.")
            return
        try:
            self._serial_port = serial.Serial(port, baud, timeout=0.1)
        except Exception as e:
            QMessageBox.critical(self, "Connection Failed", str(e))
            return

        self._running = True
        threading.Thread(target=self._read_loop, daemon=True).start()

        self.connect_btn.setEnabled(False)
        self.disconnect_btn.setEnabled(True)
        self.send_btn.setEnabled(True)
        self.port_combo.setEnabled(False)
        self.baud_combo.setEnabled(False)

        self.conn_status.setText(f"\u26AB  Connected: {port} @ {baud}")
        self.conn_status.setStyleSheet("color: #27ae60; font-size: 13px; font-weight: 600;")
        self._header.set_status(f"Monitor: {port}", "#27ae60")
        self._status_bar.showMessage(f"Connected to {port} @ {baud}")

    def _disconnect(self) -> None:
        self._running = False
        if self._serial_port and self._serial_port.is_open:
            self._serial_port.close()
        self._serial_port = None
        self.connect_btn.setEnabled(True)
        self.disconnect_btn.setEnabled(False)
        self.send_btn.setEnabled(False)
        self.port_combo.setEnabled(True)
        self.baud_combo.setEnabled(True)
        self.conn_status.setText("\u26AB  Disconnected")
        self.conn_status.setStyleSheet("color: #e74c3c; font-size: 13px; font-weight: 600;")
        self._header.set_status("Idle", "#e74c3c")
        self._status_bar.showMessage("Disconnected")

    def _read_loop(self) -> None:
        while self._running and self._serial_port and self._serial_port.is_open:
            try:
                data = self._serial_port.read(256)
                if data:
                    self.data_signal.emit(data.decode(errors="replace"))
            except Exception:
                if self._running:
                    self.data_signal.emit("[READ ERROR]\n")
                break

    def _send_data(self) -> None:
        if not self._serial_port or not self._serial_port.is_open:
            return
        text = self.send_edit.text()
        if not text:
            return
        try:
            self._serial_port.write(text.encode())
            if self.crlf_cb.isChecked():
                self._serial_port.write(b"\r\n")
            suffix = "\\r\\n" if self.crlf_cb.isChecked() else ""
            self.monitor_panel.append(f'<span style="color: #f39c12;">>> {text}{suffix}</span>')
            self.send_edit.clear()
        except Exception as e:
            self.monitor_panel.append(f'<span style="color: #e74c3c;">[SEND ERROR] {e}</span>')

    @pyqtSlot(str)
    def _on_data(self, text):
        import datetime
        lines = text.rstrip("\r\n").split("\n")
        if self.ts_cb.isChecked():
            ts = datetime.datetime.now().strftime("%H:%M:%S.%f")[:-3]
            for line in lines:
                if line.strip():
                    self.monitor_panel.append(
                        f'<span style="color: #7fb3d8;">[{ts}]</span> '
                        f'<span style="color: #a6e3a1;">{line.rstrip()}</span>'
                    )
        else:
            for line in lines:
                self.monitor_panel.append(f'<span style="color: #a6e3a1;">{line.rstrip()}</span>')


# ── Main window ─────────────────────────────────────────────────────
class FlasherWindow(QMainWindow):
    def __init__(self) -> None:
        super().__init__()
        self.setWindowTitle("MCU-Framework Flasher")
        self.setMinimumSize(860, 720)
        self.resize(920, 800)

        central = QWidget()
        self.setCentralWidget(central)
        root = QVBoxLayout(central)
        root.setSpacing(0)
        root.setContentsMargins(0, 0, 0, 0)

        self.header = HeaderBanner()
        root.addWidget(self.header)

        tabs = QTabWidget()
        tabs.setMinimumWidth(820)
        self.flasher_tab = FlasherTab(self.header, self.statusBar())
        self.monitor_tab = MonitorTab(self.header, self.statusBar())
        tabs.addTab(self.flasher_tab, "\u26A1  Flasher")
        tabs.addTab(self.monitor_tab, "\U0001F4E1  Monitor")
        root.addWidget(tabs, 1)

        self.header.set_status("Idle", "#e74c3c")
        self.statusBar().showMessage("Ready \u2014 connect board and select firmware")


def main() -> None:
    os.environ.setdefault("QT_AUTO_SCREEN_SCALE_FACTOR", "1")
    QApplication.setAttribute(Qt.AA_EnableHighDpiScaling, True)
    QApplication.setAttribute(Qt.AA_UseHighDpiPixmaps, True)

    app = QApplication(sys.argv)
    app.setStyleSheet(STYLESHEET)
    app.setApplicationName("MCU-Framework Flasher")
    app.setApplicationVersion("4.0.0")

    window = FlasherWindow()
    window.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
