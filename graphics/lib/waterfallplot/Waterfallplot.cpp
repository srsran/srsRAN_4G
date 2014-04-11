#include "Waterfallplot.h"
#include "WaterfallplotWrapper.h"

using namespace std;

Waterfallplot::Waterfallplot(int numDataPoints, int numRows)
{
  plot_ = new WaterfallplotWrapper(numDataPoints, numRows);
}

Waterfallplot::~Waterfallplot()
{
  delete plot_;
}

void Waterfallplot::appendNewData(float* data, int numPoints)
{
  plot_->appendNewData(data, numPoints);
}

void Waterfallplot::appendNewData(double* data, int numPoints)
{
  plot_->appendNewData(data, numPoints);
}

void Waterfallplot::setTitle(std::string title)
{
  plot_->setTitle(title);
}

void Waterfallplot::setPlotXLabel(std::string xLabel)
{
  plot_->setPlotXLabel(xLabel);
}

void Waterfallplot::setPlotYLabel(std::string yLabel)
{
  plot_->setPlotYLabel(yLabel);
}

void Waterfallplot::setPlotXAxisRange(double xMin, double xMax)
{
  plot_->setPlotXAxisRange(xMin, xMax);
}

void Waterfallplot::setPlotXAxisScale(double xMin, double xMax)
{
  plot_->setPlotXAxisScale(xMin, xMax);
}

void Waterfallplot::setPlotYAxisScale(double yMin, double yMax)
{
  plot_->setPlotYAxisScale(yMin, yMax);
}

void Waterfallplot::setSpectrogramXLabel(std::string xLabel)
{
  plot_->setSpectrogramXLabel(xLabel);
}

void Waterfallplot::setSpectrogramYLabel(std::string yLabel)
{
  plot_->setSpectrogramYLabel(yLabel);
}

void Waterfallplot::setSpectrogramXAxisRange(double xMin, double xMax)
{
  plot_->setSpectrogramXAxisRange(xMin, xMax);
}

void Waterfallplot::setSpectrogramYAxisRange(double yMin, double yMax)
{
  plot_->setSpectrogramYAxisRange(yMin, yMax);
}

void Waterfallplot::setSpectrogramZAxisScale(double zMin, double zMax)
{
  plot_->setSpectrogramZAxisScale(zMin, zMax);
}

