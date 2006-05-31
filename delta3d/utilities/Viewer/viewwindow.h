#ifndef DELTA_VIEWWINDOW
#define DELTA_VIEWWINDOW

// header files
#include <vector>

#include <dtABC/fl_dt_window.h>

#include "viewer.h"



// forward references
class ViewState;
class UserInterface;



// FLTK window to be shown
class ViewWindow  :  public   Fl_DT_Window<Viewer>
{
      typedef  Fl_DT_Window<Viewer>   MyParent;

      DECLARE_MANAGEMENT_LAYER(ViewWindow)

   private:
      typedef  std::vector<ViewState*> FILE_LIST;

      static   const char*             DEF_PATH;

   public:
                                       ViewWindow();
                                       ViewWindow( int w, int h, const char* l = 0L );
                                       ViewWindow( int x, int y, int w, int h, const char* l = 0L );
   protected:
      virtual                          ~ViewWindow();
   public:
      virtual  void                    show( void );
      virtual  void                    OnMessage( dtCore::Base::MessageData* data );
      virtual  void                    FileLoaded( bool loaded, const char* filename );
      virtual  void                    DisplayError( const std::string &msg );
               const char*             GetPath( void );
               void                    SetPath( const char* path );
               void                    Container( UserInterface* container );
               bool                    LoadFile( const char* file );
               bool                    SaveFileAs( const char* filename );
               void                    SelectFile( unsigned int indx );
               void                    SetDisplay( unsigned int state, bool value = true );
               void                    SetMotion( unsigned int state );
               void                    SetJoystick( unsigned int state, bool value = true );
               void                    CommandLine( int argc, char** argv );

   private:
               void                    ctor( void );
               void                    KeyboardEventHandler( const dtABC::KeyboardEvent& ev );

   private:
               std::string             mPath;
               FILE_LIST               mFileList;
               ViewState*              mCurFile;
               UserInterface*          mContainer;
               bool                    mFileLoaded; ///<magic
};
#endif // DELTA_VIEWWINDOW
