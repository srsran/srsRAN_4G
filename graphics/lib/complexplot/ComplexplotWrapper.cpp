#include "ComplexplotWrapper.h"

#include "ComplexWidget.h"
#include "Events.h"
#include <qapplication.h>
#include <QThread>

using namespace std;


ComplexplotWrapper::ComplexplotWrapper()
    :widget_(NULL)
    ,destroyed_(true)
{
  if(QCoreApplication::instance() == NULL)
    return; //TODO: throw exception here in Iris
  if(QCoreApplication::instance()->thread() == QThread::currentThread())
  {
    connect( this, SIGNAL( createWidgetSignal() ),
             this, SLOT(createWidgetSlot()) );
    connect( this, SIGNAL( destroyWidgetSignal() ),
             this, SLOT(destroyWidgetSlot()) );
    connect( this, SIGNAL( destroyWidgetSignalBlocking() ),
             this, SLOT(destroyWidgetSlot()) );
  }
  else
  {
    connect( this, SIGNAL( createWidgetSignal() ),
             this, SLOT(createWidgetSlot()),
             Qt::BlockingQueuedConnection );
    connect( this, SIGNAL( destroyWidgetSignal() ),
             this, SLOT(destroyWidgetSlot()) );
    connect( this, SIGNAL( destroyWidgetSignalBlocking() ),
             this, SLOT(destroyWidgetSlot()),
             Qt::BlockingQueuedConnection );
    moveToThread(QCoreApplication::instance()->thread());
  }
  emit createWidgetSignal();
}

ComplexplotWrapper::~ComplexplotWrapper()
{
  if(destroyed_)
    emit destroyWidgetSignal();
  else
    emit destroyWidgetSignalBlocking();
}

void ComplexplotWrapper::createWidgetSlot()
{
  widget_ = new ComplexWidget;
  destroyed_ = false;
  widget_->setAttribute(Qt::WA_DeleteOnClose, true);
  connect(widget_, SIGNAL( destroyed() ),
          this, SLOT( widgetDestroyed() ));
  connect(this, SIGNAL(setWidgetTitle(QString)),
          widget_, SLOT(setWidgetTitle(QString)));
  connect(this, SIGNAL(setWidgetXAxisScale(int,double,double)),
          widget_, SLOT(setWidgetXAxisScale(int,double,double)));
  connect(this, SIGNAL(setWidgetYAxisScale(int,double,double)),
          widget_, SLOT(setWidgetYAxisScale(int,double,double)));
  connect(this, SIGNAL(setWidgetXAxisAutoScale(int,bool)),
          widget_, SLOT(setWidgetXAxisAutoScale(int,bool)));
  connect(this, SIGNAL(setWidgetYAxisAutoScale(int,bool)),
          widget_, SLOT(setWidgetYAxisAutoScale(int,bool)));
  connect(this, SIGNAL(setWidgetXAxisRange(double,double)),
          widget_, SLOT(setWidgetXAxisRange(double,double)));

  widget_->resize( 800, 600 );
  widget_->show();
}

void ComplexplotWrapper::destroyWidgetSlot()
{
  if(widget_)
    delete widget_;
  widget_ = NULL;
}

void ComplexplotWrapper::widgetDestroyed()
{
  destroyed_ = true;
}

void ComplexplotWrapper::setNewData(complex<double>* data, int numPoints)
{
  if(destroyed_)
    return;
  qApp->postEvent(widget_, new ComplexDataEvent(data, numPoints));
}

void ComplexplotWrapper::setNewData(complex<float>* data, int numPoints)
{
  if(destroyed_)
    return;
  qApp->postEvent(widget_, new ComplexDataEvent(data, numPoints));
}

void ComplexplotWrapper::setTitle(std::string title)
{
  if(destroyed_)
    return;
  QString str = QString::fromUtf8(title.c_str());
  emit setWidgetTitle(str);
}

void ComplexplotWrapper::setXAxisAutoScale(int id, bool on=true)
{
  if(destroyed_)
    return;
  emit setWidgetXAxisAutoScale(id, on);
}

void ComplexplotWrapper::setYAxisAutoScale(int id, bool on=true)
{
  if(destroyed_)
    return;
  emit setWidgetYAxisAutoScale(id, on);
}

void ComplexplotWrapper::setXAxisScale(int id, double xMin, double xMax)
{
  if(destroyed_)
    return;
  emit setWidgetXAxisScale(id, xMin, xMax);
}

void ComplexplotWrapper::setYAxisScale(int id, double yMin, double yMax)
{
  if(destroyed_)
    return;
  emit setWidgetYAxisScale(id, yMin, yMax);
}

void ComplexplotWrapper::setXAxisRange(double xMin, double xMax)
{
  if(destroyed_)
    return;
  emit setWidgetXAxisRange(xMin, xMax);
}

