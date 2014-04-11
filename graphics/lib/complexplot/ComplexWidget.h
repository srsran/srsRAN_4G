#ifndef COMPLEXWIDGET_H
#define COMPLEXWIDGET_H

#include <qapplication.h>
#include <qwidget.h>
#include <complex>

class ComplexDataEvent;
class Lineplot;

class ComplexWidget
  : public QWidget
{
  Q_OBJECT

public:
  ComplexWidget(QWidget* parent = 0);
  virtual ~ComplexWidget();

public slots:
  void customEvent( QEvent * e );
  void setWidgetTitle(QString title);
  void setWidgetXAxisScale(int id, double xMin, double xMax);
  void setWidgetYAxisScale(int id, double yMin, double yMax);
  void setWidgetXAxisAutoScale(int id, bool on);
  void setWidgetYAxisAutoScale(int id, bool on);
  void setWidgetXAxisRange(double xMin, double xMax);

protected:
  virtual void timerEvent(QTimerEvent *event);

private:
  void setData(ComplexDataEvent* e);
  Lineplot* i_; //In-phase plot
  Lineplot* q_; //Quadrature plot
  Lineplot* m_; //Magnitude plot
  Lineplot* p_; //Phase plot

  struct opReal{double operator()(std::complex<double> i) const{return real(i);}};
  struct opImag{double operator()(std::complex<double> i) const{return imag(i);}};
  struct opAbs{double operator()(std::complex<double> i) const{return abs(i);}};
  struct opArg{double operator()(std::complex<double> i) const{return arg(i);}};

  double* iData_;
  double* qData_;
  double* mData_;
  double* pData_;
  int numPoints_;
  int timerId_;
  bool haveNewData_;
};

#endif // COMPLEXWIDGET_H
