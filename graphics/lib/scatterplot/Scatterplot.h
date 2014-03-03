#ifndef SCATTERPLOT_H
#define SCATTERPLOT_H

#include <string>
#include <complex>

class ScatterplotWrapper;

class Scatterplot
{
public:
  Scatterplot();
  ~Scatterplot();

  template<class Iterator>
  void setNewData(Iterator begin, Iterator end);
  void setNewData(std::complex<float>* data, int numPoints);
  void setNewData(std::complex<double>* data, int numPoints);
  void setTitle(std::string title);
  void setXAxisScale(double xMin, double xMax);
  void setYAxisScale(double yMin, double yMax);
  void setXAxisAutoScale(bool on);
  void setYAxisAutoScale(bool on);
  void setAxisLabels(std::string xLabel, std::string yLabel);

private:
  ScatterplotWrapper* plot_;
};

template<class Iterator>
void Scatterplot::setNewData(Iterator begin, Iterator end)
{
  int numPoints = end-begin;
  std::complex<double>* data = new std::complex<double>[numPoints];

  for(int i=0;begin!=end;begin++,i++)
  {
    data[i] = *begin;
  }

  setNewData(data, numPoints);

  delete[] data;
}

#endif // SCATTERPLOT_H
