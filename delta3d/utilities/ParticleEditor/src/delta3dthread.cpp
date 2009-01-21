#include <delta3dthread.h>

////////////////////////////////////////////////////////////////////////////////
class EmbeddedWindowSystemWrapper: public osg::GraphicsContext::WindowingSystemInterface
{
   public:
      EmbeddedWindowSystemWrapper(osg::GraphicsContext::WindowingSystemInterface& oldInterface):
         mInterface(&oldInterface)
      {
      }
      
      virtual unsigned int getNumScreens(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier = 
         osg::GraphicsContext::ScreenIdentifier())
      {
         return mInterface->getNumScreens(screenIdentifier);
      }

      virtual void getScreenResolution(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier, 
               unsigned int& width, unsigned int& height)
      {
         mInterface->getScreenResolution(screenIdentifier, width, height);
      }

      virtual bool setScreenResolution(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier, 
               unsigned int width, unsigned int height)
      {
         return mInterface->setScreenResolution(screenIdentifier, width, height);
      }

      virtual bool setScreenRefreshRate(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
               double refreshRate)
      {
         return mInterface->setScreenRefreshRate(screenIdentifier, refreshRate);
      }

      virtual osg::GraphicsContext* createGraphicsContext(osg::GraphicsContext::Traits* traits)
      {
         return new osgViewer::GraphicsWindowEmbedded(traits);
      }

   protected:
      virtual ~EmbeddedWindowSystemWrapper() {};
   private:
      dtCore::RefPtr<osg::GraphicsContext::WindowingSystemInterface> mInterface;
};

////////////////////////////////////////////////////////////////////////////////
Delta3DThread::Delta3DThread(QObject* parent):
QThread(parent)
{
}

////////////////////////////////////////////////////////////////////////////////
Delta3DThread::~Delta3DThread()
{
   if (isRunning())
   {
      this->quit();
   }

   dtCore::System::GetInstance().Stop();
}

////////////////////////////////////////////////////////////////////////////////
void Delta3DThread::run()
{
   osg::GraphicsContext::WindowingSystemInterface* winSys = osg::GraphicsContext::getWindowingSystemInterface();

   if (winSys != NULL)
   {
      osg::GraphicsContext::setWindowingSystemInterface(new EmbeddedWindowSystemWrapper(*winSys));
   }

   //need to set the current context so that all the open gl stuff in osg can initialize.
   ViewWindow& glWidget = *mWin->GetGLWidget();
   //glWidget.ThreadedInitializeGL();
   //glWidget.ThreadedMakeCurrent();
   
   mpViewer = new ParticleViewer();

   glWidget.SetGraphicsWindow(*mpViewer->GetWindow()->GetOsgViewerGraphicsWindow());
   
   mpViewer->Config();

   mWin->SetParticleViewer(mpViewer);
   mWin->SetupUI();

   dtCore::System::GetInstance().Start();
   mTimer.start(10);
}

////////////////////////////////////////////////////////////////////////////////
