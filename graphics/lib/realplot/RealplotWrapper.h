#ifndef REALPLOTWRAPPER_H
#define REALPLOTWRAPPER_H

#include <qapplication.h>

class RealWidget;

class RealplotWrapper
    : QObject
{
  Q_OBJECT

public:
  RealplotWrapper();
  ~RealplotWrapper();

  void setNewData(float* data, int numPoints);
  void setNewData(double* data, int numPoints);
  void setTitle(std::string title);
  void setAxisLabels(std::string xLabel, std::string yLabel);
  void setXAxisScale(double xMin, double xMax);
  void setYAxisScale(double yMin, double yMax);
  void setXAxisAutoScale(bool on);
  void setYAxisAutoScale(bool on);
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
  void setWidgetAxisLabels(QString xLabel, QString yLabel);
  void setWidgetXAxisScale(double xMin, double xMax);
  void setWidgetYAxisScale(double yMin, double yMax);
  void setWidgetXAxisAutoScale(bool on);
  void setWidgetYAxisAutoScale(bool on);
  void setWidgetXAxisRange(double xMin, double xMax);

private:
  RealWidget* widget_;
  bool destroyed_;
};

#endif // REALPLOTWRAPPER_H
