#ifndef MICROFONTCHARWIDGET_H
#define MICROFONTCHARWIDGET_H

#include <QWidget>
#include <QPainter>

class MicroFontCharWidget : public QWidget
{
		Q_OBJECT
	public:
		explicit MicroFontCharWidget(QWidget *parent = 0);
		~MicroFontCharWidget();
		QString generateCode();
		void setChar(QString data);
		QString pinLayout();
		void setPinLayout(QString layout);
		void autoPinLayout();
		void clearPinLayout();
		void setSevenSegment();
		void setLedMatrix(int width, int height);
		QString polygonData();
		void setPolygonData(QString data);

	signals:
		void charChanged();

	public slots:

	protected:
		void paintEvent(QPaintEvent *event);
		void mousePressEvent(QMouseEvent *event);

	private:
		QVector<QPolygonF> polygons;
		QVector<bool> polygonState;
		QVector<int> polygonPin;
		QPointF maxPoint;
		qreal scale;
		qreal marginLeft;
		qreal marginTop;
		void clear();
		void addPolygon(const QPolygonF &polygon);
		void addLed(qreal x, qreal y);
		void addLedSegment(qreal x, qreal y, bool vertical);

};

#endif // MICROFONTCHARWIDGET_H
