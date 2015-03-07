#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

	private slots:
		void on_fontViewer_charChanged();

		void on_charSpinBox_valueChanged(int value);

		void on_addButton_clicked();

		void on_removeButton_clicked();

		void on_actionSave_triggered();

		void on_actionOpen_triggered();

		void on_actionAutomatic_pin_layout_triggered();

		void on_actionClear_pin_layout_triggered();

		void on_actionSeven_segment_triggered();

		void on_actionLed_matrix_triggered();

	private:
		Ui::MainWindow *ui;
		QStringList chars;
};

#endif // MAINWINDOW_H
