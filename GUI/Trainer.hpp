#pragma once

class Trainer
{
public:
   boost::signals2::signal<void()> onStoped;
   boost::signals2::signal<void(std::string)> onOutput;

   void start(NeatController& c)
   {
      mCtrl = &c;
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
      mStop = false;

      while(!mStop)
      {
         mCtrl->step();

         std::stringstream s;
         printState(*mCtrl, s);

         onOutput(s.str());
      }

      onStoped();
   }

   bool mStop = true;
   NeatController* mCtrl;
   std::thread mThread;
};