#include "microfontcharwidget.h"
#include <qmath.h>
#include <QMouseEvent>
#include <QInputDialog>

MicroFontCharWidget::MicroFontCharWidget(QWidget *parent) : QWidget(parent)
{
	setSevenSegment();
}

MicroFontCharWidget::~MicroFontCharWidget()
{

}

void MicroFontCharWidget::paintEvent(QPaintEvent __attribute__((unused)) *event) {
	QPainter painter(this);
	scale = qMin(painter.window().width() / maxPoint.x(), painter.window().height() / maxPoint.y()) * 0.8;
	marginLeft = (painter.window().width() - maxPoint.x() * scale) / 2;
	marginTop = (painter.window().height() - maxPoint.y() * scale) / 2;;
	for (int i = 0; i < polygons.size(); i++) {
		QPolygonF realPolygon;
		foreach (QPointF point, polygons[i]) {
			realPolygon.append(QPointF(point.x() * scale + marginLeft, point.y() * scale + marginTop));
		}
		painter.setPen(QColor(Qt::black));
		if (polygonState[i]) {
			painter.setBrush(QBrush(QColor(Qt::green)));
		} else {
			painter.setBrush(QBrush(QColor(Qt::black)));
		}
		painter.drawPolygon(realPolygon);
		if (polygonPin[i] > -1) {
			if (!polygonState[i]) {
				painter.setPen(QPen(QColor(Qt::white)));
			}
			painter.drawText(realPolygon.boundingRect(), QString::number(polygonPin[i]), QTextOption(Qt::AlignCenter));
		}
	}
}

void MicroFontCharWidget::clear() {
	polygons.clear();
	polygonState.clear();
	polygonPin.clear();
	maxPoint.setX(0.0);
	maxPoint.setY(0.0);
}

void MicroFontCharWidget::addPolygon(const QPolygonF &polygon) {
	polygons.push_back(polygon);
	polygonState.push_back(false);
	polygonPin.push_back(-1);
	foreach (QPointF point, polygon) {
		if (point.x() > maxPoint.x()) {
			maxPoint.setX(point.x());
		}
		if (point.y() > maxPoint.y()) {
			maxPoint.setY(point.y());
		}
	}
}

void MicroFontCharWidget::addLed(qreal x, qreal y) {
	QPolygonF polygon;
	for (qreal i = M_PI / 8; i < 2 * M_PI; i += M_PI / 4) {
		polygon.append(QPointF(x + 0.5 * cos(i), y + 0.5 * sin(i)));
	}
	addPolygon(polygon);
}

void MicroFontCharWidget::addLedSegment(qreal x, qreal y, bool vertical) {
	QPolygonF polygon;
	for (qreal i = 0; i < 2 * M_PI; i += M_PI / 3) {
		if (vertical) {
			polygon.append(QPointF(x + 0.1 * sin(i), y + cos(i)));
		} else {
			polygon.append(QPointF(x + cos(i), y + 0.1 * sin(i)));
		}
	}
	addPolygon(polygon);
}

void MicroFontCharWidget::setSevenSegment() {
	clear();
	addLedSegment(0.0, 1.0, true);
	addLedSegment(0.0, 3.0, true);
	addLedSegment(1.0, 4.0, false);
	addLedSegment(2.0, 3.0, true);
	addLedSegment(2.0, 1.0, true);
	addLedSegment(1.0, 0.0, false);
	addLedSegment(1.0, 2.0, false);
	addLed(3.0, 4.0);
	update();
}

void MicroFontCharWidget::setLedMatrix(int width, int height) {
	clear();
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			addLed(j, i);
		}
	}
	update();
}

void MicroFontCharWidget::mousePressEvent(QMouseEvent *event) {
	qreal x = (event->x() - marginLeft) / scale;
	qreal y = (event->y() - marginTop) / scale;
	for (int i = 0; i < polygons.size(); i++) {
		if (polygons[i].containsPoint(QPointF(x, y), Qt::OddEvenFill)) {
			if (event->button() == Qt::LeftButton) {
				polygonState[i] = !polygonState[i];
			} else if (event->button() == Qt::RightButton) {
				bool ok = false;
				int newPin = QInputDialog::getInt(NULL, "MicroFontEditor", "Enter pin index:", polygonPin[i],
								  -1, 256, 1, &ok);
				if (ok) {
					polygonPin[i] = newPin;
				}
			} else {
				break;
			}
			update();
			emit charChanged();
			break;
		}
	}
}

QString MicroFontCharWidget::generateCode() {
	return generateCode(getChar());
}

QString MicroFontCharWidget::generateCode(QString data) {
	QString code = "";
	int byte = 0, offset = 0;
	for (int i = 0; i < qMin(data.size(), polygons.size()); i++) {
		if ((data[i] != '0') && (polygonPin[i] > -1)) {
			byte |= 1 << (polygonPin[i] - offset);
		}
		if (((i - offset) == 7) || (i == (data.size() - 1))) {
			if (code.size() != 0) {
				code += ", ";
			}
			code += "0x" + QString::number(byte, 16);
			offset = i + 1;
			byte = 0;
		}

	}
	return code;
}

void MicroFontCharWidget::setChar(QString data) {
	if (data.size() > 0) {
		for (int i = 0; i < qMin(data.size(), polygons.size()); i++) {
			if (data[i] == '0') {
				polygonState[i] = false;
			} else {
				polygonState[i] = true;
			}
		}
	} else {
		for (int i = 0; i < polygons.size(); i++) {
			polygonState[i] = false;
		}
	}
	update();
	emit charChanged();
}

QString MicroFontCharWidget::getChar() {
	QString tmp = "";
	for (int i = 0; i < polygons.size(); i++) {
		if (polygonState[i]) {
			tmp += "1";
		} else {
			tmp += "0";
		}
	}
	return tmp;
}

QString MicroFontCharWidget::pinLayout() {
	QString tmp = "";
	for (int i = 0; i < polygonPin.size(); i++) {
		if (i != 0) {
			tmp += ", ";
		}
		tmp += QString::number(polygonPin[i]);
	}
	return tmp;
}

void MicroFontCharWidget::setPinLayout(QString layout) {
	QStringList l = layout.split(", ");
	for (int i = 0; i < l.size(); i++) {
		polygonPin[i] = l[i].toInt();
	}
	update();
	emit charChanged();
}

void MicroFontCharWidget::autoPinLayout() {
	for (int i = 0; i < polygonPin.size(); i++) {
		polygonPin[i] = i;
	}
	update();
	emit charChanged();
}

void MicroFontCharWidget::clearPinLayout() {
	for (int i = 0; i < polygonPin.size(); i++) {
		polygonPin[i] = -1;
	}
	update();
	emit charChanged();
}

QString MicroFontCharWidget::polygonData() {
	QString tmp = "";
	for (int i = 0; i < polygons.size(); i++) {
		if (i != 0) {
			tmp += "; ";
		}
		for (int j = 0; j < polygons[i].size(); j++) {
			if (j != 0) {
				tmp += ", ";
			}
			tmp += QString::number(polygons[i][j].x());
			tmp += ":";
			tmp += QString::number(polygons[i][j].y());
		}
	}
	return tmp;
}

void MicroFontCharWidget::setPolygonData(QString data) {
	clear();
	QStringList l1 = data.split("; ");
	foreach (QString s1, l1) {
		QStringList l2 = s1.split(", ");
		QPolygonF polygon;
		foreach (QString s2, l2) {
			QStringList l3 = s2.split(":");
			polygon.append(QPointF(l3[0].toDouble(), l3[1].toDouble()));
		}
		addPolygon(polygon);
	}
	update();
}
