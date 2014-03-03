#include "ComplexWidget.h"
#include "Lineplot.h"
#include "Events.h"

#include <qlayout.h>
#include <algorithm>
#include <boost/lambda/lambda.hpp>

using namespace std;
namespace bl = boost::lambda;

ComplexWidget::ComplexWidget(QWidget *parent)
  :QWidget(parent)
{
  i_ = new Lineplot();
  q_ = new Lineplot();
  m_ = new Lineplot();
  p_ = new Lineplot();

  i_->setAxisTitle(QwtPlot::xBottom, "In-phase");
  q_->setAxisTitle(QwtPlot::xBottom, "Quadrature");
  m_->setAxisTitle(QwtPlot::xBottom, "Magnitude");
  p_->setAxisTitle(QwtPlot::xBottom, "Phase");

  QVBoxLayout* vLayout1 = new QVBoxLayout(this);
  vLayout1->addWidget(i_);
  vLayout1->addWidget(q_);
  vLayout1->addWidget(m_);
  vLayout1->addWidget(p_);

  numPoints_ = 16;
  iData_ = new double[numPoints_];
  qData_ = new double[numPoints_];
  mData_ = new double[numPoints_];
  pData_ = new double[numPoints_];
  timerId_ = startTimer(10);
  haveNewData_ = false;
}

ComplexWidget::~ComplexWidget()
{
  delete i_;
  delete q_;
  delete m_;
  delete p_;
}

void ComplexWidget::customEvent( QEvent * e )
{
  if(e->type() == ComplexDataEvent::type)
  {
    ComplexDataEvent* dataEvent = (ComplexDataEvent*)e;
    setData(dataEvent);
  }
}

void ComplexWidget::timerEvent(QTimerEvent *event)
{
  if(event->timerId() == timerId_)
  {
    if(haveNewData_)
    {
      i_->replot();
      q_->replot();
      m_->replot();
      p_->replot();
      haveNewData_ = false;
    }
    return;
  }
  QWidget::timerEvent(event);
}

void ComplexWidget::setData(ComplexDataEvent* e)
{
  if(e->numPoints_ != numPoints_)
  {
    numPoints_ = e->numPoints_;
    delete [] iData_;
    delete [] qData_;
    delete [] mData_;
    delete [] pData_;

    iData_ = new double[numPoints_];
    qData_ = new double[numPoints_];
    mData_ = new double[numPoints_];
    pData_ = new double[numPoints_];
  }

  transform(e->dataPoints_, &e->dataPoints_[numPoints_], iData_, opReal());
  transform(e->dataPoints_, &e->dataPoints_[numPoints_], qData_, opImag());
  transform(e->dataPoints_, &e->dataPoints_[numPoints_], mData_, opAbs());
  transform(e->dataPoints_, &e->dataPoints_[numPoints_], pData_, opArg());

  i_->setData(iData_, numPoints_);
  q_->setData(qData_, numPoints_);
  m_->setData(mData_, numPoints_);
  p_->setData(pData_, numPoints_);
  haveNewData_ = true;
}

void ComplexWidget::setWidgetTitle(QString title)
{
  setWindowTitle(title);
}

void ComplexWidget::setWidgetXAxisScale(int id, double xMin, double xMax)
{
  switch(id)
  {
  case 0:
    i_->setAxisScale(QwtPlot::xBottom, xMin, xMax);
    break;
  case 1:
    q_->setAxisScale(QwtPlot::xBottom, xMin, xMax);
    break;
  case 2:
    m_->setAxisScale(QwtPlot::xBottom, xMin, xMax);
    break;
  case 3:
    p_->setAxisScale(QwtPlot::xBottom, xMin, xMax);
    break;
  default:
    break;
  }
}

void ComplexWidget::setWidgetYAxisScale(int id, double yMin, double yMax)
{
  switch(id)
  {
  case 0:
    i_->setAxisScale(QwtPlot::yLeft, yMin, yMax);
    break;
  case 1:
    q_->setAxisScale(QwtPlot::yLeft, yMin, yMax);
    break;
  case 2:
    m_->setAxisScale(QwtPlot::yLeft, yMin, yMax);
    break;
  case 3:
    p_->setAxisScale(QwtPlot::yLeft, yMin, yMax);
    break;
  default:
    break;
  }
}

void ComplexWidget::setWidgetXAxisAutoScale(int id, bool on=true)
{
  switch(id)
  {
  case 0:
    i_->setAxisAutoScale(QwtPlot::xBottom, on);
    break;
  case 1:
    q_->setAxisAutoScale(QwtPlot::xBottom, on);
    break;
  case 2:
    m_->setAxisAutoScale(QwtPlot::xBottom, on);
    break;
  case 3:
    p_->setAxisAutoScale(QwtPlot::xBottom, on);
    break;
  default:
    break;
  }
}

void ComplexWidget::setWidgetYAxisAutoScale(int id, bool on=true)
{
  switch(id)
  {
  case 0:
    i_->setAxisAutoScale(QwtPlot::yLeft, on);
    break;
  case 1:
    q_->setAxisAutoScale(QwtPlot::yLeft, on);
    break;
  case 2:
    m_->setAxisAutoScale(QwtPlot::yLeft, on);
    break;
  case 3:
    p_->setAxisAutoScale(QwtPlot::yLeft, on);
    break;
  default:
    break;
  }
}

void ComplexWidget::setWidgetXAxisRange(double xMin, double xMax)
{
  i_->setXAxisRange(xMin, xMax);
  q_->setXAxisRange(xMin, xMax);
  m_->setXAxisRange(xMin, xMax);
  p_->setXAxisRange(xMin, xMax);
}
