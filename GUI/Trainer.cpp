#include "Trainer.hpp"

void Trainer::start(IProject& s)
{
   mSubject = &s;
   if(mThread.joinable())
   {
      mThread.join();
   }
   mThread = std::thread(&Trainer::threadFunc, this);
}

void Trainer::stop()
{
   mStop = true;
}

bool Trainer::isRunning()
{
   return !mStop;
}

Trainer::~Trainer()
{
   if(mThread.joinable())
   {
      mThread.join();
   }
}

void Trainer::threadFunc()
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