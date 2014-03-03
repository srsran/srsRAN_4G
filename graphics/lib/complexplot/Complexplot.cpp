#include "Complexplot.h"
#include "ComplexplotWrapper.h"

using namespace std;

Complexplot::Complexplot()
{
  plot_ = new ComplexplotWrapper;
}

Complexplot::~Complexplot()
{
  delete plot_;
}

void Complexplot::setNewData(complex<double>* data, int numPoints)
{
  plot_->setNewData(data, numPoints);
}

void Complexplot::setNewData(complex<float>* data, int numPoints)
{
  plot_->setNewData(data, numPoints);
}

void Complexplot::setTitle(std::string title)
{
  plot_->setTitle(title);
}

void Complexplot::setXAxisAutoScale(PlotId id, bool on=true)
{
  plot_->setXAxisAutoScale(id, on);
}

void Complexplot::setYAxisAutoScale(PlotId id, bool on=true)
{
  plot_->setYAxisAutoScale(id, on);
}

void Complexplot::setXAxisScale(PlotId id, double xMin, double xMax)
{
  plot_->setXAxisScale(id, xMin, xMax);
}

void Complexplot::setYAxisScale(PlotId id, double yMin, double yMax)
{
  plot_->setYAxisScale(id, yMin, yMax);
}

void Complexplot::setXAxisRange(double xMin, double xMax)
{
  plot_->setXAxisRange(xMin, xMax);
}

