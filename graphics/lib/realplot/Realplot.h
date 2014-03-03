#ifndef REALPLOT_H
#define REALPLOT_H

#include <string>

class RealplotWrapper;

class Realplot
{
public:
  Realplot();
  ~Realplot();

  template<class Iterator>
  void setNewData(Iterator begin, Iterator end);
  void setNewData(float* data, int numPoints);
  void setNewData(double* data, int numPoints);
  void setTitle(std::string title);
  void setXAxisScale(double xMin, double xMax);
  void setYAxisScale(double yMin, double yMax);
  void setXAxisAutoScale(bool on);
  void setYAxisAutoScale(bool on);
  void setXAxisRange(double xMin, double xMax);
  void setLabels(std::string xLabel, std::string yLabel);

private:
  RealplotWrapper* plot_;
};

template<class Iterator>
void Realplot::setNewData(Iterator begin, Iterator end)
{
  int numPoints = end-begin;
  double* data = new double[numPoints];

  for(int i=0;begin!=end;begin++,i++)
  {
    data[i] = *begin;
  }

  setNewData(data, numPoints);

  delete[] data;
}

#endif // REALPLOT_H
