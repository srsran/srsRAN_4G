#include "WaterfallWidget.h"
#include "Spectrogramplot.h"
#include "Lineplot.h"
#include "Events.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <algorithm>

using namespace std;


WaterfallWidget::WaterfallWidget(int numDataPoints, int numRows, QWidget *parent)
  :QWidget(parent)
{
  p_ = new Lineplot();
  s_ = new Spectrogramplot(numDataPoints, numRows);
  b_ = new QPushButton("Autoscale");

  connect(b_, SIGNAL(clicked()), this, SLOT(autoscale()));

  QVBoxLayout* vLayout1 = new QVBoxLayout(this);
  vLayout1->addWidget(p_);vLayout1->setStretch(0,1);
  vLayout1->addWidget(s_);vLayout1->setStretch(1,3);
  vLayout1->addWidget(b_);

  numPoints_ = numDataPoints;
  data_ = new double[numPoints_];
  timerId_ = startTimer(10);
  haveNewData_ = false;
}

WaterfallWidget::~WaterfallWidget()
{
  delete p_;
  delete s_;
}

void WaterfallWidget::customEvent( QEvent * e )
{
  if(e->type() == RealDataEvent::type)
  {
    RealDataEvent* dataEvent = (RealDataEvent*)e;
    appendData(dataEvent);
  }
}

void WaterfallWidget::setWidgetTitle(QString title)
{
  setWindowTitle(title);
}

void WaterfallWidget::setPlotXLabel(QString xLabel)
{
  p_->setAxisTitle(QwtPlot::xBottom, xLabel);
}

void WaterfallWidget::setPlotYLabel(QString yLabel)
{
  p_->setAxisTitle(QwtPlot::yLeft, yLabel);
}

void WaterfallWidget::setPlotXAxisRange(double xMin, double xMax)
{
  p_->setXAxisRange(xMin, xMax);
}

void WaterfallWidget::setPlotXAxisScale(double xMin, double xMax)
{
  p_->setAxisScale(QwtPlot::xBottom, xMin, xMax);
}

void WaterfallWidget::setPlotYAxisScale(double yMin, double yMax)
{
  p_->setAxisScale(QwtPlot::yLeft, yMin, yMax);
}

void WaterfallWidget::setSpectrogramXLabel(QString xLabel)
{
  s_->setAxisTitle(QwtPlot::xBottom, xLabel);
}

void WaterfallWidget::setSpectrogramYLabel(QString yLabel)
{
  s_->setAxisTitle(QwtPlot::yLeft, yLabel);
}

void WaterfallWidget::setSpectrogramXAxisRange(double xMin, double xMax)
{
  s_->setXAxisRange(xMin, xMax);
}

void WaterfallWidget::setSpectrogramYAxisRange(double yMin, double yMax)
{
  s_->setYAxisRange(yMin, yMax);
}

void WaterfallWidget::setSpectrogramZAxisScale(double zMin, double zMax)
{
  s_->setZAxisScale(zMin, zMax);
}

void WaterfallWidget::autoscale()
{
  double min = s_->min();
  double max = s_->max();
  s_->setZAxisScale(min, max);
  p_->setAxisAutoScale(QwtPlot::yLeft, false);
  p_->setAxisScale(QwtPlot::yLeft, min, max);
}

void WaterfallWidget::timerEvent(QTimerEvent *event)
{
  if(event->timerId() == timerId_)
  {
    if(haveNewData_)
    {
      p_->replot();
      s_->replot();
      haveNewData_ = false;
    }
    return;
  }
  QWidget::timerEvent(event);
}

void WaterfallWidget::appendData(RealDataEvent* e)
{
  if(e->numPoints_ != numPoints_)
  {
    numPoints_ = e->numPoints_;
    delete [] data_;
    data_ = new double[numPoints_];
  }

  memcpy(data_, e->dataPoints_, numPoints_*sizeof(double));

  p_->setData(data_, numPoints_);
  s_->appendData(data_, numPoints_);
  haveNewData_ = true;
}
