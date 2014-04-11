#ifndef COMPLEXPLOTWRAPPER_H
#define COMPLEXPLOTWRAPPER_H

#include <qapplication.h>
#include <complex>

class ComplexWidget;

class ComplexplotWrapper
    : QObject
{
  Q_OBJECT

public:
  ComplexplotWrapper();
  ~ComplexplotWrapper();

  void setNewData(std::complex<float>* data, int numPoints);
  void setNewData(std::complex<double>* data, int numPoints);
  void setTitle(std::string title);
  void setXAxisAutoScale(int id, bool on);
  void setYAxisAutoScale(int id, bool on);
  void setXAxisScale(int id, double xMin, double xMax);
  void setYAxisScale(int id, double yMin, double yMax);
  void setXAxisRange(double xMin, double xMax);

public slots:
  void createWidgetSlot();
  void destroyWidgetSlot();
  void widgetDestroyed();

signals:
  void createWidgetSignal();
  void destroyWidgetSignal();
  void destroyWidgetSignalBlocking();
  void setWidgetTitle(QString title);
  void setWidgetXAxisAutoScale(int id, bool on);
  void setWidgetYAxisAutoScale(int id, bool on);
  void setWidgetXAxisScale(int id, double xMin, double xMax);
  void setWidgetYAxisScale(int id, double yMin, double yMax);
  void setWidgetXAxisRange(double xMin, double xMax);

private:
  ComplexWidget* widget_;
  bool destroyed_;
};

#endif // COMPLEXPLOTWRAPPER_H
