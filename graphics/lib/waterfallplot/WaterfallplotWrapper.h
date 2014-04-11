#ifndef WATERFALLPLOTWRAPPER_H
#define WATERFALLPLOTWRAPPER_H

#include <qapplication.h>

class WaterfallWidget;

class WaterfallplotWrapper
    : QObject
{
  Q_OBJECT

public:
  WaterfallplotWrapper(int numDataPoints, int numRows);
  ~WaterfallplotWrapper();

  void appendNewData(float* data, int numPoints);
  void appendNewData(double* data, int numPoints);
  void setTitle(std::string title);
  void setPlotXLabel(std::string xLabel);
  void setPlotYLabel(std::string yLabel);
  void setPlotXAxisRange(double xMin, double xMax);
  void setPlotXAxisScale(double xMin, double xMax);
  void setPlotYAxisScale(double yMin, double yMax);
  void setSpectrogramXLabel(std::string xLabel);
  void setSpectrogramYLabel(std::string yLabel);
  void setSpectrogramXAxisRange(double xMin, double xMax);
  void setSpectrogramYAxisRange(double yMin, double yMax);
  void setSpectrogramZAxisScale(double zMin, double zMax);

public slots:
  void createWidgetSlot(int numDataPoints, int numRows);
  void destroyWidgetSlot();
  void widgetDestroyed();

signals:
  void createWidgetSignal(int numDataPoints, int numRows);
  void destroyWidgetSignal();
  void destroyWidgetSignalBlocking();
  void setWidgetTitle(QString title);
  void setWidgetPXLabel(QString xLabel);
  void setWidgetPYLabel(QString yLabel);
  void setWidgetPXAxisRange(double xMin, double xMax);
  void setWidgetPXAxisScale(double xMin, double xMax);
  void setWidgetPYAxisScale(double yMin, double yMax);
  void setWidgetSXLabel(QString xLabel);
  void setWidgetSYLabel(QString yLabel);
  void setWidgetSXAxisRange(double xMin, double xMax);
  void setWidgetSYAxisRange(double yMin, double yMax);
  void setWidgetSZAxisScale(double zMin, double zMax);

private:
  WaterfallWidget* widget_;
  bool destroyed_;
};

#endif // WATERFALLPLOTWRAPPER_H
