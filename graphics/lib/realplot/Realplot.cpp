#include "Realplot.h"
#include "RealplotWrapper.h"

Realplot::Realplot()
{
  plot_ = new RealplotWrapper;
}

Realplot::~Realplot()
{
  delete plot_;
}

void Realplot::setNewData(double* data, int numPoints)
{
  plot_->setNewData(data, numPoints);
}

void Realplot::setNewData(float* data, int numPoints)
{
  plot_->setNewData(data, numPoints);
}

void Realplot::setTitle(std::string title)
{
  plot_->setTitle(title);
}

void Realplot::setXAxisScale(double xMin, double xMax)
{
  plot_->setXAxisScale(xMin, xMax);
}

void Realplot::setYAxisScale(double yMin, double yMax)
{
  plot_->setYAxisScale(yMin, yMax);
}

void Realplot::setXAxisAutoScale(bool on=true)
{
  plot_->setXAxisAutoScale(on);
}

void Realplot::setYAxisAutoScale(bool on=true)
{
  plot_->setYAxisAutoScale(on);
}

void Realplot::setXAxisRange(double xMin, double xMax)
{
  plot_->setXAxisRange(xMin, xMax);
}

void Realplot::setLabels(std::string xLabel, std::string yLabel)
{
  plot_->setAxisLabels(xLabel, yLabel);
}

