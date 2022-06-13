#pragma once
#include <boost/signals2.hpp>
#include <thread>
#include "IProject.hpp"

class Trainer
{
public:
   boost::signals2::signal<void()> signalStarted;
   boost::signals2::signal<void()> signalStopped;
   boost::signals2::signal<void(std::chrono::milliseconds)> signalStep;

   void start(IProject& s);
   void stop();

   bool isRunning();

   ~Trainer();

private:
   void threadFunc();

   bool mStop = true;
   IProject* mSubject;
   std::thread mThread;
};