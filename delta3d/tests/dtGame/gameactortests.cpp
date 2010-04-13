/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * @author Eddie Johnson and David Guthrie
 */

#include <prefix/unittestprefix.h>
#include <iostream>
#include <osg/Math>
#include <dtUtil/log.h>
#include <dtUtil/datapathutils.h>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/camera.h>
#include <dtCore/timer.h>
#include <dtDAL/datatype.h>
#include <dtDAL/resourcedescriptor.h>
#include <dtDAL/actortype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/project.h>
#include <dtUtil/datastream.h>
#include <dtGame/messageparameter.h>
#include <dtGame/machineinfo.h>
#include <dtGame/gameactor.h>
#include <dtGame/basemessages.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/environmentactor.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/invokable.h>
#include <dtABC/application.h>
#include <testGameActorLibrary/testgameactorlibrary.h>
#include <testGameActorLibrary/testgameenvironmentactor.h>
#include <testGameActorLibrary/testgamepropertyproxy.h>
#include <testGameActorLibrary/testgameactor.h>
#include <dtCore/observerptr.h>

#include <cppunit/extensions/HelperMacros.h>

#include "testcomponent.h"
extern dtABC::Application& GetGlobalApplication();

class GameActorTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(GameActorTests);

      CPPUNIT_TEST(TestGameActorProxy);
      //CPPUNIT_TEST(TestGameActorNoDefaultStateSet);
      CPPUNIT_TEST(TestGameActorProxyDeleteError);
      CPPUNIT_TEST(TestSetEnvironmentActor);
      CPPUNIT_TEST(TestAddRemoveFromEnvActor);
      CPPUNIT_TEST(TestInvokables);
      CPPUNIT_TEST(TestInvokableMessageRegistration);
      CPPUNIT_TEST(TestGlobalInvokableMessageRegistration);
      CPPUNIT_TEST(TestGlobalInvokableMessageRegistrationEndOfFrame);
      CPPUNIT_TEST(TestStaticGameActorTypes);
      CPPUNIT_TEST(TestEnvironmentTimeConversions);
      CPPUNIT_TEST(TestDefaultProcessMessageRegistration);
      CPPUNIT_TEST(TestMessageProcessingPerformance);
      CPPUNIT_TEST(TestActorIsInGM);
      CPPUNIT_TEST(TestOnRemovedActor);
      CPPUNIT_TEST(TestAddActorComponent);
      CPPUNIT_TEST(TestActorComponentInitialized);
      CPPUNIT_TEST(TestGetAllActorComponents);
      CPPUNIT_TEST(TestUnregisterNextInvokable);

   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();
   void TestGameActorProxy();
   void TestGameActorNoDefaultStateSet();
   void TestGameActorProxyDeleteError();
   void TestSetEnvironmentActor();
   void TestAddRemoveFromEnvActor();
   void TestInvokables();
   void TestInvokableMessageRegistration();
   void TestDefaultProcessMessageRegistration();
   void TestGlobalInvokableMessageRegistration();
   void TestGlobalInvokableMessageRegistrationEndOfFrame();
   void TestStaticGameActorTypes();
   void TestEnvironmentTimeConversions();
   void TestMessageProcessingPerformance();
   void TestActorIsInGM();
   void TestOnRemovedActor();
   void TestAddActorComponent();
   void TestActorComponentInitialized();
   void TestGetAllActorComponents();
   void TestUnregisterNextInvokable();

private:
   static const std::string mTestGameActorLibrary;
   static const std::string mTestActorLibrary;
   dtCore::RefPtr<dtGame::GameManager> mManager;
};


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(GameActorTests);

const std::string GameActorTests::mTestGameActorLibrary = "testGameActorLibrary";
const std::string GameActorTests::mTestActorLibrary     = "testActorLibrary";

////////////////////////////////////////////////////////////////////////
void GameActorTests::setUp()
{
   try
   {
      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();
      dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList());

      mManager = new dtGame::GameManager(*GetGlobalApplication().GetScene());
      mManager->SetApplication(GetGlobalApplication());
      mManager->LoadActorRegistry(mTestGameActorLibrary);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& ex)
   {
      CPPUNIT_FAIL(std::string("Error: ") + ex.what());
   }
}

////////////////////////////////////////////////////////////////////////
void GameActorTests::tearDown()
{
   dtCore::System::GetInstance().Stop();
   if (mManager.valid())
   {
      mManager->DeleteAllActors(true);
      mManager->UnloadActorRegistry(mTestGameActorLibrary);
      mManager = NULL;
   }
}

////////////////////////////////////////////////////////////////////////
void GameActorTests::TestActorIsInGM()
{
   try
   {
      dtCore::RefPtr<const dtDAL::ActorType> actorType = mManager->FindActorType("ExampleActors", "Test1Actor");
      dtCore::RefPtr<dtDAL::ActorProxy> proxy = mManager->CreateActor(*actorType);
      dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());
      CPPUNIT_ASSERT_MESSAGE("ActorProxy should not be NULL", gap != NULL);
      CPPUNIT_ASSERT_MESSAGE("ActorProxy Should not be in GM", gap->IsInGM() != true);

      mManager->AddActor(*gap.get(), false, false);

      CPPUNIT_ASSERT_MESSAGE("ActorProxy Should be in GM", gap->IsInGM() != false);
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

void GameActorTests::TestGameActorProxy()
{
   try
   {
      dtCore::RefPtr<dtGame::GameActorProxy> gap;
      mManager->CreateActor("ExampleActors", "Test1Actor", gap);

      CPPUNIT_ASSERT_MESSAGE("GameActorProxy should not be NULL", gap != NULL);
      CPPUNIT_ASSERT_MESSAGE("GameActor should have a reference to the proxy", &gap->GetGameActor().GetGameActorProxy() == gap.get());

      dtGame::GameActor* p = &gap->GetGameActor();

      CPPUNIT_ASSERT_MESSAGE("Actor should not be NULL", p != NULL);
      CPPUNIT_ASSERT_MESSAGE("IsGameActor should always return true", gap->IsGameActorProxy());
      CPPUNIT_ASSERT_MESSAGE("Game Actor should not initially be published", !gap->IsPublished());
      CPPUNIT_ASSERT_MESSAGE("Game Actor should not initially be remote", !gap->IsRemote());


      CPPUNIT_ASSERT_MESSAGE("GameActorProxy's ownership should default to SERVER_LOCAL",
               gap->GetInitialOwnership() == dtGame::GameActorProxy::Ownership::SERVER_LOCAL);

      gap->SetInitialOwnership(dtGame::GameActorProxy::Ownership::SERVER_LOCAL);

      CPPUNIT_ASSERT_MESSAGE("GameActorProxy's ownership should have been set", gap->GetInitialOwnership() == dtGame::GameActorProxy::Ownership::SERVER_LOCAL);

      CPPUNIT_ASSERT_MESSAGE("GameActorProxy local actor update policy should default to ACCEPT_ALL",
               gap->GetLocalActorUpdatePolicy() == dtGame::GameActorProxy::LocalActorUpdatePolicy::ACCEPT_ALL);

      gap->SetLocalActorUpdatePolicy(dtGame::GameActorProxy::LocalActorUpdatePolicy::IGNORE_ALL);

      CPPUNIT_ASSERT_MESSAGE("GameActorProxy local actor update policy should now be IGNORE_ALL",
               gap->GetLocalActorUpdatePolicy() == dtGame::GameActorProxy::LocalActorUpdatePolicy::IGNORE_ALL);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
//   catch (const std::exception& e)
//   {
//      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
//   }
}

void GameActorTests::TestGameActorNoDefaultStateSet()
{
   try
   {
      dtCore::RefPtr<dtGame::GameActorProxy> gap;
      mManager->CreateActor("ExampleActors", "Test1Actor", gap);
      CPPUNIT_ASSERT_MESSAGE("GameActorProxy should not be NULL", gap != NULL);

      dtGame::GameActor* gameActor = NULL;
      gap->GetActor(gameActor);

      CPPUNIT_ASSERT_MESSAGE("a newly created game actor should not have stateset.",
               gameActor->GetOSGNode()->getStateSet() == NULL);

      mManager->AddActor(*gap, false, false);

      CPPUNIT_ASSERT_MESSAGE("A game actor that has just been added to the gm should not have stateset",
               gameActor->GetOSGNode()->getStateSet() == NULL);

      gameActor->SetShaderGroup(std::string());

      CPPUNIT_ASSERT_MESSAGE("Setting the shader group to empty should not create a stateset",
               gameActor->GetOSGNode()->getStateSet() == NULL);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

void GameActorTests::TestGameActorProxyDeleteError()
{
   // This test verifies that an error is thrown if the proxy is deleted but the actor is still around
   // but the developer calls GetGameActorProxy.  This can happen because the GameACtor only holds onto
   // an observer_ptr to the parent.
   try
   {
      dtCore::RefPtr<const dtDAL::ActorType> actorType = mManager->FindActorType("ExampleActors", "Test1Actor");

      CPPUNIT_ASSERT(actorType != NULL);

      dtCore::RefPtr<dtGame::GameActorProxy> gap;
      mManager->CreateActor(*actorType, gap);

      CPPUNIT_ASSERT_MESSAGE("GameActorProxy should not be NULL", gap != NULL);
      CPPUNIT_ASSERT_MESSAGE("GameActor should have a reference to the proxy", &gap->GetGameActor().GetGameActorProxy() == gap.get());

      dtCore::RefPtr<dtGame::GameActor> actor = &gap->GetGameActor();
      dtCore::ObserverPtr<dtGame::GameActorProxy> gapObserver(gap.get());
      dtCore::ObserverPtr<dtGame::GameActorProxy> gapOb2(gapObserver);
      gap = NULL;

      CPPUNIT_ASSERT_MESSAGE("Nothing should be holding onto the proxy, so the observer should be NULL", !gapObserver.valid());
      ///This should throw an exception because the proxy should have been cleaned up.
      CPPUNIT_ASSERT_THROW(actor->GetGameActorProxy(), dtUtil::Exception);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
//   catch (const std::exception& e)
//   {
//      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
//   }
}

void GameActorTests::TestInvokables()
{
   try
   {
      dtCore::RefPtr<const dtDAL::ActorType> actorType = mManager->FindActorType("ExampleActors", "Test1Actor");

      dtCore::RefPtr<dtDAL::ActorProxy> proxy = mManager->CreateActor(*actorType);
      dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());

      CPPUNIT_ASSERT_MESSAGE("ActorProxy should not be NULL", gap != NULL);

      std::vector<dtGame::Invokable*> toFill;
      gap->GetInvokables(toFill);
      CPPUNIT_ASSERT_MESSAGE("The actor should have at least 2 invokables",toFill.size() >= 5);
      dtGame::Invokable* iF = gap->GetInvokable("Fire One");
      dtGame::Invokable* iR = gap->GetInvokable("Reset");
      CPPUNIT_ASSERT_MESSAGE("The actor should have an invokable named Fire One ",iF != NULL);
      CPPUNIT_ASSERT_MESSAGE("The actor should have an invokable named Reset ",iR != NULL);

      dtDAL::BooleanActorProperty* prop = NULL;
      gap->GetProperty("Has Fired", prop);

      CPPUNIT_ASSERT_MESSAGE("The actor should have a property named \"Has Fired\"", prop != NULL);

      CPPUNIT_ASSERT_MESSAGE("Property \"Has Fired\" should be false", !prop->GetValue());

      //need a dummy message
      dtCore::RefPtr<dtGame::Message> message = mManager->GetMessageFactory().CreateMessage(dtGame::MessageType::TICK_LOCAL);

      iF->Invoke(*message);

      CPPUNIT_ASSERT_MESSAGE("Property \"Has Fired\" should be true", prop->GetValue());

      iR->Invoke(*message);

      CPPUNIT_ASSERT_MESSAGE("Property \"Has Fired\" should be false", !prop->GetValue());

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

void GameActorTests::TestInvokableMessageRegistration()
{
   try
   {
      dtCore::RefPtr<const dtDAL::ActorType> actorType = mManager->FindActorType("ExampleActors", "Test1Actor");

      dtCore::RefPtr<dtDAL::ActorProxy> proxy = mManager->CreateActor(*actorType);
      dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());

      CPPUNIT_ASSERT_MESSAGE("ActorProxy should not be NULL", gap != NULL);

      dtGame::Invokable* iToggle = gap->GetInvokable("Toggle Ticks");

      CPPUNIT_ASSERT_MESSAGE("The actor should have an invokable named Toggle Ticks ",iToggle != NULL);

      std::vector<dtGame::Invokable*> toFill1, toFill2;

      gap->GetMessageHandlers(dtGame::MessageType::TICK_LOCAL, toFill1);
      gap->GetMessageHandlers(dtGame::MessageType::TICK_REMOTE, toFill2);

      CPPUNIT_ASSERT_MESSAGE("There should be one handler for tick local", toFill1.size() == 1);
      CPPUNIT_ASSERT_MESSAGE("The one handler should be Tick Local", toFill1[0]->GetName() == "Tick Local");
      CPPUNIT_ASSERT_MESSAGE("There should be one handler for tick remote", toFill2.size() == 1);
      CPPUNIT_ASSERT_MESSAGE("The one handler should be Tick Remote", toFill2[0]->GetName() == "Tick Remote");

      //need a dummy message
      dtCore::RefPtr<dtGame::Message> message = mManager->GetMessageFactory().CreateMessage(dtGame::MessageType::TICK_LOCAL);

      //this will remove the invokables registration.
      iToggle->Invoke(*message);

      gap->GetMessageHandlers(dtGame::MessageType::TICK_LOCAL, toFill1);
      gap->GetMessageHandlers(dtGame::MessageType::TICK_REMOTE, toFill2);

      CPPUNIT_ASSERT_MESSAGE("There should be one handler for tick local", toFill1.size() == 0);
      CPPUNIT_ASSERT_MESSAGE("There should be one handler for tick remote", toFill2.size() == 0);

      iToggle->Invoke(*message);

      //now to actually fire them...
      //need a dummy message
      message = mManager->GetMessageFactory().CreateMessage(dtGame::MessageType::TICK_LOCAL);
      //so it will reach the actor
      message->SetAboutActorId(gap->GetId());
      //this will remove the invokables registration.
      mManager->AddActor(*gap, false, false);
      mManager->SendMessage(*message);

      CPPUNIT_ASSERT_MESSAGE("Zero local ticks should have been received.", static_cast<dtDAL::IntActorProperty*>(gap->GetProperty("Local Tick Count"))->GetValue() == 0);
      CPPUNIT_ASSERT_MESSAGE("Zero remote ticks should have been received.", static_cast<dtDAL::IntActorProperty*>(gap->GetProperty("Remote Tick Count"))->GetValue() == 0);

      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_MESSAGE("One local tick should have been received.", static_cast<dtDAL::IntActorProperty*>(gap->GetProperty("Local Tick Count"))->GetValue() == 1);
      CPPUNIT_ASSERT_MESSAGE("Zero remote ticks should have been received.", static_cast<dtDAL::IntActorProperty*>(gap->GetProperty("Remote Tick Count"))->GetValue() == 0);

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

void GameActorTests::TestDefaultProcessMessageRegistration()
{
   try
   {
      dtCore::RefPtr<const dtDAL::ActorType> actor1Type = mManager->FindActorType("ExampleActors", "Test1Actor");
      dtCore::RefPtr<const dtDAL::ActorType> actor2Type = mManager->FindActorType("ExampleActors", "Test2Actor");

      dtCore::RefPtr<dtDAL::ActorProxy> proxy1 = mManager->CreateActor(*actor1Type);
      dtCore::RefPtr<dtGame::GameActorProxy> gap1 = dynamic_cast<dtGame::GameActorProxy*>(proxy1.get());

      dtCore::RefPtr<dtDAL::ActorProxy> proxy2 = mManager->CreateActor(*actor2Type);
      dtCore::RefPtr<dtGame::GameActorProxy> gap2 = dynamic_cast<dtGame::GameActorProxy*>(proxy2.get());

      CPPUNIT_ASSERT_MESSAGE("ActorProxy should not be NULL", gap1 != NULL);
      CPPUNIT_ASSERT_MESSAGE("ActorProxy should not be NULL", gap2 != NULL);

      // Make sure the invokable was created
      dtGame::Invokable* iTestListener = gap2->GetInvokable(dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE);
      CPPUNIT_ASSERT_MESSAGE("The actor should have an invokable named \"Process Message\"", iTestListener != NULL);

      mManager->AddActor(*gap1, false, false);
      mManager->AddActor(*gap2, false, false);

      // Make sure we can use proxy method to register for messages
      gap2->RegisterForMessages(dtGame::MessageType::INFO_ACTOR_PUBLISHED, dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE);
      std::vector< std::pair<dtGame::GameActorProxy*, std::string> > toFill;
      mManager->GetRegistrantsForMessages(dtGame::MessageType::INFO_ACTOR_PUBLISHED, toFill);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be one registered game actor listener for the actor published message",
         (unsigned int) 1, (unsigned int) toFill.size());

      // And unregister them too
      gap2->UnregisterForMessages(dtGame::MessageType::INFO_ACTOR_PUBLISHED, dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE);
      mManager->GetRegistrantsForMessages(dtGame::MessageType::INFO_ACTOR_PUBLISHED, toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be zero registered game actor listener for the actor published message", toFill.size() == 0);

      // now use the Register For Self behavior - they only go on the actor itself
      gap2->RegisterForMessagesAboutSelf(dtGame::MessageType::INFO_ACTOR_PUBLISHED,
         dtGame::GameActorProxy::PROCESS_MSG_INVOKABLE);

      // There shouldnt be any listeners on the GM
      mManager->GetRegistrantsForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap2->GetId(), toFill);
      CPPUNIT_ASSERT_MESSAGE("There should not be any listeners for the actor published message this time", toFill.size() == 0);
      mManager->GetRegistrantsForMessages(dtGame::MessageType::INFO_ACTOR_PUBLISHED, toFill);
      CPPUNIT_ASSERT_MESSAGE("There should not be any listeners for the actor published message this time", toFill.size() == 0);

      mManager->PublishActor(*gap1);
      mManager->PublishActor(*gap2);

      dtCore::System::GetInstance().Step();

      // One publish message should have been received. So, count should be 2
      CPPUNIT_ASSERT_EQUAL_MESSAGE("We should only have gotten 1 publish, so count should be 2.",
         int(2), static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Actor Published Count"))->GetValue());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

void GameActorTests::TestGlobalInvokableMessageRegistrationEndOfFrame()
{
   try
   {
      dtCore::RefPtr<const dtDAL::ActorType> actor1Type = mManager->FindActorType("ExampleActors", "Test2Actor");

      dtCore::RefPtr<dtGame::GameActorProxy> gap1;
      mManager->CreateActor(*actor1Type, gap1);

      mManager->AddActor(*gap1, false, false);

      dtGame::Invokable* iTestListener = gap1->GetInvokable("Test Message Listener");

      CPPUNIT_ASSERT_THROW(
               mManager->RegisterForMessages(dtGame::MessageType::TICK_END_OF_FRAME, *gap1, iTestListener->GetName()),
               dtUtil::Exception);

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

void GameActorTests::TestGlobalInvokableMessageRegistration()
{
   try
   {
      dtCore::RefPtr<const dtDAL::ActorType> actor1Type = mManager->FindActorType("ExampleActors", "Test1Actor");
      dtCore::RefPtr<const dtDAL::ActorType> actor2Type = mManager->FindActorType("ExampleActors", "Test2Actor");

      dtCore::RefPtr<dtGame::GameActorProxy> gap1;
      mManager->CreateActor(*actor1Type, gap1);

      dtCore::RefPtr<dtGame::GameActorProxy> gap2;
      mManager->CreateActor(*actor2Type, gap2);

      CPPUNIT_ASSERT_MESSAGE("ActorProxy should not be NULL", gap1 != NULL);
      CPPUNIT_ASSERT_MESSAGE("ActorProxy should not be NULL", gap2 != NULL);

      dtGame::Invokable* iTestListener = gap2->GetInvokable("Test Message Listener");

      CPPUNIT_ASSERT_MESSAGE("The actor should have an invokable named \"Test Message Listener\"",iTestListener != NULL);

      mManager->AddActor(*gap1, false, false);
      mManager->AddActor(*gap2, false, false);

      mManager->RegisterForMessages(dtGame::MessageType::INFO_MAP_LOADED, *gap2, iTestListener->GetName());
      mManager->RegisterForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED,
            gap1->GetId(), *gap2, iTestListener->GetName());

      std::vector< std::pair<dtGame::GameActorProxy*, std::string> > toFill;

      mManager->GetRegistrantsForMessages(dtGame::MessageType::INFO_MAP_LOADED, toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be one registered global listener for the Map Loaded message",
            toFill.size() == 1);

      mManager->GetRegistrantsForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED,
            gap1->GetId(), toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be one registered game actor listener for the actor published message",
            toFill.size() == 1);

      mManager->UnregisterForMessages(dtGame::MessageType::INFO_MAP_LOADED,
            *gap2, iTestListener->GetName());
      mManager->UnregisterForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED,
            gap1->GetId(), *gap2, iTestListener->GetName());

      mManager->GetRegistrantsForMessages(dtGame::MessageType::INFO_MAP_LOADED, toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be zero registered global listener for the Map Loaded message",
            toFill.size() == 0);

      mManager->GetRegistrantsForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be zero registered game actor listener for the actor published message",
            toFill.size() == 0);

      mManager->RegisterForMessages(dtGame::MessageType::INFO_MAP_LOADED, *gap2, iTestListener->GetName());
      mManager->RegisterForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED,
            gap1->GetId(), *gap2, iTestListener->GetName());

      //std::cout << gap1->GetId() << std::endl;

      mManager->GetRegistrantsForMessages(dtGame::MessageType::INFO_MAP_LOADED, toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be one registered global listener for the Map Loaded message",
            toFill.size() == 1);

      mManager->GetRegistrantsForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED,
            gap1->GetId(), toFill);
      CPPUNIT_ASSERT_MESSAGE("There should be one registered game actor listener for the actor published message",
            toFill.size() == 1);

      mManager->PublishActor(*gap1);
      mManager->SendMessage(*mManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_MAP_LOADED));
      mManager->DeleteActor(*gap1);

      //actors are not removed immediately
      CPPUNIT_ASSERT(mManager->FindGameActorById(gap1->GetId()) != NULL);

      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      //Actor should be removed by now.
      CPPUNIT_ASSERT(mManager->FindGameActorById(gap1->GetId()) == NULL);

      //A publish and map loaded message should have been received, but no listener was added for delete.
      CPPUNIT_ASSERT_EQUAL(0, static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Actor Deleted Count"))->GetValue());
      CPPUNIT_ASSERT_EQUAL(1, static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Actor Published Count"))->GetValue());
      CPPUNIT_ASSERT_EQUAL(1, static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Map Loaded Count"))->GetValue());

      dtCore::RefPtr<dtGame::GameActorProxy> gap3;
      mManager->CreateActor(*actor1Type, gap3);

      //add global and actor-specific delete listeners
      mManager->RegisterForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_DELETED,
            gap3->GetId(), *gap2, iTestListener->GetName());
      mManager->RegisterForMessages(dtGame::MessageType::INFO_ACTOR_DELETED, *gap2, iTestListener->GetName());
      //take off the publish listener so that when the message shows up, it will not be passed on.
      mManager->UnregisterForMessagesAboutActor(dtGame::MessageType::INFO_ACTOR_PUBLISHED, gap1->GetId(), *gap2, iTestListener->GetName());

      //add and publish the actor.
      mManager->AddActor(*gap3, false, true);
      //delete it
      mManager->DeleteActor(*gap3);
      //add a load map message again.
      mManager->SendMessage(*mManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_MAP_LOADED));

      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Actor Deleted Count"))->GetValue() == 2);
      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Actor Published Count"))->GetValue() == 1);
      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Map Loaded Count"))->GetValue() == 2);

      //test removing the map loaded message and then send one
      mManager->UnregisterForMessages(
         dtGame::MessageType::INFO_MAP_LOADED, *gap2, iTestListener->GetName());
      mManager->SendMessage(*mManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_MAP_LOADED));
      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Actor Deleted Count"))->GetValue() == 2);
      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Actor Published Count"))->GetValue() == 1);
      CPPUNIT_ASSERT(static_cast<dtDAL::IntActorProperty*>(gap2->GetProperty("Map Loaded Count"))->GetValue() == 2);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

void GameActorTests::TestAddRemoveFromEnvActor()
{
   try
   {
      dtCore::RefPtr<const dtDAL::ActorType> type = mManager->FindActorType("ExampleActors", "TestEnvironmentActor");
      CPPUNIT_ASSERT_MESSAGE("Should have been able to find the test environment actor in the test game library", type.valid());
      dtCore::RefPtr<dtDAL::ActorProxy> ap = mManager->CreateActor(*type);
      CPPUNIT_ASSERT_MESSAGE("The game manager should have been able to create the test environment actor", ap.valid());

      dtCore::RefPtr<TestGameEnvironmentActorProxy> eap = dynamic_cast<TestGameEnvironmentActorProxy*>(ap.get());
      CPPUNIT_ASSERT_MESSAGE("The dynamic cast should not have returned NULL", eap != NULL);
      dtCore::RefPtr<TestGameEnvironmentActor> ea = dynamic_cast<TestGameEnvironmentActor*>(eap->GetActor());
      CPPUNIT_ASSERT_MESSAGE("Should have been able to cast the environment proxy's actor to an environment actor", ea != NULL);

      // SET ENVIRONMENT ACTOR TESTS
      mManager->SetEnvironmentActor(eap.get());
      //One cannot enable paging without a window.
      dtCore::System::GetInstance().Step();

      // --- TEST GM REMOVAL --- START --- //
      // Re-insert environment actor for this sub-test
      mManager->SetEnvironmentActor(eap.get());
      // Add actor 1
      ap = mManager->CreateActor(*type);
      mManager->AddActor(*ap);
      // Add actor 2
      dtCore::RefPtr<dtDAL::ActorProxy> ap2 = mManager->CreateActor(*type);
      mManager->AddActor(*ap2);

      osg::observer_ptr<dtDAL::ActorProxy> apObserver1 = ap.get();
      osg::observer_ptr<dtDAL::ActorProxy> apObserver2 = ap2.get();

      // Check GM count
      CPPUNIT_ASSERT_MESSAGE("The game manager should have 2 actors in its scene",
         ea->GetNumEnvironmentChildren() == 2);

      // Remove actor 2
      ea->RemoveActor(*ap2->GetActor());
      CPPUNIT_ASSERT_MESSAGE("The actor 2 should be removed from the scene", ! ea->ContainsActor(*ap2->GetActor()) );
      CPPUNIT_ASSERT_MESSAGE("The game manager should have 1 actor in its scene",
         ea->GetNumEnvironmentChildren() == 1);

      // Add actor 2 as child to actor 1
      ap->GetActor()->AddChild(ap2->GetActor());

      // Delete actor 1 from GM
      mManager->DeleteActor(*ap);
      dtCore::System::GetInstance().Step();

      // Ensure environment contains actor 2.
      CPPUNIT_ASSERT_MESSAGE("The environment actor should have actor 2 as a child", ea->ContainsActor(*ap2->GetActor()) );

      std::vector<dtCore::DeltaDrawable*> actors;
      std::vector<dtCore::DeltaDrawable*> drawables;

      // Ensure actor 2 is also returned in the list of actors returned by the environment.
      bool foundActor = false;
      ea->GetAllActors(actors);
      for (unsigned int i = 0; i < actors.size(); ++i)
      {
         if (ap2->GetId() == actors[i]->GetUniqueId())
         {
            foundActor = true;
            break;
         }
      }
      actors.clear();
      CPPUNIT_ASSERT_MESSAGE("The environment actor should return actor 2 in the list returned from GetAllActors().", foundActor );


      // Remove actor 2 so no actors are in the GM for the next section of this test function.
      mManager->DeleteActor(*ap2);
      dtCore::System::GetInstance().Step();

      // Ensure environment has removed actor 2.
      CPPUNIT_ASSERT_MESSAGE("The environment actor should have removed actor 2", ! ea->ContainsActor(*ap2->GetActor()) );

      // Ensure all actors are removed from the environment
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT(actors.empty());

      ap  = NULL;
      ap2 = NULL;
      CPPUNIT_ASSERT_MESSAGE("Actor 1 should have been deleted.", !apObserver1.valid());
      CPPUNIT_ASSERT_MESSAGE("Actor 2 should have been deleted.", !apObserver2.valid());

      mManager->DeleteAllActors(true);
      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT(drawables.empty());
      // --- TEST GM REMOVAL --- END --- //
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

void GameActorTests::TestSetEnvironmentActor()
{
   try
   {
      dtCore::RefPtr<dtCore::View> view = new dtCore::View;
      view->SetScene(&mManager->GetScene());
      //One cannot enable paging without a window.
      //mManager->GetScene().EnablePaging();
      //CPPUNIT_ASSERT(mManager->GetScene().IsPagingEnabled());
      dtCore::RefPtr<TestComponent> tc = new TestComponent("name");
      CPPUNIT_ASSERT(tc.valid());
      mManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::NORMAL);
      dtCore::RefPtr<const dtDAL::ActorType> type = mManager->FindActorType("ExampleActors", "TestEnvironmentActor");
      CPPUNIT_ASSERT_MESSAGE("Should have been able to find the test environment actor in the test game library", type.valid());
      dtCore::RefPtr<dtDAL::ActorProxy> ap = mManager->CreateActor(*type);
      CPPUNIT_ASSERT_MESSAGE("The game manager should have been able to create the test environment actor", ap.valid());

      dtCore::RefPtr<TestGameEnvironmentActorProxy> eap = dynamic_cast<TestGameEnvironmentActorProxy*>(ap.get());
      CPPUNIT_ASSERT_MESSAGE("The dynamic cast should not have returned NULL", eap != NULL);
      dtCore::RefPtr<TestGameEnvironmentActor> ea = dynamic_cast<TestGameEnvironmentActor*>(eap->GetActor());
      CPPUNIT_ASSERT_MESSAGE("Should have been able to cast the environment proxy's actor to an environment actor", ea != NULL);

      std::vector<dtCore::DeltaDrawable*> actors;
      std::vector<dtCore::DeltaDrawable*> drawables;
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT(actors.empty());

      type = mManager->FindActorType("ExampleActors", "TestPlayer");
      CPPUNIT_ASSERT_MESSAGE("Should have been able to find the test player actor in the game manager", type.valid());

      const unsigned int numActors = 20;
      for (unsigned int i = 0; i < numActors; ++i)
      {
         ap = mManager->CreateActor(*type);
         CPPUNIT_ASSERT_MESSAGE("The game manager should be able to create the test player actor", ap.valid());
         mManager->AddActor(*ap);
      }

      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The game manager should have the correct number of actors in it",
            numActors, unsigned(drawables.size() - 1));

      // SET ENVIRONMENT ACTOR TESTS
      mManager->SetEnvironmentActor(eap.get());
      dtCore::System::GetInstance().Step();
      std::vector<dtCore::RefPtr<const dtGame::Message> > msgs = tc->GetReceivedProcessMessages();
      bool wasMessage = false;
      for (unsigned int i = 0; i < msgs.size(); ++i)
      {
         if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_ENVIRONMENT_CHANGED)
         {
            wasMessage = true;
         }
      }
      CPPUNIT_ASSERT_MESSAGE("An environment change message should have been sent", wasMessage);
      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT_MESSAGE("An environment actor was added.  The game manager should only have "
         " the environment actor and the default scene light.", drawables.size() == 2);
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("The environment actor should now have the number of actors in it", actors.size() == numActors);
      mManager->SetEnvironmentActor(NULL);
      //CPPUNIT_ASSERT(mManager->GetScene().IsPagingEnabled());
      //mManager->GetScene().DisablePaging();
      msgs.clear();
      tc->reset();
      dtCore::System::GetInstance().Step();
      msgs = tc->GetReceivedProcessMessages();
      wasMessage = false;
      for (unsigned int i = 0; i < msgs.size(); ++i)
      {
         if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_ENVIRONMENT_CHANGED)
         {
            wasMessage = true;
         }
      }
      CPPUNIT_ASSERT_MESSAGE("An environment change message should NOT have been sent", !wasMessage);
      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT_MESSAGE("Set the environment actor to NULL. The game manager should now have the correct "
            "number of actors in it", drawables.size()-1 == numActors);
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("The environment actor should not have any actors in it", actors.empty());
      mManager->SetEnvironmentActor(NULL);
      //CPPUNIT_ASSERT(!mManager->GetScene().IsPagingEnabled());
      msgs.clear();
      tc->reset();
      dtCore::System::GetInstance().Step();
      msgs = tc->GetReceivedProcessMessages();
      wasMessage = false;
      for (unsigned int i = 0; i < msgs.size(); ++i)
      {
         if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_ENVIRONMENT_CHANGED)
         {
            wasMessage = true;
         }
      }
      CPPUNIT_ASSERT_MESSAGE("An environment change message should NOT have been sent, it was a no-op", !wasMessage);
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("Setting the environment from NULL to NULL should not have done anything", actors.empty());
      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT_MESSAGE("Setting the environment from NULL to NULL should not have done anything", drawables.size()-1 == numActors);

      type = mManager->FindActorType("ExampleActors", "TestEnvironmentActor");
      CPPUNIT_ASSERT(type.valid());
      ap = mManager->CreateActor(*type);
      CPPUNIT_ASSERT(ap.valid());
      dtCore::RefPtr<dtGame::IEnvGameActorProxy> eap2 = dynamic_cast<dtGame::IEnvGameActorProxy*>(ap.get());
      CPPUNIT_ASSERT(eap2 != NULL);
      dtCore::RefPtr<dtGame::IEnvGameActor>      ea2  = dynamic_cast<dtGame::IEnvGameActor*>(eap2->GetActor());
      CPPUNIT_ASSERT(ea2 != NULL);
      mManager->SetEnvironmentActor(eap.get());
      //One cannot enable paging without a window.
      //CPPUNIT_ASSERT(!mManager->GetScene().IsPagingEnabled());
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("Once again, the environment actor should now have all the actors", actors.size() == numActors);
      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT_MESSAGE("Once again, the game manager should have no actors in the scene except the environment actor", drawables.size() == 2);
      //One cannot enable paging without a window.
      //mManager->GetScene().EnablePaging();
      mManager->SetEnvironmentActor(eap2.get());
      //CPPUNIT_ASSERT(mManager->GetScene().IsPagingEnabled());
      msgs.clear();
      tc->reset();
      dtCore::System::GetInstance().Step();
      msgs = tc->GetReceivedProcessMessages();
      wasMessage = false;
      for (unsigned int i = 0; i < msgs.size(); i++)
      {
         if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_ENVIRONMENT_CHANGED)
         {
            wasMessage = true;
         }
      }
      CPPUNIT_ASSERT_MESSAGE("An environment change message should have been sent", wasMessage);
      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT_MESSAGE("Setting the environment actor on the game manager from one to another should NOT add any actors to the game manager", drawables.size() == 2);
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("The environment actor was set to another, this environment actor should NOT have any actors", actors.empty());
      ea2->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("Since this environment actor was set, it should now have all the actors", actors.size() == numActors);
      mManager->SetEnvironmentActor(NULL);
      //One cannot enable paging without a window.
      //CPPUNIT_ASSERT(mManager->GetScene().IsPagingEnabled());
      msgs.clear();
      tc->reset();
      dtCore::System::GetInstance().Step();
      msgs = tc->GetReceivedProcessMessages();
      wasMessage = false;
      for (unsigned int i = 0; i < msgs.size(); ++i)
      {
         if (msgs[i]->GetMessageType() == dtGame::MessageType::INFO_ENVIRONMENT_CHANGED)
         {
            wasMessage = true;
         }
      }
      CPPUNIT_ASSERT_MESSAGE("An environment change message should NOT have been sent", !wasMessage);
      ea->GetAllActors(actors);
      CPPUNIT_ASSERT(actors.empty());
      ea2->GetAllActors(actors);
      CPPUNIT_ASSERT_MESSAGE("The previous environment actor should not have any actors", actors.empty());
      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT_MESSAGE("The game manager should now have all the actors", drawables.size() - 1 == numActors);
      mManager->DeleteAllActors(true);
      mManager->GetActorsInScene(drawables);
      CPPUNIT_ASSERT(drawables.empty());
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
   //catching std::exception messes up the CPPUNIT failure exceptions.
   //catch(const std::exception &e)
   //{
   //   std::string cause = std::string("std::exception of type: ") + typeid(e).name() + std::string(" caught. Message is: ") + e.what();
   //   CPPUNIT_FAIL(cause);
   //}
}

void GameActorTests::TestStaticGameActorTypes()
{
   const unsigned int size = 8;
   dtCore::RefPtr<dtDAL::ActorType> types[size] =
   {
      TestGameActorLibrary::TEST1_GAME_ACTOR_PROXY_TYPE,
      TestGameActorLibrary::TEST2_GAME_ACTOR_PROXY_TYPE,
      TestGameActorLibrary::TEST_PLAYER_GAME_ACTOR_PROXY_TYPE,
      TestGameActorLibrary::TEST_TASK_GAME_ACTOR_PROXY_TYPE,
      TestGameActorLibrary::TEST_COUNTER_TASK_GAME_ACTOR_PROXY_TYPE,
      TestGameActorLibrary::TEST_TANK_GAME_ACTOR_PROXY_TYPE,
      TestGameActorLibrary::TEST_JET_GAME_ACTOR_PROXY_TYPE,
      TestGameActorLibrary::TEST_HELICOPTER_GAME_ACTOR_PROXY_TYPE/*,
      TestGameActorLibrary::TEST_ENVIRONMENT_GAME_ACTOR_PROXY_TYPE*/
   };
   for (unsigned int i = 0; i < size; ++i)
   {
      dtCore::RefPtr<dtDAL::ActorProxy> proxy = mManager->CreateActor(*types[i]);
      CPPUNIT_ASSERT_MESSAGE("The proxy should not be NULL", proxy.valid());
      CPPUNIT_ASSERT_MESSAGE("The proxy's actor should not be NULL", proxy->GetActor() != NULL);
   }
}

void GameActorTests::TestEnvironmentTimeConversions()
{
   dtCore::RefPtr<dtDAL::ActorProxy> proxy = mManager->CreateActor(*TestGameActorLibrary::TEST_ENVIRONMENT_GAME_ACTOR_PROXY_TYPE);
   CPPUNIT_ASSERT(proxy.valid());
   dtCore::RefPtr<TestGameEnvironmentActor> envActor = dynamic_cast<TestGameEnvironmentActor*>(proxy->GetActor());
   CPPUNIT_ASSERT(envActor.valid());
   std::string testTime = "2006-04-20T18:04:09";
   envActor->SetTimeAndDateString(testTime);
   int year, month, day, hour, min, sec;
   envActor->GetTimeAndDate(year, month, day, hour, min, sec);
   CPPUNIT_ASSERT_MESSAGE("The year should be correct", year == 2006);
   CPPUNIT_ASSERT_MESSAGE("The month should be correct", month == 4);
   CPPUNIT_ASSERT_MESSAGE("The day should be correct", day == 20);
   CPPUNIT_ASSERT_MESSAGE("The hour should be correct", hour == 18);
   CPPUNIT_ASSERT_MESSAGE("The minute should be correct", min == 4);
   CPPUNIT_ASSERT_MESSAGE("The seconds should be close to correct", osg::equivalent(float(sec), 9.0f, 1.0f));

   std::string newValue = envActor->GetTimeAndDateString();
   CPPUNIT_ASSERT_MESSAGE("The time and date string should be equal to the original value", newValue == testTime);
}

void GameActorTests::TestMessageProcessingPerformance()
{
   int numActors = 20;
   int numTicks = 42;
   try
   {
      dtDAL::Project::GetInstance().CreateContext("Working Project");
      dtDAL::Project::GetInstance().SetContext("Working Project");
      dtCore::RefPtr<dtGame::DefaultMessageProcessor> dmc = new dtGame::DefaultMessageProcessor();
      mManager->AddComponent(*dmc, dtGame::GameManager::ComponentPriority::HIGHEST);

      dtCore::RefPtr<const dtDAL::ActorType> actor1Type = mManager->FindActorType("ExampleActors", "TestGamePropertyProxy");

      // Start time in microseconds
      dtCore::Timer_t startTime(0);// = mManager->GetRealClockTime();
      dtCore::Timer statsTickClock;
      //Timer_t frameTickStart(0);
      startTime = statsTickClock.Tick();

      for (int i = 0; i < numActors; ++i)
      {
         dtCore::RefPtr<TestGamePropertyProxy> proxy1;
         mManager->CreateActor(*actor1Type, proxy1);
         //dtCore::RefPtr<dtGame::TestGamePropertyProxy> gap1 = dynamic_cast<dtGame::TestGamePropertyProxy*>(proxy1.get());
         CPPUNIT_ASSERT_MESSAGE("ActorProxy should not be NULL", proxy1 != NULL);

         //proxy1->GetProperty(dtDAL::TransformableActorProxy::PROPERTY_ROTATION)->SetReadOnly(true);
         //proxy1->GetProperty(dtDAL::TransformableActorProxy::PROPERTY_TRANSLATION)->SetReadOnly(true);
         //proxy1->GetProperty(dtDAL::TransformableActorProxy::PROPERTY_SCALE)->SetReadOnly(true);
         proxy1->SetRegisterListeners(false);
         // add it as a remote actor
         mManager->AddActor(*proxy1, true, false);
      }

      dtCore::System::GetInstance().Step();

      std::vector<dtDAL::ActorProxy*> testProxies;
      mManager->FindActorsByType(*actor1Type, testProxies);

      // loop multiple ticks.
      for (int tickCounter = 0; tickCounter < numTicks; ++tickCounter)
      {
         // loop through the TON of actors (38 properties each)
         for (unsigned int actorIndex = 0; actorIndex < testProxies.size(); ++actorIndex)
         {
            // get one of the actors
            dtCore::RefPtr<dtDAL::ActorProxy> actorProxy = testProxies[actorIndex];
            dtCore::RefPtr<TestGamePropertyProxy> propProxy =
               dynamic_cast<TestGamePropertyProxy*>(actorProxy.get());

            // create and populate an actor update message with ALL properties for this actor
            dtCore::RefPtr<dtGame::ActorUpdateMessage> updateMsg;
            mManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED, updateMsg);
            propProxy->PopulateActorUpdate(*updateMsg);
            mManager->SendMessage(*updateMsg);
         }

         dtCore::System::GetInstance().Step();
      }

      dtUtil::FileUtils::GetInstance().DirDelete("Working Project", true);
      CPPUNIT_ASSERT(!dtUtil::FileUtils::GetInstance().DirExists("Working Project"));

      // Start time in microseconds
      dtCore::Timer_t stopTime = statsTickClock.Tick();
      double timeDelta = statsTickClock.DeltaSec(startTime, stopTime);
      //Timer_t stopTime = mManager->GetRealClockTime();

      // 1 second???
      std::ostringstream ss;
      ss << "Update Msgs for " << numActors << " actors, 38 props, " << numTicks << " ticks took - [" << timeDelta << "] seconds " << std::endl;
      //CPPUNIT_ASSERT_MESSAGE(ss.str(), timeDelta < 10.0);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////
void GameActorTests::TestOnRemovedActor()
{
   // This only tests that the new OnRemovedFromWorld method is called
   // Normal delete actor tests are done elsewhere.

   dtCore::RefPtr<TestGamePropertyProxy> proxy1;
   dtCore::RefPtr<const dtDAL::ActorType> actor1Type = mManager->FindActorType("ExampleActors", "TestGamePropertyProxy");
   mManager->CreateActor(*actor1Type, proxy1);
   mManager->AddActor(*proxy1, true, false);

   dtCore::System::GetInstance().Step();

   CPPUNIT_ASSERT_MESSAGE("Proxy should NOT be marked as 'RemovedFromWorld'", !proxy1->IsRemovedFromWorld());

   mManager->DeleteActor(*proxy1);
   dtCore::System::GetInstance().Step();

   CPPUNIT_ASSERT_MESSAGE("Proxy should BE marked as 'RemovedFromWorld'", proxy1->IsRemovedFromWorld());
}

//////////////////////////////////////////////////////
void GameActorTests::TestAddActorComponent()
{
   try
   {
      dtCore::RefPtr<const dtDAL::ActorType> actorType = mManager->FindActorType("ExampleActors", "Test1Actor");
      dtCore::RefPtr<dtDAL::ActorProxy> proxy = mManager->CreateActor(*actorType);
      dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());

      dtGame::GameActor* actor = &gap->GetGameActor();

      dtCore::RefPtr<TestActorComponent1> component = new TestActorComponent1();
      actor->AddComponent(*component);

      CPPUNIT_ASSERT_MESSAGE("Actor owner not set", component->GetOwner() == actor);

      bool hascomp = actor->HasComponent(TestActorComponent1::TYPE);
      CPPUNIT_ASSERT_MESSAGE("Actor component not found after it was added!", hascomp);

      bool found = (actor->GetComponent(TestActorComponent1::TYPE) == component.get());
      CPPUNIT_ASSERT_MESSAGE("Could not retrieve actor component after it was added!", found);

      TestActorComponent1* compare;
      if(!actor->GetComponent(compare))
      {
         CPPUNIT_FAIL("Could not retrieve actor component after it was added");
      }
      bool foundtemplate = (compare == component);
      CPPUNIT_ASSERT_MESSAGE("Could not retrieve actor component after it was added!", foundtemplate);

      actor->RemoveComponent(TestActorComponent1::TYPE);

      try
      {
         bool notfound = (actor->GetComponent(TestActorComponent1::TYPE) == NULL);
         CPPUNIT_ASSERT_MESSAGE("Searching for actor component should throw exception!", notfound);
      }
      catch(const dtUtil::Exception&)
      {
      }
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
}



//////////////////////////////////////////////////////
void GameActorTests::TestActorComponentInitialized()
{
   try
   {
      dtCore::RefPtr<const dtDAL::ActorType> actorType = mManager->FindActorType("ExampleActors", "Test1Actor");
      dtCore::RefPtr<dtDAL::ActorProxy> proxy = mManager->CreateActor(*actorType);
      dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());
      dtGame::GameActor* actor = &gap->GetGameActor();

      dtCore::RefPtr<TestActorComponent1> component1 = new TestActorComponent1();
      actor->AddComponent(*component1);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("ActorComponent didn't get called when added to actor", true, component1->mWasAdded);


      mManager->AddActor(*gap, true, false);

      //component 1 should have entered the world now
      CPPUNIT_ASSERT_EQUAL_MESSAGE("ActorComponent didn't enter the world, after being added to the GM", true, component1->mEnteredWorld);

      dtCore::RefPtr<TestActorComponent2> component2 = new TestActorComponent2();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("ActorComponent shouldn't have been added to Actor yet", false, component2->mWasAdded);

      actor->AddComponent(*component2);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("ActorComponent didn't get added to an actor which is already in the world",true, component2->mWasAdded);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("ActorComponent didn't enter the world, after being added to an Actor already in the world", true, component2->mEnteredWorld);

      actor->RemoveComponent(*component2);
      CPPUNIT_ASSERT_MESSAGE("Actor component2 should have been removed from world when removed from actor!", component2->mLeftWorld);
      CPPUNIT_ASSERT_MESSAGE("Actor component2 should be de-initialized when removed from actor!", component2->mWasRemoved);

      CPPUNIT_ASSERT_MESSAGE("Actor component should not be removed yet!", !component1->mWasRemoved);
      mManager->DeleteActor(*gap);

      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_MESSAGE("Actor component should have left the world!", component1->mLeftWorld);

      // Make sure the actor is deleted
      gap = NULL;
      proxy = NULL;
      // Actor should be removed by now.
      CPPUNIT_ASSERT_MESSAGE("Actor component should have been removed when the actor is deleted from memory!", component1->mWasRemoved);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
}

////////////////////////////////////////////////////////////////////////////////
void GameActorTests::TestGetAllActorComponents()
{
   dtCore::RefPtr<const dtDAL::ActorType> actorType = mManager->FindActorType("ExampleActors", "Test1Actor");
   dtCore::RefPtr<dtDAL::ActorProxy> proxy = mManager->CreateActor(*actorType);
   dtCore::RefPtr<dtGame::GameActorProxy> gap = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());
   mManager->AddActor(*gap, true, false);
   dtGame::GameActor* actor = &gap->GetGameActor();


   std::vector<ActorComponent*> components;
   actor->GetAllComponents(components);
   const size_t startingSize = components.size();


   dtCore::RefPtr<TestActorComponent1> component1 = new TestActorComponent1();
   dtCore::RefPtr<TestActorComponent2> component2 = new TestActorComponent2();
   actor->AddComponent(*component1);
   actor->AddComponent(*component2);

   //wipe out any old remnants
   components = std::vector<ActorComponent*>();
   actor->GetAllComponents(components);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Actor didn't return back the number of added ActorComponents",
                                startingSize + 2, components.size());
}

////////////////////////////////////////////////////////////////////////////////
void GameActorTests::TestUnregisterNextInvokable()
{
   dtCore::RefPtr<const dtDAL::ActorType> actor1Type = mManager->FindActorType("ExampleActors", "Test1Actor");
   dtCore::RefPtr<const dtDAL::ActorType> actor2Type = mManager->FindActorType("ExampleActors", "Test1Actor");

   dtCore::RefPtr<dtDAL::ActorProxy> proxy1 = mManager->CreateActor(*actor1Type);
   dtCore::RefPtr<dtGame::GameActorProxy> gap1 = dynamic_cast<dtGame::GameActorProxy*>(proxy1.get());

   dtCore::RefPtr<dtDAL::ActorProxy> proxy2 = mManager->CreateActor(*actor2Type);
   dtCore::RefPtr<dtGame::GameActorProxy> gap2 = dynamic_cast<dtGame::GameActorProxy*>(proxy2.get());

   dtGame::Invokable* invokable1 = gap1->GetInvokable(dtGame::MessageType::TICK_LOCAL.GetName());
   dtGame::Invokable* invokable2 = gap2->GetInvokable(dtGame::MessageType::TICK_LOCAL.GetName());

   mManager->AddActor(*gap1, false, false);
   mManager->AddActor(*gap2, false, false);

   mManager->RegisterForMessages(dtGame::MessageType::TICK_LOCAL, *gap1, invokable1->GetName());
   mManager->RegisterForMessages(dtGame::MessageType::TICK_LOCAL, *gap2, invokable2->GetName());


   dtCore::System::GetInstance().Step();
}