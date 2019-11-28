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

   void start(NeatProject& s);
   void stop();

   bool isRunning();

   ~Trainer();

private:
   void threadFunc();

   bool mStop = true;
   NeatProject* mSubject;
   std::thread mThread;
};