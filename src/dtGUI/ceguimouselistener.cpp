#include <dtGUI/ceguimouselistener.h>
#if CEGUI_VERSION_MAJOR >= 0 && CEGUI_VERSION_MINOR < 7
#include <dtGUI/hud.h> //old
#endif
#include <CEGUI/CEGUIInputEvent.h>       // for internal type, CEGUI::Key::Scan
#include <CEGUI/CEGUISystem.h>

using namespace dtGUI;

#if CEGUI_VERSION_MAJOR >= 0 && CEGUI_VERSION_MINOR >= 7
CEGUIMouseListener::CEGUIMouseListener():
mWidth(0)
,mHeight(0)
,mHalfWidth(0)
,mHalfHeight(0)
,mMouseX(0)
,mMouseY(0)
{
}
#else
CEGUIMouseListener::CEGUIMouseListener(HUD *pGui):
m_pGUI(pGui)
,mWidth(0)
,mHeight(0)
,mHalfWidth(0)
,mHalfHeight(0)
,mMouseX(0)
,mMouseY(0)
{
}
#endif

CEGUIMouseListener::~CEGUIMouseListener()
{
}

void CEGUIMouseListener::SetWindowSize(unsigned int width, unsigned int height)
{
   mWidth = width;
   mHeight = height;
   mHalfWidth = width/2;
   mHalfHeight = height/2;
}

bool CEGUIMouseListener::HandleMouseMoved(const dtCore::Mouse* mouse, float x, float y)
{
   MakeCurrent();

   mMouseX = x - mMouseX;
   mMouseY = y - mMouseY;
   CEGUI::System::getSingleton().injectMouseMove(mMouseX * mHalfWidth, mMouseY * -mHalfHeight);
   mMouseX = x;
   mMouseY = y;

   ///\todo document these magic constants from the CEUIDrawable-days.
   return CEGUI::System::getSingleton().injectMousePosition( ((x+1)*0.5f)*mWidth, ((-y+1)*0.5f)*mHeight);
}

bool CEGUIMouseListener::HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y)
{
   return HandleMouseMoved(mouse, x, y);
}

bool CEGUIMouseListener::HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
{
   MakeCurrent();

   UpdateWindowSize();

   switch( button )
   {
   case dtCore::Mouse::LeftButton:
      return CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
      break;
   case dtCore::Mouse::RightButton:
      return CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::RightButton);
      break;
   case dtCore::Mouse::MiddleButton:
      return CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MiddleButton);
      break;
   }

   return false;
}

bool CEGUIMouseListener::HandleButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
{
   MakeCurrent();

   UpdateWindowSize();
   
   switch(button)
   {
   case dtCore::Mouse::LeftButton:
      {
         return CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
      }  break;

   case dtCore::Mouse::RightButton:
      {
         return CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::RightButton);
      }  break;

   case dtCore::Mouse::MiddleButton:
      {
         return CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MiddleButton);
      }  break;
   }

   return false;
}

bool CEGUIMouseListener::HandleMouseScrolled(const dtCore::Mouse* mouse, int delta)
{
   MakeCurrent();

   UpdateWindowSize();

   return CEGUI::System::getSingleton().injectMouseWheelChange( (float)delta );
}

bool CEGUIMouseListener::HandleButtonClicked(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount)
{
   CEGUI::System& system = CEGUI::System::getSingleton();
   bool handled = false;
   for (int i = 0; i < clickCount; ++i) 
   {
      switch(button)
      {
      case dtCore::Mouse::LeftButton:
         {
            handled = system.injectMouseButtonDown(CEGUI::LeftButton);
            handled = system.injectMouseButtonUp(CEGUI::LeftButton);
         }  break;

      case dtCore::Mouse::RightButton:
         {
            handled = system.injectMouseButtonDown(CEGUI::RightButton);
            handled = system.injectMouseButtonUp(CEGUI::RightButton);
         }  break;

      case dtCore::Mouse::MiddleButton:
         {
            handled = system.injectMouseButtonDown(CEGUI::MiddleButton);
            handled = system.injectMouseButtonUp(CEGUI::MiddleButton);
         }  break;

      default:
         break;
      }
   }

   return handled;
}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::CEGUIMouseListener::UpdateWindowSize()
{
#if CEGUI_VERSION_MAJOR >= 0 && CEGUI_VERSION_MINOR >= 7
   CEGUI::Size size = CEGUI::System::getSingleton().getRenderer()->getDisplaySize();
   SetWindowSize(size.d_width, size.d_height);
#else
   SetWindowSize( CEGUI::System::getSingleton().getRenderer()->getWidth(), CEGUI::System::getSingleton().getRenderer()->getHeight() );
#endif
}

////////////////////////////////////////////////////////////////////////////////
void dtGUI::CEGUIMouseListener::MakeCurrent()
{
#if CEGUI_VERSION_MAJOR >= 0 && CEGUI_VERSION_MINOR == 6
   if (m_pGUI != NULL)
   {
      m_pGUI->MakeCurrent();
   }
#endif;
}