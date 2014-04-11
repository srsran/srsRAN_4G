#ifndef REALWIDGET_H
#define REALWIDGET_H

#include <qapplication.h>
#include <qwidget.h>
#include <complex>

class RealDataEvent;
class Lineplot;

class RealWidget
  : public QWidget
{
  Q_OBJECT

public:
  RealWidget(QWidget* parent = 0);
  virtual ~RealWidget();

public slots:
  void customEvent( QEvent * e );
  void setWidgetTitle(QString title);
  void setWidgetAxisLabels(QString xLabel, QString yLabel);
  void setWidgetXAxisScale(double xMin, double xMax);
  void setWidgetYAxisScale(double yMin, double yMax);
  void setWidgetXAxisAutoScale(bool on);
  void setWidgetYAxisAutoScale(bool on);
  void setWidgetXAxisRange(double xMin, double xMax);

protected:
  virtual void timerEvent(QTimerEvent *event);

private:
  void setData(RealDataEvent* e);
  Lineplot* l_; //The line plot

  double* dataPoints_;
  int numPoints_;
  int timerId_;
  bool haveNewData_;
};

#endif // REALWIDGET_H
