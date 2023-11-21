#include "Timer.h"
#include <iostream>
#include <cassert>

typedef std::chrono::high_resolution_clock Clock;
using std::chrono::nanoseconds;
using std::chrono::duration_cast;

Timer::Timer(const std::string& name, bool print): mName(name), mPrint(print), mEnabled(true)
{
    sAllTimers().push_back(this);
    if(mEnabled==true)
    {
       mStart = Clock::now();
       if(mPrint && mName != "") std::cout << name << " start " << std::endl;
    }
}

Timer::~Timer()
{
   sAllTimers().remove(this);
}

std::list<Timer*>& Timer::sAllTimers()
{
   static std::list<Timer*> all_timers;
   return all_timers;
}

void Timer::SetEnabledAll(bool enable)
{
   for (Timer* t : sAllTimers())
   {
      t->SetEnabled(enable);
   }
}

void Timer::SetPrintAll(bool print)
{
   for (Timer* t : sAllTimers())
   {
      t->SetPrint(print);
   }
}

long double Timer::Stop()
{
    if (mEnabled == false) return -1.0;
    std::chrono::time_point<Clock> stop = Clock::now();
    auto diff = stop - mStart;
    nanoseconds ns = duration_cast<nanoseconds>(diff);
    mLastTimeMs = ns.count()*1e-6;
    long double time_elapsed_s = mLastTimeMs/1000.0;
    
    if (mPrint) std::cout << mName << " stop " << time_elapsed_s << " s " << "(" << mLastTimeMs << "ms)" << std::endl;
    return mLastTimeMs;
}

void Timer::Restart()
{
    if (mEnabled == false) return;
    mStart = Clock::now();
    if (mPrint) std::cout << mName << " restart " << std::endl;
}


#include <GL/glew.h>


GpuTimer::GpuTimer(const std::string& name, bool print): Timer(name, print)
{
   if(mEnabled == false) return;
   if(glGenQueries != nullptr)
   {
      glGenQueries(2, mQuery);
      if (mPrint) std::cout << mName << " start " << std::endl;
      glQueryCounter(mQuery[0], GL_TIMESTAMP);
   }
}

GpuTimer::~GpuTimer()
{
   if (mQuery[0] != -1)
   {
      glDeleteQueries(2, mQuery);
   }
   sAllTimers().remove(this);
}

long double GpuTimer::Stop()
{
    if(mEnabled == false) return -1.0;

    glQueryCounter(mQuery[1], GL_TIMESTAMP);
    GLint stopTimerAvailable = 0;
    
    const int safety = 200000;
    int i=0;
    for(; i<safety; i++)
    {
        glGetQueryObjectiv(mQuery[1], GL_QUERY_RESULT_AVAILABLE, &stopTimerAvailable);
        if(stopTimerAvailable) break;
    }
    
    if(i>=safety)
    {
       //assert(i<safety);
       return -1.0;
    }

    GLuint64 startTime, stopTime;
    glGetQueryObjectui64v(mQuery[0], GL_QUERY_RESULT, &startTime);
    glGetQueryObjectui64v(mQuery[1], GL_QUERY_RESULT, &stopTime);

    mLastTimeMs = (long double)(stopTime - startTime) / 1000000.0;
    long double time_elapsed_s = mLastTimeMs / 1000.0;

    if (mPrint) std::cout << mName << " stop " << time_elapsed_s << " s " << "(" << mLastTimeMs << "ms)" << std::endl;
    return mLastTimeMs;
}

void GpuTimer::Restart()
{
   if(mEnabled == false) return;
   
   if(mQuery[0] == -1)
   {
      glGenQueries(2, mQuery);
   }
   glQueryCounter(mQuery[0], GL_TIMESTAMP);
   if (mPrint) std::cout << mName << " restart " << std::endl;
}










DoubleBufferedGpuTimer::DoubleBufferedGpuTimer(const std::string& name, bool print) : Timer(name, print)
{
   if (mEnabled == false) return;
   if (glGenQueries != nullptr)
   {
      glGenQueries(2, mStartQuery);
      glGenQueries(2, mEndQuery);
      if (mPrint) std::cout << mName << " start " << std::endl;
      glQueryCounter(mStartQuery[0], GL_TIMESTAMP);
      glQueryCounter(mEndQuery[0], GL_TIMESTAMP);
      SwapBuffers();
      glQueryCounter(mStartQuery[0], GL_TIMESTAMP);
   }
}

void DoubleBufferedGpuTimer::SwapBuffers()
{
   std::swap<unsigned int>(mStartQuery[0], mStartQuery[1]);
   std::swap<unsigned int>(mEndQuery[0], mEndQuery[1]);
}

DoubleBufferedGpuTimer::~DoubleBufferedGpuTimer()
{
   if (mStartQuery[0] != -1)
   {
      glDeleteQueries(2, mStartQuery);
      mStartQuery[0] = -1;
      mStartQuery[1] = -1;
   }
   if (mEndQuery[0] != -1)
   {
      glDeleteQueries(2, mEndQuery);
      mEndQuery[0] = -1;
      mEndQuery[1] = -1;
   }
   sAllTimers().remove(this);
}

long double DoubleBufferedGpuTimer::Stop()
{
   if (mEnabled == false) return -1.0;

   glQueryCounter(mEndQuery[0], GL_TIMESTAMP);
   GLint stopTimerAvailable = 0;

   const int safety = 200000;
   int i = 0;
   for (; i < safety; i++)
   {
      glGetQueryObjectiv(mEndQuery[1], GL_QUERY_RESULT_AVAILABLE, &stopTimerAvailable);
      if (stopTimerAvailable) break;
   }

   GLuint64 startTime, stopTime;
   glGetQueryObjectui64v(mStartQuery[1], GL_QUERY_RESULT, &startTime);
   glGetQueryObjectui64v(mEndQuery[1], GL_QUERY_RESULT, &stopTime);
   SwapBuffers();

   mLastTimeMs = (long double)(stopTime - startTime) / 1000000.0;
   long double time_elapsed_s = mLastTimeMs / 1000.0;

   if (mPrint) std::cout << mName << " stop " << time_elapsed_s << " s " << "(" << mLastTimeMs << "ms)" << std::endl;
   return mLastTimeMs;
}

void DoubleBufferedGpuTimer::Restart()
{
   if (mEnabled == false) return;

   if (mStartQuery[0] == -1)
   {
      glGenQueries(2, mStartQuery);
      glGenQueries(2, mEndQuery);
      
      glQueryCounter(mStartQuery[0], GL_TIMESTAMP);
      glQueryCounter(mEndQuery[0], GL_TIMESTAMP);
      SwapBuffers();
   }
   glQueryCounter(mStartQuery[0], GL_TIMESTAMP);
   if (mPrint) std::cout << mName << " restart " << std::endl;
}








#include "imgui.h"
#include "implot.h"

// utility structure for realtime plot
struct ScrollingBuffer {
   int MaxSize;
   int Offset;
   ImVector<ImVec2> Data;
   ScrollingBuffer(int max_size = 2000)
   {
      MaxSize = max_size;
      Offset = 0;
      Data.reserve(MaxSize);
   }
   void AddPoint(float x, float y)
   {
      if (Data.size() < MaxSize)
         Data.push_back(ImVec2(x, y));
      else
      {
         Data[Offset] = ImVec2(x, y);
         Offset = (Offset + 1) % MaxSize;
      }
   }
   void Erase()
   {
      if (Data.size() > 0)
      {
         Data.shrink(0);
         Offset = 0;
      }
   }
};

// utility structure for realtime plot
struct RollingBuffer
{
   float Span;
   ImVector<ImVec2> Data;
   RollingBuffer()
   {
      Span = 10.0f;
      Data.reserve(2000);
   }
   void AddPoint(float x, float y)
   {
      float xmod = fmodf(x, Span);
      if (!Data.empty() && xmod < Data.back().x)
         Data.shrink(0);
      Data.push_back(ImVec2(xmod, y));
   }
};

#include <deque>
#include <vector>

namespace TimerGui
{
   bool show_timers_plot = false;
   std::deque<bool> plot; //std::vector<bool> is broken
   std::vector<ScrollingBuffer> timer_data;
}




void TimerGui::Menu()
{
   static bool show_enable_all = false;
   static bool show_silence_all = true;

   if (ImGui::BeginMenu("Timers"))
   {
      if (!show_enable_all && ImGui::MenuItem("Disable All", 0))
      {
         Timer::SetEnabledAll(false);
         show_enable_all = !show_enable_all;
      }
      else if (show_enable_all && ImGui::MenuItem("Enable All", 0))
      {
         Timer::SetEnabledAll(true);
         show_enable_all = !show_enable_all;
      }

      if (show_silence_all && ImGui::MenuItem("Silence All", 0))
      {
         Timer::SetPrintAll(false);
         show_silence_all = !show_silence_all;
      }
      else if (!show_silence_all && ImGui::MenuItem("Unsilence All", 0))
      {
         Timer::SetPrintAll(true);
         show_silence_all = !show_silence_all;
      }

      if (ImGui::MenuItem("Show Plot Window", 0, &show_timers_plot))
      {
         //show_timers_plot = !show_timers_plot;
      }
      
      ImGui::Separator();
      int i = 0;
      for (Timer* t : Timer::sAllTimers())
      {
         if(t->mName == "") continue;
         if(ImGui::MenuItem(t->mName.c_str(), "", t->mEnabled))
         {
            t->mEnabled = !t->mEnabled;
         }
         ImGui::SameLine();
         ImGui::Text("%f ms  ", t->mLastTimeMs);
         i++;
      }
      ImGui::EndMenu();
   }
}

void TimerGui::DrawGui()
{
   int n_timers = Timer::sAllTimers().size();
   if (plot.size() != n_timers)
   {
      plot.resize(n_timers, false);
      timer_data.resize(n_timers);
   }

   if (show_timers_plot)
   {
      int index = 0;
      if(ImGui::Begin("Timers plot", &show_timers_plot))
      {
         const ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp;
         if(ImGui::BeginTable("Timer Table", 5, flags, ImVec2(-1.0, 150.0f)))
         {
            ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Enabled", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Print", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Plot", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Last Time (ms)", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            
            index = 0;
            for (Timer* t : Timer::sAllTimers())
            {
               ImGui::TableNextRow();
               ImGui::PushID(index);
               ImGui::TableSetColumnIndex(0);   //name
               ImGui::Text(t->mName.c_str());
               ImGui::TableSetColumnIndex(1);   //enabled
               ImGui::Checkbox("##enabled", &t->mEnabled);
               ImGui::TableSetColumnIndex(2);   //print
               ImGui::Checkbox("##print", &t->mPrint);
               ImGui::TableSetColumnIndex(3);   //plot
               ImGui::Checkbox("##plot", &plot[index]);
               ImGui::TableSetColumnIndex(4); //time
               ImGui::Text("% f", t->GetLastTimeMs());
               ImGui::PopID();
               index++;
            }
            ImGui::EndTable();
         }
         static float history = 500.0f;
         static float frame = 0.0;
         frame += 1.0;

         index=0;
         for (Timer* t : Timer::sAllTimers())
         {
            timer_data[index].AddPoint(frame, t->GetLastTimeMs());
            index++;
         }

         const ImPlotAxisFlags hflags = ImPlotAxisFlags_NoTickLabels;
         const ImPlotAxisFlags vflags = ImPlotAxisFlags_None;

         if (ImPlot::BeginPlot("##Scrolling"))
         {
            ImPlot::SetupAxes("Frame", "Time (ms)", hflags, vflags);
            ImPlot::SetupAxisLimits(ImAxis_X1, frame - history, frame, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, 5.0);

            index = 0;
            for (Timer* t : Timer::sAllTimers())
            {
               if(plot[index] == true)
               {
                  ImPlot::SetNextLineStyle(ImVec4(0, 0, 0, -1), 2.0f);
                  ImPlot::PlotLine(t->mName.c_str(), &timer_data[index].Data[0].x, &timer_data[index].Data[0].y, timer_data[index].Data.size(), 0, timer_data[index].Offset, 2 * sizeof(float));
               }
               index++;
            }
            ImPlot::EndPlot();
         }
         ImGui::End();
      }
   }
}

