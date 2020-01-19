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
      auto t = std::chrono::system_clock::now();
      mSubject->step();
      auto t2 = std::chrono::system_clock::now();

      signalStep(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t));
   }

   signalStopped();
}