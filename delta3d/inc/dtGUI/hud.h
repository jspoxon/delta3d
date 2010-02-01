#ifndef DTGUI_HUD_H
#define DTGUI_HUD_H

#include <map>
#include <string>

#include <dtGUI/export.h>
#include <dtGUI/ceguiconnectionmanager.h>
#include <dtGUI/ceguimouselistener.h>
#include <dtGUI/ceguikeyboardlistener.h>
#include <dtGUI/widget.h>
#include <dtCore/refptr.h>
#include <dtCore/base.h>

namespace dtCore
{
    class Mouse;
    class Camera;
    class Keyboard;
    class DeltaDrawable;
}

/// @cond DOXYGEN_SHOULD_SKIP_THIS
//OpenSceneGraph declarations:
#include <osg/Drawable>
namespace osg
{
    class GraphicsContext;
    class State;
    class Camera;
    class Texture2D;
}

//CEGUI declarations:
namespace CEGUI
{
    class System;
    class WindowManager;
    class EventArgs;
}
/// @endcond


namespace dtGUI
{

//dtGUI declarations:
class CEGUIRenderer;



/**
 * @brief
 *     gui-class for rendering and managing widgets
 *
 *     any HUD can use any camera as render-target
 *     each HUD owns it's own root-sheet (@see GetRootSheet()) which always fits the the whole viewport of the targeted-camera
 *
 *     internally each HUD will use an own name-prefix for widgets(if they were created via LoadLayout/CreateWidget).
 *     so if one intend to use cegui-standard-api (example: CEGUI::WindowManager::getWindow()) window names must be prefixed by the HUD's prefix @see GetPrefix()
 *
 *     if creating new connections via Connect/Disconnect one keep in mind that these connections are
 *     handled by the gui-object and thus they will be lost on gui-object-destruction
 *     the Connect/Disconnect are inteded to handle the connection creation/destruction more easyly for widgets.
 *     because of this fact it's wise to use them only for this
 */
class DT_GUI_EXPORT HUD : public dtCore::Base,
                          public CEGUIConnectionManager
{
    public:

        DECLARE_MANAGEMENT_LAYER(HUD)

        /**
         * @brief
         *     constructs a new gui object 
         *
         * @param pTargetCamera
         *     camera where the gui is drawn as hud
         *
         * @param pObservedKeyboard 
         *     observed keyboard-object for input
         *
         * @param pObservedMouse
         *     observed mouse-object for input   
         *
         * @param sName
         *     delta3D-name of the gui-object
         */
        HUD(osg::Camera* pTargetCamera, dtCore::Keyboard* pObservedKeyboard = 0, dtCore::Mouse* pObservedMouse = 0, const std::string& sName="HUD");

        /**
         * @brief
         *     constructs a new gui object 
         *
         * @param pTargetCamera
         *     camera where the gui will show up (as hud)
         *
         * @param pObservedKeyboard 
         *     observed keyboard-object for input
         *
         * @param pObservedMouse
         *     observed mouse-object for input   
         *
         * @param sName
         *     delta3D-name of the gui-object
         */
        HUD(dtCore::Camera* pTargetCamera, dtCore::Keyboard* pObservedKeyboard = 0, dtCore::Mouse* pObservedMouse = 0, const std::string& sName="HUD");

        /********************************************************************************
                                    widget creation & management
         ********************************************************************************/

        ///searches widget by it's name
        Widget* GetWidget(const std::string& sWidgetName, const std::string& sPrefix="");

        ///creates/adds a widget to a given parent
        Widget* CreateWidget(Widget* pParentWidget, const std::string& sWidgetTypeName, const std::string& sWidgetName="", const std::string& sPrefix="");

        ///creates/adds a widget to root-sheet
        Widget* CreateWidget(const std::string& sWidgetTypeName, const std::string& sWidgetName="", const std::string& sPrefix="");

        ///loads a layout-file
        Widget* LoadLayout(const std::string& sFileName, const std::string& sPrefix="", const std::string& sResourceGroup = "");

        ///loads a layout-file and add it to the given Widget(which should a child of the gui)
        Widget* LoadLayout(Widget* pWidgetParent, const std::string& sFileName, const std::string& sPrefix="", const std::string& sResourceGroup = "");

        ///returns default sheet (autogenerated root-window)
        inline Widget* GetRootSheet() { return m_pRootsheet; }

        ///returns default sheet (autogenerated root-window)
        inline const Widget* GetRootsheet() const { return m_pRootsheet; }

        ///returns auto-generated prefix for the gui
        const std::string GetPrefix() const;

        /********************************************************************************
                                         gui management
         ********************************************************************************/

        osg::Geode* GetInternalGraph() const { return m_pInternalGraph; }

        ///sets the rendertarget (camera) for the gui-object
        void SetCamera(osg::Camera* pTargetCamera);

        ///sets the mouse-object from which the gui receives events
        void SetMouse(dtCore::Mouse* pObservedMouse);

        ///returns mouse-object from which the gui receives events
        inline dtCore::Mouse* GetMouse() { return m_pMouse.get(); }

        ///returns mouse-object from which the gui receives events
        inline const dtCore::Mouse* GetMouse() const { return m_pMouse.get(); }

        ///sets keyboard-object which is used for input
        void SetKeyboard(dtCore::Keyboard* pObservedKeyboard);

        ///returns keyboard-object used by the gui for input
        inline dtCore::Keyboard* GetKeyboard() { return m_pKeyboard.get(); }

        ///returns keyboard-object used by the gui for input
        inline const dtCore::Keyboard* GetKeyboard() const { return m_pKeyboard.get(); }

        ///returns associated osg-texture for any widget which has the "Image" property
        osg::Texture2D *GetOrCreateOSGTexture(const std::string& sWidgetName);

        //sets m_pRootWindow as active Sheet (that'll inject/rendering mouse-events/keyboard-events/renderin to m_pRootwindows)
        void MakeCurrent() const;

        ///sets the file path for all gui-objects 
        static void SetFilePath(const std::string& sPath);

        ///returns the file path for gui-xml-data/images @see SetFilePath
        static const std::string& GetFilePath();

        /// returns whether the CEGUI mouse cursor should be visible or not
        bool GetIsMouseCursorVisible() const   { return mIsMouseCursorVisible; }

        /// sets whether the CEGUI mouse cursor should be visible or not
        void SetIsMouseCursorVisible(bool val) { mIsMouseCursorVisible = val; }

        /********************************************************************************
                                 CEGUIConnectionManager-wrapper
         ********************************************************************************/

        ///wraps CEGUIConnectionManager::Connect(this->GetWidget(widgetName), eventName, pObjectMember, pObject)
        template<class T>
        inline void Connect(const std::string& sWidgetName, const std::string& sEventName, bool (T::* pObjectMember)(const CEGUI::EventArgs&), T *pObject)
        {
            CEGUIConnectionManager::Connect( (sWidgetName=="") ? 0 : GetWidget(sWidgetName), sEventName, pObjectMember, pObject);
        }


        ///wraps this->Disconnect(this->GetWidget(widgetName), eventName,  pObjectMember, pObject)
        template<class T>
        inline void Disconnect(const std::string& sWidgetName, const std::string& sEventName, bool (T::* pObjectMember)(const CEGUI::EventArgs&), T *pObject)
        {
            CEGUIConnectionManager::Disconnect((sWidgetName=="") ? 0 : GetWidget(sWidgetName), sEventName, pObjectMember, pObject);
        }

        ///wraps this->Disconnect(this->GetWidget(widgetName), eventName, 'wildcard', pObject)
        template<class T>
        inline void Disconnect(const std::string& sWidgetName, const std::string& sEventName, int, T *pObject)
        {
            CEGUIConnectionManager::Disconnect<T>((sWidgetName=="") ? 0 : GetWidget(sWidgetName), sEventName, 0, pObject);
        }

        ///wraps this->Disconnect(this->GetWidget(widgetName), eventName,  pObjectMember, 'widlcard')
        template<class T>
        inline void Disconnect(const std::string& sWidgetName, const std::string& sEventName, bool (T::* pObjectMember)(const CEGUI::EventArgs&), int)
        {
            CEGUIConnectionManager::Disconnect<T>((sWidgetName=="") ? 0 : GetWidget(sWidgetName), sEventName, pObjectMember, 0);
        }

        ///wraps this->Disconnect(this->GetWidget(widgetName), eventName,  'wildcard', 'wildcard')
        inline void Disconnect(const std::string& sWidgetName, const std::string& sEventName, int, int)
        {
            CEGUIConnectionManager::Disconnect<CEGUIConnectionManager>((sWidgetName=="") ? 0 : GetWidget(sWidgetName), sEventName, 0, 0);
        }



        //wraps this->Connect(this->GetWidget(sWidgetName), sEventName, pFunction)
        inline void Connect(const std::string& sWidgetName, const std::string& sEventName, bool (* pFunction)(const CEGUI::EventArgs&))
        {
            CEGUIConnectionManager::Connect((sWidgetName=="") ? 0 : GetWidget(sWidgetName), sEventName, pFunction);
        }

        //wraps this->Disconnect(this->GetWidget(sWidgetName), sEventName, pFunction)
        inline void Disconnect(const std::string& sWidgetName, const std::string& sEventName, bool (* pFunction)(const CEGUI::EventArgs&))
        {
            CEGUIConnectionManager::Disconnect((sWidgetName=="") ? 0 : GetWidget(sWidgetName), sEventName, pFunction);
        }

        /********************************************************************************
                         static-wrappers (additional gui management)
         ********************************************************************************/

        /// shortcut/wrapper to CEGUI::ImagesetManager::createImageset
        static void LoadImageset(const std::string& sFileName, const std::string& sResourceGroup = "");

        /// shortcut/wrapper to CEGUI::ImagesetManager::createImageset
        static void CreateImageset(const std::string& sImagesetName, const std::string& sFileName, const std::string& sResourceGroup = "");

        /// shortcut/wrapper to CEGUI::ImagesetManager::destroyImageset
        static void UnloadImageset(const std::string& sFileName);

        /// shortcut/wrapper to CEGUI::ImagesetManager::destroyAllImagesets
        static void UnloadAllImagesets();

        /// shortcut/wrapper to CEGUI::SchemeManager::loadScheme
        static void LoadScheme(const std::string& sFileName, const std::string& sResourceGroup = "");

        /// shortcut/wrapper to CEGUI::SchemeManager::unloadScheme
        static void UnloadScheme(const std::string&);

        /// shortcut/wrapper to CEGUI::SchemeManager::undloadAllSchemes
        static void UnloadAllSchemes();

        /// shortcut/wrapper to CEGUI::FontManager::createFont
        static void CreateFont(const std::string&);

        /// shortcut/wrapper to CEGUI::FontManager::destroryFont
        static void DestroyFont(const std::string&);

        /// shortcut/wrapper to CEGUI::FontManager::destroryAllFonts
        static void DestroyAllFonts();

        /// shortcut/wrapper to CEGUI::System::getSingleton().setMouseCursor
        static void SetMouseCursor(const std::string& sImagesetName, const std::string& sImageName);

        // shortcut/wrapper to CEGUI::ImagesetManager::getSingleton().isImagesetPresent
        static bool IsImagesetPresent(const std::string& sImagesetName);

        // shortcut/wrapper to CEGUI::SchemeManager::getSingleton().isSchemePresent
        static bool IsSchemePresent(const std::string& sSchemeName);

        // shortcut/wrapper to CEGUI::WindowManager::getSingleton().isWindowPresent
        static bool IsWindowPresent(const std::string& sWindowName);

   protected:
         ///destructs an existing gui object
         virtual ~HUD();

    private:

        ///used to receive "preframe"-delta-messages
        void OnMessage(dtCore::Base::MessageData *);

        /********************************************************************************
                             methods used internally (code scattering)
         ********************************************************************************/

        void _SetupInternalGraph();
        void _SetupDefaultUI();
        void _CheckCamera();

        static void _SetupSystemAndRenderer();

        /********************************************************************************
                                            HUDDrawable
         ********************************************************************************/

        /**
         * @brief
         *     an osg::Drawable which will render a provided(defined via the constructor) gui
         */
        class HUDDrawable : public osg::Drawable
        {
            public:

                ///copy constructor
                HUDDrawable(const HUDDrawable& drawable, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

                ///constructs an GUIdrawable-object and assign an HUD-object
                HUDDrawable(HUD *);

                ///implementation of osg::Object
                virtual osg::Object* cloneType() const;

                ///implementation of osg::Object
                virtual osg::Object* clone(const osg::CopyOp& copyop) const;

                ///draw-implementation of osg::Object
                virtual void drawImplementation(osg::RenderInfo & renderInfo) const;

            protected:

                ///destructs an HUDDrawable-object
                virtual ~HUDDrawable();

            private:

                HUD *m_pGUI; ///pointer to the assigned GUI
        };

        /********************************************************************************
                               private struct GUIViewport
         ********************************************************************************/

        /**
         * @brief
         *     internal viewport-representation
         */
        struct GUIViewport
        {
            GUIViewport(int _iX, int _iY, int _iWidth, int _iHeight) : 
                iX(_iX),
                iY(_iY),
                iWidth(_iWidth),
                iHeight(_iHeight)
            {}

            int iX;
            int iY;
            int iWidth;
            int iHeight;
        };

        /********************************************************************************
                                      member variables
         ********************************************************************************/
        GUIViewport                           m_Viewport; ///current viewport of the gui
        dtCore::RefPtr<osg::Camera>           m_pCamera; ///camera, whose viewport is used to draw the gui
        dtCore::RefPtr<dtCore::Keyboard>      m_pKeyboard; ///observed keyboard
        dtCore::RefPtr<dtCore::Mouse>         m_pMouse; ///observed mouse
        dtCore::RefPtr<dtGUI::CEGUIKeyboardListener> m_pKeyboardListener; ///needed for injection mouse-events to the cegui
        dtCore::RefPtr<dtGUI::CEGUIMouseListener>    m_pMouseListener; ///needed for injection keyboard-events to the cegui
        osg::Geode*                           m_pInternalGraph; ///osg graph used to render the gui
        CEGUI::Window*                        m_pRootsheet; ///auto-generated panel-window

        bool mIsMouseCursorVisible;

        static std::string FilePath; ///gui-files-system-path (/data/gui)
        static bool SystemAndRendererCreatedByHUD;
};

} //namespace dtHUD

#endif //DTGUI_HUD_H
