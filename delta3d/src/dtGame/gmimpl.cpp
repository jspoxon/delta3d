#include <dtGame/gmimpl.h>
#include <dtGame/basemessages.h>


//////////////////////////////////////////////////////////////////////////
void dtGame::GMImpl::ClearTimerSingleSet(std::set<TimerInfo> &timerSet, 
                                         const std::string &name,
                                         const dtGame::GameActorProxy *proxy)
{
   std::set<TimerInfo>::iterator i = timerSet.begin();
   while (i != timerSet.end())
   {
      std::set<TimerInfo>::iterator toDelete;
      const TimerInfo& timer = *i;
      if (timer.name == name &&  (proxy == NULL || timer.aboutActor == proxy->GetId()))
      {
         toDelete = i;
         ++i;
         timerSet.erase(toDelete);
      }
      else
      {
         ++i;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
dtGame::GMImpl::GMImpl(dtCore::Scene& scene) : mGMStatistics()
, mMachineInfo( new MachineInfo())
, mEnvironment(NULL)
//, mSendCreatesAndDeletes(true)
//, mAddActorsToScene(true)
, mFactory("GameManager MessageFactory", *mMachineInfo, "")
, mScene(&scene)
, mLibMgr(&dtDAL::LibraryManager::GetInstance())
, mApplication(NULL)
, mRemoveGameEventsOnMapChange(true)
, mLogger(&dtUtil::Log::GetInstance("gamemanager.cpp"))
{

}
////////////////////////////////////////////////////////////////////////////////
void dtGame::GMImpl::ProcessTimers(GameManager& gm, std::set<TimerInfo>& listToProcess, dtCore::Timer_t clockTime)
{
   std::set<TimerInfo>::iterator itor;
   std::set<TimerInfo> repeatingTimers;
   for (itor=listToProcess.begin(); itor!=listToProcess.end();)
   {
      if (itor->time <= clockTime)
      {
         dtCore::RefPtr<TimerElapsedMessage> timerMsg =
            static_cast<TimerElapsedMessage*>(mFactory.CreateMessage(MessageType::INFO_TIMER_ELAPSED).get());

         timerMsg->SetTimerName(itor->name);
         float lateTime = float((clockTime - itor->time));
         // convert from microseconds to seconds
         lateTime /= 1e6;
         timerMsg->SetLateTime(lateTime);
         timerMsg->SetAboutActorId(itor->aboutActor);
         gm.SendMessage(*timerMsg.get());
         if (itor->repeat)
         {
            TimerInfo tInfo = *itor;
            tInfo.time += tInfo.interval;
            repeatingTimers.insert(tInfo);
         }

         std::set<TimerInfo>::iterator toDelete = itor;
         ++itor;
         listToProcess.erase(toDelete);
      }
      else
      {
         break;
      }
   }

   // Repeating timers have to be readded so they are processed again later
   listToProcess.insert(repeatingTimers.begin(), repeatingTimers.end());
}

////////////////////////////////////////////////////////////////////////////////
void dtGame::GMImpl::RemoveActorFromScene(GameManager& gm, dtDAL::ActorProxy& proxy)
{
   dtCore::DeltaDrawable& dd = *proxy.GetActor();

   if (dd.GetSceneParent() != mScene.get())
   {
      return;
   }

   // find all of the children that have actor proxies associated with them to move them up
   // one level in the scene.
   std::vector< dtCore::RefPtr<dtDAL::ActorProxy> > childrenToMove;
   for (unsigned i = 0; i < dd.GetNumChildren(); ++i)
   {
      dtCore::DeltaDrawable& child = *dd.GetChild(i);
      dtDAL::ActorProxy* childProxy = gm.FindActorById(child.GetUniqueId());
      if (childProxy != NULL)
      {
         childrenToMove.push_back(childProxy);
      }
   }

   if (dd.GetParent() == NULL)
   {
      // remove the proxy drawable
      mScene->RemoveDrawable(&dd);

      // put all the children in the base scene.
      for (size_t i = 0; i < childrenToMove.size(); ++i)
      {
         mScene->AddDrawable(childrenToMove[i]->GetActor());
      }
   }
   else
   {
      // add all the children to the parent drawable.
      for (size_t i = 0; i < childrenToMove.size(); ++i)
      {
         dtCore::DeltaDrawable* child = childrenToMove[i]->GetActor();
         child->Emancipate();
         dd.GetParent()->AddChild(child);
      }
      // remove the proxy drawable from the parent.
      dd.Emancipate();
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtGame::GMImpl::SendEnvironmentChangedMessage(GameManager& gm, IEnvGameActorProxy* envActor)
{
   dtCore::RefPtr<Message> msg = mFactory.CreateMessage(MessageType::INFO_ENVIRONMENT_CHANGED);
   msg->SetAboutActorId(mEnvironment.valid() ? envActor->GetActor()->GetUniqueId() : dtCore::UniqueId(""));
   msg->SetSource(*mMachineInfo);
   gm.SendMessage(*msg);
}
