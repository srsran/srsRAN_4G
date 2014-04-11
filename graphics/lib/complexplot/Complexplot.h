#ifndef COMPLEXPLOT_H
#define COMPLEXPLOT_H

#include <string>
#include <complex>

class ComplexplotWrapper;

class Complexplot
{
public:
  enum PlotId
  {
    I,
    Q,
    Magnitude,
    Phase
  };
  Complexplot();
  ~Complexplot();

  template<class Iterator>
  void setNewData(Iterator begin, Iterator end);
  void setNewData(std::complex<float>* data, int numPoints);
  void setNewData(std::complex<double>* data, int numPoints);
  void setTitle(std::string title);
  void setXAxisAutoScale(PlotId id, bool on);
  void setYAxisAutoScale(PlotId id, bool on);
  void setXAxisScale(PlotId id, double xMin, double xMax);
  void setYAxisScale(PlotId id, double yMin, double yMax);
  void setXAxisRange(double xMin, double xMax);

private:
  ComplexplotWrapper* plot_;
};

template<class Iterator>
void Complexplot::setNewData(Iterator begin, Iterator end)
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

#endif // COMPLEXPLOT_H
