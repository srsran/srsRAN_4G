#ifndef WATERFALLDATA_H
#define WATERFALLDATA_H

#include <qwt_raster_data.h>
#include <boost/circular_buffer.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <algorithm>
#include <cmath>
#include "irisapi/Exceptions.h"


class WaterfallData
    :public QwtRasterData
{
public:
  typedef std::vector<double>                       Vec;
  typedef boost::shared_ptr< std::vector<double> >  VecPtr;
  typedef boost::circular_buffer< VecPtr >          VecPtrBuf;
  typedef VecPtrBuf::iterator                       VecPtrBufIt;

  WaterfallData(int numDataPoints, int numRows)
    :QwtRasterData()
    ,nData_(numDataPoints)
    ,nRows_(numRows)
    ,data_(numRows)
  {
    for(int i=0;i<nRows_;i++)
    {
      data_.push_front(VecPtr(new std::vector<double>(nData_)));
      data_[0]->assign(nData_, 0.0);
    }
  }

  void appendData(double* data, int n)
  {
    if(n != nData_)
      throw iris::InvalidDataException("WaterfallData: invalid data length");

    VecPtr v = data_.front();
    v->assign(data, data+n);
    data_.push_back(v);
  }

  double max()
  {
    Vec maxVec;
    for(int i=0;i<nRows_;i++)
    {
      VecPtr v = data_[i];
      maxVec.push_back(*(std::max_element(v->begin(),v->end())));
    }
    return *(std::max_element(maxVec.begin(),maxVec.end()));
  }

  double min()
  {
    Vec minVec;
    for(int i=0;i<nRows_;i++)
    {
      VecPtr v = data_[i];
      minVec.push_back(*(std::min_element(v->begin(),v->end())));
    }
    return *(std::min_element(minVec.begin(),minVec.end()));
  }

  double value(double x, double y) const
  {
    double bottom = interval(Qt::YAxis).minValue();
    double top = interval(Qt::YAxis).maxValue();
    double left = interval(Qt::XAxis).minValue();
    double right = interval(Qt::XAxis).maxValue();
    double xStep = std::abs(right-left)/nData_;
    double yStep = std::abs(top-bottom)/nRows_;

    int ix = (x-left) / xStep;
    int iy = (y-bottom) / yStep;
    if(ix >= nData_)
      ix = nData_-1;
    if(iy >= nRows_)
      iy = nRows_-1;
    double ret = (*data_[iy])[ix];
    return ret;
  }

private:
  VecPtrBuf data_;
  int nData_;
  int nRows_;
};

#endif // WATERFALLDATA_H
