/* -*-c++-*-
 * testShaders - testshaders (.h & .cpp) - Using 'The MIT License'
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//////////////////////////////////////////////////////////////////////////////
// NOTE - This test is provided for historical reference. The ability to
// use shaders is now provided via the ShaderManager functionality. This is used
// in a variety of examples, demos and test apps. See Tutorial Library.
//////////////////////////////////////////////////////////////////////////////

#include <dtABC/application.h>
#include <dtCore/globals.h>
#include <dtCore/object.h>
#include <osg/Drawable>
#include <osg/PrimitiveSet>
#include <osg/Program>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/shadermanager.h>
#include <dtDAL/project.h>
#include <osgGA/GUIEventAdapter>

using namespace dtABC;
using namespace dtCore;

class TestShadersApp : public Application
{

public:
   TestShadersApp(const std::string& configFilename = "testshadersconfig.xml")
      : Application(configFilename)
   {
      mTotalTime = 0.0f;

      std::string contextName = dtCore::GetDeltaRootPath() + "/examples/data/demoMap";
      dtDAL::Project::GetInstance().SetContext(contextName, true);

      //load the xml file which specifies our shaders
      dtCore::ShaderManager& sm = dtCore::ShaderManager::GetInstance();
      sm.LoadShaderDefinitions("Shaders/ShaderDefs.xml");

      LoadGeometry();
      EnableShaders();

      Transform xform(0.0f, -3.0f, 0.0f, 0.0f, 0.0f, 0.0f);
      GetCamera()->SetTransform(xform);

      GetWindow()->SetWindowTitle("testShaders");
   }

protected:

   virtual ~TestShadersApp() {}

public:

   void LoadGeometry()
   {
      mObject = new dtCore::Object("Happy Sphere");
      mObject->LoadFile("models/physics_happy_sphere.ive");
      AddDrawable(mObject.get());
   }

   void EnableShaders()
   {
      dtCore::ShaderManager& sm = dtCore::ShaderManager::GetInstance();
      dtCore::ShaderProgram* sp = sm.FindShaderPrototype("TestShader", "TestShader");

      if (sp != NULL)
      {
         sm.AssignShaderFromPrototype(*sp, *mObject->GetOSGNode());
         mEnabled = true;
      }
   }

   void DisableShaders()
   {
      dtCore::ShaderManager& sm = dtCore::ShaderManager::GetInstance();
      sm.UnassignShaderFromNode(*mObject->GetOSGNode());
      mEnabled = false;
   }

   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int key)
   {
      bool verdict(false);
      if (key == osgGA::GUIEventAdapter::KEY_Escape)
      {
         this->Quit();
         verdict = true;
      }
      else if (key == osgGA::GUIEventAdapter::KEY_Space)
      {
         //osg::StateSet* ss = mObject->GetOSGNode()->getOrCreateStateSet();

         if (mEnabled)
         {
            DisableShaders();
         }
         else
         {
            EnableShaders();
         }
         verdict = true;
      }

      return verdict;
   }

   virtual void PreFrame(const double deltaFrameTime )
   {
      mTotalTime += deltaFrameTime;

      osg::Matrix rotateMat;
      rotateMat.makeRotate(osg::DegreesToRadians(30.0f) * mTotalTime, osg::Vec3(0.0f, 0.0f, 1.0f));
      Transform xform;
      mObject->GetTransform(xform);
      xform.SetRotation(rotateMat);
      mObject->SetTransform(xform);
   }

private:

   RefPtr<dtCore::Object> mObject;

   float                  mTotalTime;
   bool                   mEnabled;

   DeltaWin::Resolution   mRes;
};

int main(int argc, char* argv[])
{

   std::string dataPath = dtCore::GetDeltaDataPathList();
   dtCore::SetDataFilePathList(dataPath + ";" +
                               dtCore::GetDeltaRootPath() + "/examples/data" + ";" +
                               dtCore::GetDeltaRootPath() + "/examples/testShaders");

   RefPtr<TestShadersApp> app = new TestShadersApp( "testshadersconfig.xml" );
   app->Config();
   app->Run();

   return 0;
}
