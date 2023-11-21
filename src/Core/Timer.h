#pragma once
#include <chrono>
#include <string>
#include <list>

namespace TimerGui
{
   void Menu();
   void DrawGui();
}

class Timer
{
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
    protected:
        std::string mName;
        long double mLastTimeMs=0.0;
        bool mPrint;
        bool mEnabled; 

    public:
        Timer(const std::string& name="", bool print = true);
        ~Timer();
        virtual long double Stop();
        virtual void Restart();
        long double GetLastTimeMs() {return mLastTimeMs;}
        void SetPrint(bool print) {mPrint = print;}
        void SetEnabled(bool enable) { mEnabled = enable; }
        void SetName(const std::string& name) { mName = name; }

   static void SetEnabledAll(bool enabled);
   static void SetPrintAll(bool print);
   static std::list<Timer*>& sAllTimers();

   friend void TimerGui::Menu();
   friend void TimerGui::DrawGui();
};


class GpuTimer:public Timer
{
    private:
        unsigned int mQuery[2] = {0U-1U, 0U-1U};

    public:
        GpuTimer(const std::string& name="", bool print = true);
        ~GpuTimer();
        GpuTimer& operator=(const GpuTimer& t)
        {
           mQuery[0] = -1;
           mQuery[1] = -1;
           return *this;
        }
        GpuTimer(GpuTimer& t)
        {
           mQuery[0] = -1;
           mQuery[1] = -1;
        }

        long double Stop() override;
        void Restart() override;

      friend void TimerGui::Menu();
      friend void TimerGui::DrawGui();
};


class DoubleBufferedGpuTimer :public Timer
{
   private:
      unsigned int mStartQuery[2] = { 0U-1U, 0U-1U };
      unsigned int mEndQuery[2] = { 0U-1U, 0U-1U };
      void SwapBuffers();

   public:
      DoubleBufferedGpuTimer(const std::string& name = "", bool print = true);
      ~DoubleBufferedGpuTimer();
      DoubleBufferedGpuTimer& operator=(const DoubleBufferedGpuTimer& t)
      {
         mStartQuery[0] = -1;
         mStartQuery[1] = -1;
         mEndQuery[0] = -1;
         mEndQuery[1] = -1;
         return *this;
      }
      DoubleBufferedGpuTimer(DoubleBufferedGpuTimer& t)
      {
         mStartQuery[0] = -1;
         mStartQuery[1] = -1;
         mEndQuery[0] = -1;
         mEndQuery[1] = -1;
      }

      long double Stop() override;
      void Restart() override;

      friend void TimerGui::Menu();
      friend void TimerGui::DrawGui();
};