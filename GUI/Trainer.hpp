#pragma once
#include <boost/signals2.hpp>
#include <thread>
#include "NeatProject.hpp"

class Trainer
{
public:
   boost::signals2::signal<void()> signalStarted;
   boost::signals2::signal<void()> signalStopped;
   boost::signals2::signal<void()> signalStep;

   void start(NeatProject& s)
   {
      mSubject = &s;
      if(mThread.joinable())
      {
         mThread.join();
      }
      mThread = std::thread(&Trainer::threadFunc, this);
   }

   void stop()
   {
      mStop = true;
   }

   bool isRunning()
   {
      return !mStop;
   }

   ~Trainer()
   {
      if(mThread.joinable())
      {
         mThread.join();
      }
   }

private:
   void threadFunc()
   {
      signalStarted();

      mStop = false;

      while(!mStop)
      {
         mSubject->step();

         signalStep();
      }

      signalStopped();
   }

   bool mStop = true;
   NeatProject* mSubject;
   std::thread mThread;
};