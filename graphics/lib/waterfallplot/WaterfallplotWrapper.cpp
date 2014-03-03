#include "WaterfallplotWrapper.h"

#include "WaterfallWidget.h"
#include "Events.h"
#include <qapplication.h>
#include <QThread>

using namespace std;

WaterfallplotWrapper::WaterfallplotWrapper(int numDataPoints, int numRows)
    :widget_(NULL)
    ,destroyed_(true)
{
  if(QCoreApplication::instance() == NULL)
    return; //TODO: throw exception here in Iris
  if(QCoreApplication::instance()->thread() == QThread::currentThread())
  {
    connect( this, SIGNAL( createWidgetSignal(int, int) ),
             this, SLOT(createWidgetSlot(int, int)) );
    connect( this, SIGNAL( destroyWidgetSignal() ),
             this, SLOT(destroyWidgetSlot()) );
    connect( this, SIGNAL( destroyWidgetSignalBlocking() ),
             this, SLOT(destroyWidgetSlot()) );
  }
  else
  {
    connect( this, SIGNAL( createWidgetSignal(int, int) ),
             this, SLOT(createWidgetSlot(int, int)),
             Qt::BlockingQueuedConnection );
    connect( this, SIGNAL( destroyWidgetSignal() ),
             this, SLOT(destroyWidgetSlot()) );
    connect( this, SIGNAL( destroyWidgetSignalBlocking() ),
             this, SLOT(destroyWidgetSlot()),
             Qt::BlockingQueuedConnection );
    moveToThread(QCoreApplication::instance()->thread());
  }
  emit createWidgetSignal(numDataPoints, numRows);
}

WaterfallplotWrapper::~WaterfallplotWrapper()
{
  if(destroyed_)
    emit destroyWidgetSignal();
  else
    emit destroyWidgetSignalBlocking();
}

void WaterfallplotWrapper::createWidgetSlot(int numDataPoints, int numRows)
{
  widget_ = new WaterfallWidget(numDataPoints, numRows);
  destroyed_ = false;
  widget_->setAttribute(Qt::WA_DeleteOnClose, true);
  connect(widget_, SIGNAL( destroyed() ),
          this, SLOT( widgetDestroyed() ));
  connect(this, SIGNAL(setWidgetTitle(QString)),
          widget_, SLOT(setWidgetTitle(QString)));
  connect(this, SIGNAL(setWidgetPXLabel(QString)),
          widget_, SLOT(setPlotXLabel(QString)));
  connect(this, SIGNAL(setWidgetPYLabel(QString)),
          widget_, SLOT(setPlotYLabel(QString)));
  connect(this, SIGNAL(setWidgetPXAxisRange(double, double)),
          widget_, SLOT(setPlotXAxisRange(double, double)));
  connect(this, SIGNAL(setWidgetPXAxisScale(double, double)),
          widget_, SLOT(setPlotXAxisScale(double, double)));
  connect(this, SIGNAL(setWidgetPYAxisScale(double, double)),
          widget_, SLOT(setPlotYAxisScale(double, double)));
  connect(this, SIGNAL(setWidgetSXLabel(QString)),
          widget_, SLOT(setSpectrogramXLabel(QString)));
  connect(this, SIGNAL(setWidgetSYLabel(QString)),
          widget_, SLOT(setSpectrogramYLabel(QString)));
  connect(this, SIGNAL(setWidgetSXAxisRange(double, double)),
          widget_, SLOT(setSpectrogramXAxisRange(double, double)));
  connect(this, SIGNAL(setWidgetSYAxisRange(double, double)),
          widget_, SLOT(setSpectrogramYAxisRange(double, double)));
  connect(this, SIGNAL(setWidgetSZAxisScale(double, double)),
          widget_, SLOT(setSpectrogramZAxisScale(double, double)));

  widget_->resize( 800, 600 );
  widget_->show();
}

void WaterfallplotWrapper::destroyWidgetSlot()
{
  delete widget_;
}

void WaterfallplotWrapper::widgetDestroyed()
{
  destroyed_ = true;
}

void WaterfallplotWrapper::appendNewData(float* data, int numPoints)
{
  if(destroyed_)
    return;
  qApp->postEvent(widget_, new RealDataEvent(data, numPoints));
}

void WaterfallplotWrapper::appendNewData(double* data, int numPoints)
{
  if(destroyed_)
    return;
  qApp->postEvent(widget_, new RealDataEvent(data, numPoints));
}


void WaterfallplotWrapper::setTitle(std::string title)
{
  if(destroyed_)
    return;
  QString str = QString::fromUtf8(title.c_str());
  emit setWidgetTitle(str);
}

void WaterfallplotWrapper::setPlotXLabel(std::string xLabel)
{
  if(destroyed_)
    return;
  QString str = QString::fromUtf8(xLabel.c_str());
  emit setWidgetPXLabel(str);
}

void WaterfallplotWrapper::setPlotYLabel(std::string yLabel)
{
  if(destroyed_)
    return;
  QString str = QString::fromUtf8(yLabel.c_str());
  emit setWidgetPYLabel(str);
}

void WaterfallplotWrapper::setPlotXAxisRange(double xMin, double xMax)
{
  if(destroyed_)
    return;
  emit setWidgetPXAxisRange(xMin, xMax);
}

void WaterfallplotWrapper::setPlotXAxisScale(double xMin, double xMax)
{
  if(destroyed_)
    return;
  emit setWidgetPXAxisScale(xMin, xMax);
}

void WaterfallplotWrapper::setPlotYAxisScale(double yMin, double yMax)
{
  if(destroyed_)
    return;
  emit setWidgetPYAxisScale(yMin, yMax);
}

void WaterfallplotWrapper::setSpectrogramXAxisRange(double xMin, double xMax)
{
  if(destroyed_)
    return;
  emit setWidgetSXAxisRange(xMin, xMax);
}

void WaterfallplotWrapper::setSpectrogramXLabel(std::string xLabel)
{
  if(destroyed_)
    return;
  QString str = QString::fromUtf8(xLabel.c_str());
  emit setWidgetSXLabel(str);
}

void WaterfallplotWrapper::setSpectrogramYLabel(std::string yLabel)
{
  if(destroyed_)
    return;
  QString str = QString::fromUtf8(yLabel.c_str());
  emit setWidgetSYLabel(str);
}

void WaterfallplotWrapper::setSpectrogramYAxisRange(double yMin, double yMax)
{
  if(destroyed_)
    return;
  emit setWidgetSYAxisRange(yMin, yMax);
}

void WaterfallplotWrapper::setSpectrogramZAxisScale(double zMin, double zMax)
{
  if(destroyed_)
    return;
  emit setWidgetSZAxisScale(zMin, zMax);
}
