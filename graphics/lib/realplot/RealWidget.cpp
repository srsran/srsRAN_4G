#include "RealWidget.h"
#include "Lineplot.h"
#include "Events.h"

#include <qlayout.h>
#include <algorithm>

using namespace std;

RealWidget::RealWidget(QWidget *parent)
  :QWidget(parent)
{
  l_ = new Lineplot();
  QVBoxLayout* vLayout1 = new QVBoxLayout(this);
  vLayout1->addWidget(l_);

  numPoints_ = 16;
  dataPoints_ = new double[numPoints_];
  timerId_ = startTimer(10);
  haveNewData_ = false;
}

RealWidget::~RealWidget()
{
  delete l_;
}

void RealWidget::customEvent( QEvent * e )
{
  if(e->type() == RealDataEvent::type)
  {
    RealDataEvent* dataEvent = (RealDataEvent*)e;
    setData(dataEvent);
  }
}

void RealWidget::timerEvent(QTimerEvent *event)
{
  if(event->timerId() == timerId_)
  {
    if(haveNewData_)
    {
      l_->replot();
      haveNewData_ = false;
    }
    return;
  }
  QWidget::timerEvent(event);
}

void RealWidget::setData(RealDataEvent* e)
{
  if(e->numPoints_ != numPoints_)
  {
    numPoints_ = e->numPoints_;
    delete [] dataPoints_;
    dataPoints_ = new double[numPoints_];
  }
  for(int i=0;i<numPoints_;i++)
    dataPoints_[i] = e->dataPoints_[i];

  l_->setData(dataPoints_, numPoints_);
  haveNewData_ = true;
}

void RealWidget::setWidgetTitle(QString title)
{
  l_->setTitle(title);
}

void RealWidget::setWidgetAxisLabels(QString xLabel, QString yLabel)
{
  l_->setAxisTitle(QwtPlot::xBottom, xLabel);
  l_->setAxisTitle(QwtPlot::yLeft, yLabel);
}

void RealWidget::setWidgetXAxisScale(double xMin, double xMax)
{
  l_->setAxisScale(QwtPlot::xBottom, xMin, xMax);
}

void RealWidget::setWidgetYAxisScale(double yMin, double yMax)
{
  l_->setAxisScale(QwtPlot::yLeft, yMin, yMax);
}

void RealWidget::setWidgetXAxisAutoScale(bool on=true)
{
  l_->setAxisAutoScale(QwtPlot::xBottom, on);
}

void RealWidget::setWidgetYAxisAutoScale(bool on=true)
{
  l_->setAxisAutoScale(QwtPlot::yLeft, on);
}

void RealWidget::setWidgetXAxisRange(double xMin, double xMax)
{
  l_->setXAxisRange(xMin, xMax);
}
