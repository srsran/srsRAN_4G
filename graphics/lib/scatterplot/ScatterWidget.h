#ifndef SCATTERWIDGET_H
#define SCATTERWIDGET_H

#include <qapplication.h>
#include <qwidget.h>
#include <complex>

class ComplexDataEvent;
class Pointplot;

class ScatterWidget
  : public QWidget
{
  Q_OBJECT

public:
  ScatterWidget(QWidget* parent = 0);
  virtual ~ScatterWidget();

public slots:
  void customEvent( QEvent * e );
  void setWidgetTitle(QString title);
  void setWidgetAxisLabels(QString xLabel, QString yLabel);
  void setWidgetXAxisScale(double xMin, double xMax);
  void setWidgetYAxisScale(double yMin, double yMax);
  void setWidgetXAxisAutoScale(bool on);
  void setWidgetYAxisAutoScale(bool on);

protected:
  virtual void timerEvent(QTimerEvent *event);

private:
  void setData(ComplexDataEvent* e);
  Pointplot* plot_;

  struct opReal{double operator()(std::complex<double> i) const{return real(i);}};
  struct opImag{double operator()(std::complex<double> i) const{return imag(i);}};

  double* iData_;
  double* qData_;
  int numPoints_;
  int timerId_;
  bool haveNewData_;
};

#endif // SCATTERWIDGET_H
