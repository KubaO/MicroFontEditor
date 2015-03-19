#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	chars.push_back("");
	dirty = false;
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_fontViewer_charChanged() {
	QString code = ui->fontViewer->generateCode();
	ui->codeLineEdit->setText(code);
	chars[ui->charSpinBox->value()] = ui->fontViewer->getChar();
	QString tmp = "";
	for (int i = 0; i < chars.size(); i++) {
		if (i != 0) {
			tmp += ",\n";
		}
		tmp += ui->fontViewer->generateCode(chars[i]);
	}
	ui->fullCodeTextEdit->setPlainText(tmp);
	dirty = true;
}

void MainWindow::on_charSpinBox_valueChanged(int value)
{
	ui->fontViewer->setChar(chars[value]);
}

void MainWindow::on_addButton_clicked()
{
	chars.insert(chars.begin() + ui->charSpinBox->value() + 1, "");
	ui->charSpinBox->setRange(0, chars.size() - 1);
	ui->charSpinBox->stepUp();
	ui->removeButton->setEnabled(true);
	on_charSpinBox_valueChanged(ui->charSpinBox->value());
	dirty = true;
}

void MainWindow::on_removeButton_clicked()
{
	chars.erase(chars.begin() + ui->charSpinBox->value());
	ui->charSpinBox->setRange(0, chars.size() - 1);
	on_charSpinBox_valueChanged(ui->charSpinBox->value());
	if (chars.size() == 1) {
		ui->removeButton->setEnabled(false);
	}
	dirty = true;
}

bool MainWindow::checkDirty() {
	if (dirty) {
		int r = QMessageBox::question(this, "MicroFontEditor", "Font modified. Save?", QMessageBox::Yes, QMessageBox::No);
		if (r == QMessageBox::Yes) {
			on_actionSave_triggered();
		} else {
			dirty = false;
		}
	}
	return !dirty;
}


void MainWindow::on_actionSave_triggered()
{
	QString fileName = QFileDialog::getSaveFileName(this, "MicroFontEditor", QString(),
							"MicroFont (*.mf);;All files (*.*)", 0, 0);
	if (fileName.size() > 0) {
		QFile file(fileName);
		if (file.open(QIODevice::WriteOnly)) {
			file.write(QString(ui->fontViewer->polygonData() + "\n").toUtf8());
			file.write(QString(ui->fontViewer->pinLayout() + "\n").toUtf8());
			for (int i = 0; i < chars.size(); i++) {
				file.write(QString(chars[i] + "\n").toUtf8());
			}
			file.close();
			dirty = false;
		} else {
			QMessageBox::warning(this, "MicroFontEditor", "Failed to create file", QMessageBox::Ok, QMessageBox::Ok);
		}

	}
}

void MainWindow::on_actionOpen_triggered()
{
	if (!checkDirty()) return;
	QString fileName = QFileDialog::getOpenFileName(this, "MicroFontEditor", QString(),
							"MicroFont (*.mf);;All files (*.*)", 0, 0);
	if (fileName.size() > 0) {
		QFile file(fileName);
		if (file.open(QIODevice::ReadOnly)) {
			QString polygonData = file.readLine();
			QString pinLayout = file.readLine();
			QString code = file.readAll();
			file.close();
			ui->fontViewer->setPolygonData(polygonData);
			ui->fontViewer->setPinLayout(pinLayout);
			chars = code.split("\n");
			chars.removeLast();
			ui->charSpinBox->setRange(0, chars.size() - 1);
			ui->charSpinBox->setValue(0);
			on_charSpinBox_valueChanged(0);
			if (chars.size() <= 1) {
				ui->removeButton->setEnabled(false);
			} else {
				ui->removeButton->setEnabled(true);
			}
		} else {
			QMessageBox::warning(this, "MicroFontEditor", "Failed to open file", QMessageBox::Ok, QMessageBox::Ok);
		}
	}
}

void MainWindow::on_actionAutomatic_pin_layout_triggered()
{
	ui->fontViewer->autoPinLayout();
}

void MainWindow::on_actionClear_pin_layout_triggered()
{
	ui->fontViewer->clearPinLayout();
}

void MainWindow::on_actionSeven_segment_triggered()
{
	if (!checkDirty()) return;
	chars.clear();
	chars.append("");
	ui->fontViewer->setSevenSegment();
	ui->charSpinBox->setValue(0);
	ui->charSpinBox->setRange(0, 0);
	ui->removeButton->setEnabled(false);
	on_charSpinBox_valueChanged(0);
}

void MainWindow::on_actionLed_matrix_triggered()
{
	if (!checkDirty()) return;
	bool ok;
	int width = QInputDialog::getInt(this, "MicroFontEditor", "Enter matrix width:", 8, 1, 16, 1, &ok);
	if (!ok) return;
	int height = QInputDialog::getInt(this, "MicroFontEditor", "Enter matrix height:", 8, 1, 16, 1, &ok);
	if (!ok) return;
	chars.clear();
	chars.append("");
	ui->fontViewer->setLedMatrix(width, height);
	ui->charSpinBox->setValue(0);
	ui->charSpinBox->setRange(0, 0);
	ui->removeButton->setEnabled(false);
	on_charSpinBox_valueChanged(0);
}
