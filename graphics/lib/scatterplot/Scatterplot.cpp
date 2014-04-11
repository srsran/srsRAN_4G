#include "Scatterplot.h"
#include "ScatterplotWrapper.h"

Scatterplot::Scatterplot()
{
  plot_ = new ScatterplotWrapper;
}

Scatterplot::~Scatterplot()
{
  delete plot_;
}

void Scatterplot::setNewData(std::complex<float>* data, int numPoints)
{
  plot_->setNewData(data, numPoints);
}

void Scatterplot::setNewData(std::complex<double>* data, int numPoints)
{
  plot_->setNewData(data, numPoints);
}

void Scatterplot::setTitle(std::string title)
{
  plot_->setTitle(title);
}

void Scatterplot::setXAxisScale(double xMin, double xMax)
{
  plot_->setXAxisScale(xMin, xMax);
}

void Scatterplot::setYAxisScale(double yMin, double yMax)
{
  plot_->setYAxisScale(yMin, yMax);
}

void Scatterplot::setXAxisAutoScale(bool on=true)
{
  plot_->setXAxisAutoScale(on);
}

void Scatterplot::setYAxisAutoScale(bool on=true)
{
  plot_->setYAxisAutoScale(on);
}

void Scatterplot::setAxisLabels(std::string xLabel, std::string yLabel)
{
  plot_->setAxisLabels(xLabel, yLabel);
}

