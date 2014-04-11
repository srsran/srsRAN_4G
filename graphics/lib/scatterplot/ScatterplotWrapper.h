#ifndef SCATTERPLOTWRAPPER_H
#define SCATTERPLOTWRAPPER_H

#include <qapplication.h>
#include <complex>

class ScatterWidget;

class ScatterplotWrapper
    : QObject
{
  Q_OBJECT

public:
  ScatterplotWrapper();
  ~ScatterplotWrapper();

  void setNewData(std::complex<float>* data, int numPoints);
  void setNewData(std::complex<double>* data, int numPoints);
  void setTitle(std::string title);
  void setXAxisScale(double xMin, double xMax);
  void setYAxisScale(double yMin, double yMax);
  void setXAxisAutoScale(bool on);
  void setYAxisAutoScale(bool on);
  void setAxisLabels(std::string xLabel, std::string yLabel);

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

private:
  ScatterWidget* widget_;
  bool destroyed_;
};

#endif // SCATTERPLOTWRAPPER_H
