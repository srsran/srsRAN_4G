#include "ScatterWidget.h"
#include "Pointplot.h"
#include "Events.h"

#include <qlayout.h>
#include <algorithm>

using namespace std;

ScatterWidget::ScatterWidget(QWidget *parent)
  :QWidget(parent)
{
  plot_ = new Pointplot();
  QVBoxLayout* vLayout1 = new QVBoxLayout(this);
  vLayout1->addWidget(plot_);

  numPoints_ = 16;
  iData_ = new double[numPoints_];
  qData_ = new double[numPoints_];
  timerId_ = startTimer(10);
  haveNewData_ = false;
}

ScatterWidget::~ScatterWidget()
{
  delete iData_;
  delete qData_;
}

void ScatterWidget::customEvent( QEvent * e )
{
  if(e->type() == ComplexDataEvent::type)
  {
    ComplexDataEvent* dataEvent = (ComplexDataEvent*)e;
    setData(dataEvent);
  }
}

void ScatterWidget::timerEvent(QTimerEvent *event)
{
  if(event->timerId() == timerId_)
  {
    if(haveNewData_)
    {
      plot_->replot();
      haveNewData_ = false;
    }
    return;
  }
  QWidget::timerEvent(event);
}

void ScatterWidget::setData(ComplexDataEvent* e)
{
  if(e->numPoints_ != numPoints_)
  {
    numPoints_ = e->numPoints_;
    delete [] iData_;
    delete [] qData_;
    iData_ = new double[numPoints_];
    qData_ = new double[numPoints_];
  }

  transform(e->dataPoints_, &e->dataPoints_[numPoints_], iData_, opReal());
  transform(e->dataPoints_, &e->dataPoints_[numPoints_], qData_, opImag());

  plot_->setData(iData_, qData_, numPoints_);
  haveNewData_ = true;
}

void ScatterWidget::setWidgetTitle(QString title)
{
  plot_->setTitle(title);
}

void ScatterWidget::setWidgetAxisLabels(QString xLabel, QString yLabel)
{
  plot_->setAxisTitle(QwtPlot::xBottom, xLabel);
  plot_->setAxisTitle(QwtPlot::yLeft, yLabel);
}

void ScatterWidget::setWidgetXAxisScale(double xMin, double xMax)
{
  plot_->setAxisScale(QwtPlot::xBottom, xMin, xMax);
}

void ScatterWidget::setWidgetYAxisScale(double yMin, double yMax)
{
  plot_->setAxisScale(QwtPlot::yLeft, yMin, yMax);
}

void ScatterWidget::setWidgetXAxisAutoScale(bool on=true)
{
  plot_->setAxisAutoScale(QwtPlot::xBottom, on);
}

void ScatterWidget::setWidgetYAxisAutoScale(bool on)
{
  plot_->setAxisAutoScale(QwtPlot::yLeft, on);
}
