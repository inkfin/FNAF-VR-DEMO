#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include "imgui.h"
#include "implot.h"
#include "Module.h"
#include "ComputeShader.h"
#include "ComputePattern.h"

struct ComputeTest : public Module
{
   virtual bool Validate() {return false;}
   virtual void Complexity() {}

   static void Menu()
   {
      if (ImGui::BeginMenu("Compute Tests"))
      {
         int i = 0;
         for (Module* mod : Module::sAllModules())
         {
            ComputeTest* ctest = dynamic_cast<ComputeTest*>(mod);
            bool is_compute_test = (ctest != nullptr);
            if (is_compute_test)
            {
               if (ImGui::MenuItem(typeid(*mod).name(), "", ctest->mGuiOpen))
               {
                  ctest->mGuiOpen = !ctest->mGuiOpen;
               }
            }
            i++;
         }
         ImGui::EndMenu();
      }
   }

};

struct IntegrationTest : public ComputeTest
{
   float mP0 = 0.0f;
   float mV0 = 10.0f;
   float mG = -9.81; // acceleration due to gravity m/s/s
   float mM = 1.0f;
   float mK = 4.0f;
   float mC = 1.1f;

   float mDT = 0.1f;
   int mMaxN = 300;

   int mMode = 0; //=0: Projectile, =1: Mass-Spring_damper

   std::vector<float> mTVec;

   std::vector<float> mTruePVec;
   std::vector<float> mExplicitEulerPVec;
   std::vector<float> mSemiImplicitEulerPVec;
   std::vector<float> mLeapfrogPVec;
   std::vector<float> mVerletPVec;
   std::vector<float> mVelocityVerletPVec;
   std::vector<float> mRK2PVec;

   std::vector<float> mTrueEVec;
   std::vector<float> mExplicitEulerEVec;
   std::vector<float> mSemiImplicitEulerEVec;
   std::vector<float> mLeapfrogEVec;
   std::vector<float> mVerletEVec;
   std::vector<float> mVelocityVerletEVec;
   std::vector<float> mRK2EVec;


   float TruePProjectile(float t)
   {
      return mP0 + mV0 * t + 0.5f * mG * t * t;
   }
   
   float TrueVProjectile(float t)
   {
      return mV0 + mG * t;
   }

   float ForceProjectile(float p, float v)
   {
      return mM * mG;
   }

   float EnergyProjectile(float p, float v) //sum of potential and kinetic energy
   {
      return 0.5f * mM * v * v + mM * abs(mG) * p;
   }


  
   float TruePSpring(float t)
   {
      if(mC==0.0f) //undamped
      {
         float Omega = sqrt(mK/mM);
         return mP0*cos(Omega*t) + mV0/Omega*sin(Omega*t);
      }
      else
      {
         float c_thresh = sqrt(4.0f*mK*mM);
         float a = mC / (2.0f * mM);
         float eat = exp(-a * t);
         if(mC < c_thresh) //underdamped
         {
            float w = sqrt(4.0f*mK*mM-mC*mC)/(2.0f*mM);
            float k2 = (mV0 + a * mP0) / w;
            return mP0*eat*cos(w*t) + k2*eat*sin(w*t);
         }
         else if(mC > c_thresh) //overdamped
         {
            float b = sqrt(a*a-mK/mM);
            float k1 = (mV0 + (a + b) * mP0) / (2.0f * b);
            float k2 = (mV0 + (a - b) * mP0) / (2.0f * b);
            return k1*exp(-(a-b)*t)-k2*exp(-(a+b)*t);
         }
         else //critically damped
         {
            float k2 = (mV0 + a * mP0);
            return mP0*eat + k2*t*eat;
         }
      }
   }

   float TrueVSpring(float t)
   {
      if (mC == 0.0f) //undamped
      {
         float Omega = sqrt(mK / mM);
         //return mP0 * cos(Omega * t) + mV0 / Omega * sin(Omega * t);
         return -mP0 * Omega*sin(Omega * t) + mV0  * cos(Omega * t);
      }
      else
      {
         float c_thresh = sqrt(4.0f * mK * mM);
         float a = mC / (2.0f * mM);
         float eat = exp(-a * t);
         if (mC < c_thresh) //underdamped
         {
            float w = sqrt(4.0f * mK * mM - mC * mC) / (2.0f * mM);
            float k2 = (mV0 + a * mP0) / w;
            //return mP0 * eat * cos(w * t) + k2 * eat * sin(w * t);
            return -eat*((mP0*w + a*k2)*sin(w*t)+(a*mP0 - k2*w)*cos(w*t));
         }
         else if (mC > c_thresh) //overdamped
         {
            float b = sqrt(a * a - mK / mM);
            float k1 = (mV0 + (a + b) * mP0) / (2.0f * b);
            float k2 = (mV0 + (a - b) * mP0) / (2.0f * b);
            //return k1 * exp(-(a - b) * t) - k2 * exp(-(a + b) * t);
            return (b-a)*k1*exp(-(a-b)*t)-(-b-a)*k2*exp(-(b+a)*t);
         }
         else //critically damped
         {
            float k2 = (mV0 + a * mP0);
            //return mP0 * eat + k2 * t * eat;
            return -(a*k2*t-k2+a*mP0)*eat;
         }
      }
   }

   float EnergySpring(float p, float v) //kinetic + spring potential
   {
      return 0.5*mM*v*v + 0.5*mK*p*p;
   }

   float ForceSpring(float p, float v)
   {
      return -mK*p - mC*v;
   }

   float TrueP(float t)
   {
      if(mMode==0) return TruePProjectile(t);
      if(mMode==1) return TruePSpring(t);
   }

   float TrueV(float t)
   {
      if(mMode==0) return TrueVProjectile(t);
      if(mMode==1) return TrueVSpring(t);
   }
   
   float Energy(float p, float v) //sum of potential and kinetic energy
   {
      if(mMode==0) return EnergyProjectile(p,v);
      if(mMode==1) return EnergySpring(p, v);
   }

   float Force(float p, float v)
   {
      if(mMode==0) return ForceProjectile(p,v);
      if(mMode==1) return ForceSpring(p, v);
   }


   bool Validate() override
   {
      mTVec.resize(0);

      mTruePVec.resize(0);
      mExplicitEulerPVec.resize(0);
      mSemiImplicitEulerPVec.resize(0);
      mLeapfrogPVec.resize(0);
      mVerletPVec.resize(0);
      mVelocityVerletPVec.resize(0);
      mRK2PVec.resize(0);

      mTrueEVec.resize(0);
      mExplicitEulerEVec.resize(0);
      mSemiImplicitEulerEVec.resize(0);
      mLeapfrogEVec.resize(0);
      mVerletEVec.resize(0);
      mVelocityVerletEVec.resize(0);
      mRK2EVec.resize(0);
      
      float p_i, v_i, a_i, t_i;
      int i;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      t_i = 0.0f; //==t_0
      p_i = TrueP(t_i); //== p_0    assert == mP0
      v_i = TrueV(t_i); //== v_0    assert == mV0
      
      while((mMode==0 && p_i >= 0.0f) || (mMode==1 && mTVec.size()<mMaxN))
      {
         mTruePVec.push_back(p_i);
         mTrueEVec.push_back(Energy(p_i, v_i));
         mTVec.push_back(t_i);

         t_i += mDT;       //==t_{i+1}
         p_i = TrueP(t_i); //==p_{i+1}
         v_i = TrueV(t_i); //==v_{i+1}
      }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      p_i = mP0; //==p_0
      v_i = mV0; //==v_0
      
      for(int i=0; i<mTVec.size(); i++)
      {
         mExplicitEulerPVec.push_back(p_i);
         mExplicitEulerEVec.push_back(Energy(p_i, v_i));

         a_i = Force(p_i, v_i)/mM;

         p_i += v_i*mDT; //==p_{i+1} = p_i + v_i*dt
         v_i += a_i*mDT; //==v_{i+1} = v_i + a_i*dt
      }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      p_i = mP0; //==p_0
      v_i = mV0; //==v_0

      for (int i = 0; i < mTVec.size(); i++)
      {
         mSemiImplicitEulerPVec.push_back(p_i);
         mSemiImplicitEulerEVec.push_back(Energy(p_i, v_i));

         a_i = Force(p_i, v_i) / mM; //==a_i;

         v_i += a_i * mDT; //v_{i+1} = v_i + a_i*dt
         p_i += v_i * mDT; //p_{i+1} = p_i + v_{i+1}*dt
      }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//*
      p_i = mP0;  //==p_0
      v_i = mV0;  //==v_0
      a_i = Force(p_i, v_i) / mM; //==a_0
      v_i += 0.5f*a_i*mDT; //==v_{1/2}  half step update
      for (int i = 0; i < mTVec.size(); i++)
      {
         mLeapfrogPVec.push_back(p_i);
         mLeapfrogEVec.push_back(Energy(p_i, v_i-0.5*a_i*mDT)); //adjust velocity back to integer index for E calculation

         p_i += v_i * mDT; //p_{i+1} = p_i + v_{i+1/2}*dt
         a_i = Force(p_i, v_i) / mM; //a_{i+1}
         v_i += a_i * mDT; //v_{i+1/2+1} = v_{i+1/2} + a_{i+1}
      }
//*/
/*
      //Leapfrog at integer indices
      p_i = mP0;  //==p_0
      v_i = mV0;  //==v_0
      for (int i = 0; i < mTVec.size(); i++)
      {
         mLeapfrogPVec.push_back(p_i);
         mLeapfrogEVec.push_back(Energy(p_i, v_i)); 

         a_i = Force(p_i, v_i) / mM; //a_i
         p_i += v_i * mDT + 0.5*a_i*mDT*mDT; //p_{i+1} = p_i + v_i*dt + 0.5*a_i*dt*dt
         float a_next = Force(p_i, v_i+a_i*mDT) / mM; //a_{i+1}
         v_i += 0.5f*(a_i + a_next) * mDT; //v_{i+1} = v_{i} + (a_{i+1}+a_i)*dt/2.0
      }
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      p_i = mP0;  //==p_0
      v_i = mV0;  //==v_0

      for (int i = 0; i < mTVec.size(); i++)
      {
         mRK2PVec.push_back(p_i);
         mRK2EVec.push_back(Energy(p_i, v_i)); 

         a_i = Force(p_i, v_i) / mM; //a_{i+1}
         float ph = p_i + 0.5f*v_i*mDT;
         float vh = v_i + 0.5f*a_i*mDT;
         
         a_i = Force(ph, vh) / mM; //a_midpoint
         v_i += a_i*mDT; 
         p_i += v_i*mDT;  
      }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      p_i = mP0; //p_0
      v_i = mV0; //v_0
 
      mVerletPVec.push_back(p_i);
      mVerletEVec.push_back(Energy(p_i, v_i));
   
      //i==1;
      a_i = Force(p_i, v_i) / mM;
      p_i = p_i + mV0*mDT + 0.5f*a_i*mDT*mDT; //p_1
      v_i = (p_i - mP0)/mDT; //v_1
     
      for (int i = 1; i < mTVec.size(); i++)
      {
         mVerletPVec.push_back(p_i);
         float v_e = v_i;
         if(i>1) v_e = (mVerletPVec[i]-mVerletPVec[i-1])/mDT;
         mVerletEVec.push_back(Energy(p_i, v_e)); //adjust velocity to account for one-step lag

         a_i = Force(p_i, v_i) / mM;

         //v_i = (p_i - p_{i-1})/dt
         //or
         //v_i = (p_{i+1} - p_{i-1})/(2.0*dt)

         //p_{i+1} = p_i + v_i*dt + a_i*dt*dt
         
         p_i = 2.0f*mVerletPVec[i] - mVerletPVec[i-1] + a_i*mDT*mDT; //p_{i+1}
         //this velocity is actually a step behind p_i
         v_i = (p_i- mVerletPVec[i-1])/ (2.0f*mDT); //v_i
      }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      p_i = mP0; //p_0
      v_i = mV0; //v_0
      a_i = Force(p_i, v_i) / mM; //a_0
      for (int i = 0; i < mTVec.size(); i++)
      {
         mVelocityVerletPVec.push_back(p_i);
         mVelocityVerletEVec.push_back(Energy(p_i, v_i));

         //half-step update of v
         v_i += 0.5f*a_i*mDT; //v_{i+1/2} = v_i + 0.5*a_t*dt
         p_i += v_i * mDT;    //p_{i+1} = p_i + v_{i+1/2}*dt

         a_i = Force(p_i, v_i) / mM; //a_{i+1}
         v_i += 0.5f * a_i * mDT;   //v_{i+1} = v_{i+1/2}+0.5*a_{i+1}*dt
      }

      return true;
   }

   void DrawGui() override
   {
      if (mGuiOpen == false) return;
      if (ImGui::Begin("Integration Test", &mGuiOpen))
      {
         ImGui::RadioButton("Projectile", &mMode, 0);
         ImGui::SameLine();
         ImGui::RadioButton("Mass-spring-damper", &mMode, 1);

         ImGui::SliderFloat("DT:timestep (sec)", &mDT, 0.0001f, 0.5f);
         ImGui::SliderFloat("P0:init pos (m)", &mP0, 0.0f, 1.0f);
         ImGui::SliderFloat("V0:init vel (m/s)", &mV0, -1.0f, 1.0f);
         ImGui::SliderFloat("M:mass (kg)", &mM, 0.0f, 1.0f);
         ImGui::SliderFloat("K:spring const (N/m)", &mK, 0.0f, 1.0f);
         ImGui::SliderFloat("C:damping coeff (kg/s)", &mC, 0.0f, 5.0f);


         if (ImGui::Button("Validate"))
         {
            Validate();
         }

         if (ImPlot::BeginPlot("Projectile Height")) 
         {
            ImPlot::SetupAxes("time (s)", "Y (m)", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Square);
            ImPlot::PlotLine("True", mTVec.data(), mTruePVec.data(), mTruePVec.size());
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
            ImPlot::PlotLine("Explicit Euler", mTVec.data(), mExplicitEulerPVec.data(), mExplicitEulerPVec.size());
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Up);
            ImPlot::PlotLine("Semi-Implicit Euler", mTVec.data(), mSemiImplicitEulerPVec.data(), mSemiImplicitEulerPVec.size());
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Down);
            ImPlot::PlotLine("Leapfrog", mTVec.data(), mLeapfrogPVec.data(), mLeapfrogPVec.size());
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Left);
            ImPlot::PlotLine("Verlet", mTVec.data(), mVerletPVec.data(), mVerletPVec.size());
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Right);
            ImPlot::PlotLine("Velocity Verlet", mTVec.data(), mVelocityVerletPVec.data(), mVelocityVerletPVec.size());
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Cross);
            ImPlot::PlotLine("RK2", mTVec.data(), mRK2PVec.data(), mRK2PVec.size());
            ImPlot::EndPlot();
         }

         if (ImPlot::BeginPlot("Projectile Energy"))
         {
            ImPlot::SetupAxes("time (s)", "E (N m)", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Square);
            ImPlot::PlotLine("True", mTVec.data(), mTrueEVec.data(), mTrueEVec.size());
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
            ImPlot::PlotLine("Explicit Euler", mTVec.data(), mExplicitEulerEVec.data(), mExplicitEulerEVec.size());
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Up);
            ImPlot::PlotLine("Semi-Implicit Euler", mTVec.data(), mSemiImplicitEulerEVec.data(), mSemiImplicitEulerEVec.size());
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Down);
            ImPlot::PlotLine("Leapfrog", mTVec.data(), mLeapfrogEVec.data(), mLeapfrogEVec.size());
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Left);
            ImPlot::PlotLine("Verlet", mTVec.data(), mVerletEVec.data(), mVerletEVec.size());
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Right);
            ImPlot::PlotLine("Velocity Verlet", mTVec.data(), mVelocityVerletEVec.data(), mVelocityVerletEVec.size());
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Cross);
            ImPlot::PlotLine("RK2", mTVec.data(), mRK2EVec.data(), mRK2EVec.size());
            ImPlot::EndPlot();
         }
      }
      ImGui::End();
   }

};


struct BarrierTest : public ComputeTest
{
   ComputeShader mBarrierTestCs = ComputeShader("barrier_test_cs.glsl");

   const int n = 1024;
   Buffer mBarrierTestBuffer = Buffer(GL_SHADER_STORAGE_BUFFER);
   std::vector<int> mBarrierTestVector;
   int mMode = 0;

   void Init() override
   {
      mBarrierTestCs.Init();

      mBarrierTestBuffer.mBinding = 0;
      mBarrierTestBuffer.Init(n * sizeof(int));
      mBarrierTestBuffer.ClearToInt(0);

      mBarrierTestVector.resize(n, 0);
   }

   bool Validate() override
   {
      mBarrierTestCs.SetMode(mMode);
      mBarrierTestCs.SetNumElements(n);
      mBarrierTestCs.SetGridSize(glm::ivec3(n, 1, 1));
      mBarrierTestCs.UseProgram();

      mBarrierTestBuffer.ClearToInt(0);
      mBarrierTestBuffer.BindBufferBase();

      mBarrierTestCs.Dispatch();
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

      mBarrierTestBuffer.GetBufferSubData(mBarrierTestVector.data());

      return false;
   }

   void DrawGui() override
   {
      if (mGuiOpen == false) return;
      if (ImGui::Begin("Barrier Test", &mGuiOpen))
      {
         if(ImGui::SliderInt("Mode", &mMode, 0, 4))
         {
            mBarrierTestCs.SetMode(mMode);
         }        

         if (ImGui::Button("Validate"))
         {
            Validate();
         }


         if (ImGui::BeginTable("##BARRIER", 2, ImGuiTableFlags_ScrollY))
         {
            ImGui::TableSetupColumn("gl_GlobalInvocationID", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_None);
            ImGui::TableHeadersRow();

            ImGuiListClipper clipper;
            clipper.Begin(mBarrierTestVector.size());
            while (clipper.Step())
            {
               for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
               {
                  ImGui::TableNextRow();
                  ImGui::TableNextColumn();
                  ImGui::Text("%d", row);
                  ImGui::TableNextColumn();
                  ImGui::Text("%d", mBarrierTestVector[row]);
               }
            }
            ImGui::EndTable();//ORDER
         }

      }
      ImGui::End();
   }
};

struct CoherentTest : public ComputeTest
{
   ComputeShader mCoherentTestCs = ComputeShader("coherent_test_cs.glsl");

   const int n = 1024;
   ImageTexture mCoherentTestImage;
   Buffer mCoherentTestBuffer = Buffer(GL_SHADER_STORAGE_BUFFER);
   std::vector<float> mCoherentTestVector;

   void Init() override
   {
      mCoherentTestCs.Init();

      mCoherentTestImage.SetTarget(GL_TEXTURE_2D);
      mCoherentTestImage.SetSize(glm::ivec3(n, 1, 1));
      mCoherentTestImage.SetInternalFormat(GL_R32F);
      mCoherentTestImage.SetUnit(0);
      mCoherentTestImage.Init();

      mCoherentTestBuffer.mBinding = 0;
      mCoherentTestBuffer.Init(n * sizeof(float));
      mCoherentTestBuffer.ClearToFloat(0.0f);

      mCoherentTestVector.resize(n, 0);
   }

   bool Validate() override
   {
      mCoherentTestCs.SetGridSize(glm::ivec3(n, 1, 1));
      mCoherentTestCs.UseProgram();

      const int level = 0;
      const int offset = 0;
      std::vector<float> zeros(n, 0.0f);
      glTextureSubImage2D(mCoherentTestImage.GetTexture(), level, offset, offset, n, 1, GL_RED, GL_FLOAT, zeros.data());
      mCoherentTestImage.BindImageTexture(GL_READ_WRITE);

      mCoherentTestBuffer.ClearToFloat(0.0f);
      mCoherentTestBuffer.BindBufferBase();

      mCoherentTestCs.Dispatch();
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

      mCoherentTestBuffer.GetBufferSubData(mCoherentTestVector.data());

      return false;
   }

   void DrawGui() override
   {
      if (mGuiOpen == false) return;
      if (ImGui::Begin("Coherent Test", &mGuiOpen))
      {
         if (ImGui::Button("Validate"))
         {
            Validate();
         }
         
         ImVec2 size(n, 64);
         ImGui::Image((void*)mCoherentTestImage.GetTexture(), size);

         if (ImGui::BeginTable("##COHERENT", 2, ImGuiTableFlags_ScrollY))
         {
            ImGui::TableSetupColumn("gl_GlobalInvocationID", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_None);
            ImGui::TableHeadersRow();

            ImGuiListClipper clipper;
            clipper.Begin(mCoherentTestVector.size());
            while (clipper.Step())
            {
               for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
               {
                  ImGui::TableNextRow();
                  ImGui::TableNextColumn();
                  ImGui::Text("%d", row);
                  ImGui::TableNextColumn();
                  ImGui::Text("%f", mCoherentTestVector[row]);
               }
            }
            ImGui::EndTable();//ORDER
         }

      }
      ImGui::End();
   }
};

struct InvocationOrderTest : public ComputeTest
{
   ComputeShader mInvocationOrderCs = ComputeShader("invocation_order_cs.glsl");

   Buffer mAtomicCounterBuffer = Buffer(GL_ATOMIC_COUNTER_BUFFER);

   const int n = 1024;
   Buffer mInvocationOrderBuffer = Buffer(GL_SHADER_STORAGE_BUFFER);
   std::vector<glm::uint> mInvocationOrder;

   ImageTexture mInvocationImage;

   bool mInvocationsInOrder = false;
   bool mInvocationsUnique = false;

   void Init() override
   {
      mInvocationOrderCs.Init();

      mAtomicCounterBuffer.mBinding = 0;
      mAtomicCounterBuffer.Init(sizeof(uint32_t));
      mAtomicCounterBuffer.ClearToUint(0);

      mInvocationOrder.resize(n, -1);
   
      mInvocationOrderBuffer.mBinding = 1;
      mInvocationOrderBuffer.Init(n*sizeof(uint32_t));
      mInvocationOrderBuffer.ClearToUint(0);

      mInvocationImage.SetTarget(GL_TEXTURE_2D);
      mInvocationImage.SetSize(glm::ivec3(n, 1, 1));
      mInvocationImage.SetInternalFormat(GL_R32F);
      mInvocationImage.SetUnit(0);
      mInvocationImage.Init();
   }

   bool Validate() override
   {
      mInvocationsInOrder = false;
      mInvocationsUnique = false;

      mInvocationOrderCs.SetGridSize(glm::ivec3(n, 1, 1));
      mInvocationOrderCs.UseProgram();
      
      mAtomicCounterBuffer.ClearToUint(0);
      mAtomicCounterBuffer.BindBufferBase();
      mInvocationOrderBuffer.BindBufferBase();

      mInvocationImage.BindImageTexture(GL_WRITE_ONLY);

      mInvocationOrderCs.Dispatch();
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      mInvocationOrderBuffer.GetBufferSubData(mInvocationOrder.data());
      
      //Are the invocations ordered?
      mInvocationsInOrder = (mInvocationOrder[0]==0) && (mInvocationOrder[n-1] == n-1) && std::is_sorted(mInvocationOrder.begin(), mInvocationOrder.end());
      
      //Is each invocation id in the vector once?
      std::vector<uint32_t> sorted_order = mInvocationOrder;
      std::sort(sorted_order.begin(), sorted_order.end());
      std::vector<uint32_t>::iterator it = std::unique(sorted_order.begin(), sorted_order.end());
      mInvocationsUnique  = (sorted_order[0] == 0) && (sorted_order[n - 1] == n - 1) && (it == sorted_order.end());

      return mInvocationsInOrder;
   }

   void DrawGui() override
   {
      if (mGuiOpen == false) return;
      if (ImGui::Begin("Invocation Order Test", &mGuiOpen))
      {
         if (ImGui::Button("Validate"))
         {
            Validate();
         }
         ImGui::Text("Invocations unique = %d", mInvocationsUnique);
         ImGui::Text("Threads invoked in order = %d", mInvocationsInOrder);
         ImVec2 size(n, 128);
         ImGui::Image((void*)mInvocationImage.GetTexture(), size);
         
         if (ImGui::BeginTable("##ORDER", 2, ImGuiTableFlags_ScrollY))
         {
            ImGui::TableSetupColumn("gl_GlobalInvocationID", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Order", ImGuiTableColumnFlags_None);
            ImGui::TableHeadersRow();

            ImGuiListClipper clipper;
            clipper.Begin(mInvocationOrder.size());
            while (clipper.Step())
            {
               for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
               {
                  ImGui::TableNextRow();
                  ImGui::TableNextColumn();
                  ImGui::Text("%d", row);
                  ImGui::TableNextColumn();
                  ImGui::Text("%d", mInvocationOrder[row]);
               }
            }
            ImGui::EndTable();//ORDER
         }
            
      }
      ImGui::End();
   }
};

struct SortTest : public ComputeTest
{
   ComputeShader mBitonicSortCs = ComputeShader("bitonic_sort_cs.glsl");
   BufferBitonicSort mBitonicSort;

   ComputeShader mOddEvenSortCs = ComputeShader("buffer_odd_even_sort_cs.glsl");
   BufferOddEvenSort mOddEvenSort;

   const int mNumItems = 4*1024;
   std::vector<int> mUnsorted = std::vector<int>(mNumItems);
   std::vector<int> mCpuSortResults = std::vector<int>(mNumItems);
   std::vector<int> mBitonicSortResults = std::vector<int>(mNumItems);
   std::vector<int> mOddEvenSortResults = std::vector<int>(mNumItems);
   BufferArray mBitonicItemBuffer = BufferArray(GL_SHADER_STORAGE_BUFFER);
   BufferArray mOddEvenItemBuffer = BufferArray(GL_SHADER_STORAGE_BUFFER);
   Buffer mReadbackBuffer = Buffer(GL_SHADER_STORAGE_BUFFER);

   std::vector<double> mComplexitySize;
   std::vector<double> mCpuComplexityTimes;
   std::vector<double> mBitonicComplexityTimes;
   std::vector<double> mOddEvenComplexityTimes;

   bool mBitonicValid = false;
   bool mOddEvenValid = false;

   double mCpuSortTime = -1.0;
   double mBitonicSortTime = -1.0;
   double mOddEvenSortTime = -1.0;

   void Init() override
   {
      for (int i = 0; i < mNumItems; i++)
      {
         mUnsorted[i] = int(9e6 * (1.0 + glm::sin(45234.6 * i)));
      }
      mBitonicItemBuffer.Init(mUnsorted.size(), sizeof(int), mUnsorted.data());
      mOddEvenItemBuffer.Init(mUnsorted.size(), sizeof(int), mUnsorted.data());
      mReadbackBuffer.Init(mUnsorted.size() * sizeof(int));

      mBitonicSortCs.Init();
      mBitonicSort.SetComputeShader(mBitonicSortCs);
      mBitonicSort.Init();

      mOddEvenSortCs.Init();
      mOddEvenSort.SetComputeShader(mOddEvenSortCs);
      mOddEvenSort.Init();

      mBitonicValid = false;
      mOddEvenValid = false;
   }

   bool Validate() override
   {
      mBitonicItemBuffer.Init(mUnsorted.size(), sizeof(int), mUnsorted.data());
      mOddEvenItemBuffer.Init(mUnsorted.size(), sizeof(int), mUnsorted.data());

      mCpuSortResults = mUnsorted;
      Timer sort_timer("Cpu Sort ");
      std::sort(mCpuSortResults.begin(), mCpuSortResults.end());
      sort_timer.Stop();
      mCpuSortTime = sort_timer.GetLastTimeMs();

      GpuTimer bitonic_timer("bitonic validation timer",false);
      mBitonicSort.Compute(mBitonicItemBuffer);
      bitonic_timer.Stop();
      mBitonicItemBuffer.GetBufferSubData(mBitonicSortResults.data());
      mBitonicSortTime = bitonic_timer.GetLastTimeMs();

      mBitonicValid = (mBitonicSortResults == mCpuSortResults);
      std::cout << "Bitonic Sorting validated = " << mBitonicValid << std::endl;

      GpuTimer odd_even_timer("odd-even validation timer", false);
      mOddEvenSort.Compute(mOddEvenItemBuffer);
      odd_even_timer.Stop();
      mOddEvenItemBuffer.GetBufferSubData(mOddEvenSortResults.data());
      mOddEvenSortTime = odd_even_timer.GetLastTimeMs();

      mOddEvenValid = (mOddEvenSortResults == mCpuSortResults);
      std::cout << "Odd-Even Sorting validated = " << mOddEvenValid << std::endl;

      return mBitonicValid && mOddEvenValid;
   }

   void Complexity() override
   {
      int reps = 10;
      //randomize items
      std::vector<int> test_items(mNumItems);
      for (int i = 0; i < (mNumItems); i++)
      {
         test_items[i] = int(9e6 * (1.0 + glm::sin(45234.6 * i)));
      }

      mComplexitySize.resize(0);
      for (int n = (mNumItems); n > 1; n = n / 2)
      {
         mComplexitySize.push_back(n);
      }

      mBitonicComplexityTimes.resize(0);
      mOddEvenComplexityTimes.resize(0);
      GpuTimer bitonic_timer("bitonic complexity timer", false);
      GpuTimer odd_even_timer("odd-even complexity timer", false);
      for (int i = 0; i < mComplexitySize.size(); i++)
      {
         double bitonic_time_sum = 0.0;
         double odd_even_time_sum = 0.0;
         for (int r = 0; r < reps; r++)
         {
            BufferArray test_buffer(GL_SHADER_STORAGE_BUFFER);
            test_buffer.Init(mComplexitySize[i], sizeof(int), test_items.data());
            bitonic_timer.Restart();
            mBitonicSort.Compute(test_buffer);
            bitonic_timer.Stop();
            test_buffer.Free();
            bitonic_time_sum += bitonic_timer.GetLastTimeMs();

            test_buffer.Init(mComplexitySize[i], sizeof(int), test_items.data());
            odd_even_timer.Restart();
            mOddEvenSort.Compute(test_buffer);
            odd_even_timer.Stop();
            test_buffer.Free();
            odd_even_time_sum += odd_even_timer.GetLastTimeMs();
         }
         mBitonicComplexityTimes.push_back(bitonic_time_sum / reps);
         mOddEvenComplexityTimes.push_back(odd_even_time_sum / reps);
      }

      for (int i = 0; i < mComplexitySize.size(); i++)
      {
         double time_sum = 0.0;
         for (int r = 0; r < reps; r++)
         {
            std::vector<int> r_test = test_items;
            Timer sort_timer("Cpu Sort ");
            std::sort(r_test.begin(), r_test.begin() + mComplexitySize[i]);
            sort_timer.Stop();

            time_sum += sort_timer.GetLastTimeMs();
         }
         mCpuComplexityTimes.push_back(time_sum / reps);
      }

      std::ofstream perf_file;
      perf_file.open("sort_perf.txt");
      perf_file << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
      perf_file << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
      perf_file << "Size \t CPU \t GPU bitonic \n";
      for (int i = 0; i < mComplexitySize.size(); i++)
      {
         perf_file << std::to_string(mComplexitySize[i]) << "\t";
         perf_file << std::to_string(mCpuComplexityTimes[i]) << "\t";
         perf_file << std::to_string(mBitonicComplexityTimes[i]) << "\t";
         perf_file << std::to_string(mOddEvenComplexityTimes[i]) << "\n";
      }
      perf_file.close();
   }

   void DrawGui() override
   {
      if (mGuiOpen == false) return;
      if(ImGui::Begin("Sort Test", &mGuiOpen))
      {
         if (ImGui::BeginTable("##COLUMNS", 2))
         {
            ImGui::TableNextColumn();
            if (ImGui::Button("Validate"))
            {
               Validate();
            }
            ImGui::SameLine(); ImGui::Text("Bitonic Valid = %d", mBitonicValid);
            ImGui::SameLine(); ImGui::Text("Odd-even Valid = %d", mOddEvenValid);
            
            ImGui::Text("N = %d", mNumItems);
            ImGui::Text("CPU sort time = %f ms", mCpuSortTime);
            ImGui::Text("GPU bitonic sort time = %f ms", mBitonicSortTime);
            ImGui::Text("GPU odd-even sort time = %f ms", mOddEvenSortTime);
            if (ImGui::BeginTable("Data", 5, ImGuiTableFlags_ScrollY))
            {
               ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_None);
               ImGui::TableSetupColumn("Unsorted", ImGuiTableColumnFlags_None);
               ImGui::TableSetupColumn("CPU Sorted", ImGuiTableColumnFlags_None);
               ImGui::TableSetupColumn("Bitonic Sorted", ImGuiTableColumnFlags_None);
               ImGui::TableSetupColumn("Odd-even Sorted", ImGuiTableColumnFlags_None);
               ImGui::TableHeadersRow();

               ImGuiListClipper clipper;
               clipper.Begin(mUnsorted.size());
               while (clipper.Step())
               {
                  for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
                  {
                     ImGui::TableNextRow();
                     ImGui::TableNextColumn();
                     ImGui::Text("%d", row);
                     ImGui::TableNextColumn();
                     ImGui::Text("%d", mUnsorted[row]);
                     ImGui::TableNextColumn();
                     ImGui::Text("%d", mCpuSortResults[row]);
                     ImGui::TableNextColumn();
                     ImGui::Text("%d", mBitonicSortResults[row]);
                     ImGui::TableNextColumn();
                     ImGui::Text("%d", mOddEvenSortResults[row]);
                  }
               }
               ImGui::EndTable();//DATA
            }
            ImGui::TableNextColumn();
            if (ImGui::Button("Complexity Analysis"))
            {
               Complexity();
            }
            if (ImPlot::BeginPlot("Sort Performance")) {
               ImPlot::SetupAxes("size", "time (ms)", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
               ImPlot::SetNextMarkerStyle(ImPlotMarker_Square);
               ImPlot::PlotLine("CPU", mComplexitySize.data(), mCpuComplexityTimes.data(), mComplexitySize.size());
               ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
               ImPlot::PlotLine("GPU bitonic", mComplexitySize.data(), mBitonicComplexityTimes.data(), mComplexitySize.size());
               ImPlot::SetNextMarkerStyle(ImPlotMarker_Up);
               ImPlot::PlotLine("GPU odd-even", mComplexitySize.data(), mOddEvenComplexityTimes.data(), mComplexitySize.size());
               ImPlot::EndPlot();
            }
            ImGui::EndTable(); //COLUMNS
         }
         ImGui::End();
      }
   }
};

#include <numeric> //for partial_sum

struct ScanTest : public ComputeTest
{
   ComputeShader mBufferScanCs = ComputeShader("prefix_sum_cs.glsl");
   BufferScan mBufferScan;

   const int mNumItems = 1024;
   std::vector<int> mInput = std::vector<int>(mNumItems);
   std::vector<int> mCpuPartialSumResults = std::vector<int>(mNumItems);
   std::vector<int> mCpuScanResults = std::vector<int>(mNumItems);
   std::vector<int> mGpuScanResults = std::vector<int>(mNumItems);
   BufferArray mItemBuffer = BufferArray(GL_SHADER_STORAGE_BUFFER);
   Buffer mReadbackBuffer = Buffer(GL_SHADER_STORAGE_BUFFER);

   std::vector<double> mComplexitySize;
   std::vector<double> mGpuComplexitySize;
   std::vector<double> mCpuScanComplexityTimes;
   std::vector<double> mCpuPartialSumComplexityTimes;

   bool mValid = false;
   double mCpuScanTime = -1.0;
   double mCpuPartialSumTime = -1.0;
   double mGpuScanTime = -1.0;

   void Init() override
   {
      for (int i = 0; i < mNumItems; i++)
      {
         mInput[i] = int(2.0 * (1.0 + glm::sin(45234.6 * i)));
      }

      mItemBuffer.Init(mInput.size(), sizeof(int), mInput.data());
      mReadbackBuffer.Init(mInput.size() * sizeof(int));

      mBufferScanCs.Init();
      mBufferScan.SetComputeShader(mBufferScanCs);
      mBufferScan.Init();
      mValid = false;
   }

   bool Validate() override
   {
      Timer partial_sum_timer("std::partial_sum ");
      std::partial_sum(mInput.begin(), mInput.end(), mCpuPartialSumResults.begin());
      partial_sum_timer.Stop();
      mCpuPartialSumTime = partial_sum_timer.GetLastTimeMs();

      Timer inclusive_scan_timer("std::inclusive_scan ");
      std::inclusive_scan(mInput.begin(), mInput.end(), mCpuScanResults.begin());
      inclusive_scan_timer.Stop();
      mCpuScanTime = inclusive_scan_timer.GetLastTimeMs();

      if (mCpuPartialSumResults != mCpuScanResults)
      {
         std::cout << "Cpu methods disagree" << std::endl;
      }

      mItemBuffer.Init(mInput.size(), sizeof(int), mInput.data());
      mBufferScan.Compute(mItemBuffer);
      mReadbackBuffer.CopyFromBufferSubData(mItemBuffer);
      mReadbackBuffer.GetBufferSubData(mGpuScanResults.data());
      mGpuScanTime = mBufferScan.GetTimer().GetLastTimeMs(); //TODO use single timer

      //Be careful when validating. The GPU method is an exclusive scan, 
      // and the CPU methods are inclusive scans. 
      // If valid gpu_scan_results[1:end] == inclusive_scan_results[0:end-1];

      if (std::equal(mGpuScanResults.begin() + 1, mGpuScanResults.end(), mCpuScanResults.begin()))
      {
         mValid = true;
      }
      std::cout << "Scan validated = " << mValid << std::endl;
      return mValid;
   }
   void Complexity() override
   {
      int reps = 10;
      std::vector<int> test_items = std::vector<int>(mNumItems, 1); //all ones

      mComplexitySize.resize(0);
      for (int n = mNumItems; n > 1; n = n / 2)
      {
         mComplexitySize.push_back(n);
      }

      mGpuComplexitySize.resize(0);
      for (int i = 0; i < mComplexitySize.size(); i++)
      {
         double time_sum = 0.0;
         for (int r = 0; r < reps; r++)
         {
            BufferArray test_buffer(GL_SHADER_STORAGE_BUFFER);
            test_buffer.Init(mComplexitySize[i], sizeof(int), test_items.data());
            mBufferScan.Compute(test_buffer);
            test_buffer.Free();
            time_sum += mBufferScan.GetTimer().GetLastTimeMs();
         }
         mGpuComplexitySize.push_back(time_sum / reps);
      }

      mCpuPartialSumComplexityTimes.resize(0);
      for (int i = 0; i < mComplexitySize.size(); i++)
      {
         double time_sum = 0.0;
         for (int r = 0; r < reps; r++)
         {
            std::vector<int> partial_sum_results(mNumItems);
            Timer partial_sum_timer("std::partial_sum ");
            std::partial_sum(test_items.begin(), test_items.begin() + mComplexitySize[i], partial_sum_results.begin());
            partial_sum_timer.Stop();
            time_sum += partial_sum_timer.GetLastTimeMs();
         }
         mCpuPartialSumComplexityTimes.push_back(time_sum / reps);
      }

      mCpuScanComplexityTimes.resize(0);
      for (int i = 0; i < mComplexitySize.size(); i++)
      {
         double time_sum = 0.0;
         for (int r = 0; r < reps; r++)
         {
            std::vector<int> inclusive_scan_results(mNumItems);
            Timer inclusive_scan_timer("std::inclusive_scan ");
            std::inclusive_scan(test_items.begin(), test_items.begin() + mComplexitySize[i], inclusive_scan_results.begin());
            inclusive_scan_timer.Stop();
            time_sum += inclusive_scan_timer.GetLastTimeMs();
         }
         mCpuScanComplexityTimes.push_back(time_sum / reps);
      }

      std::ofstream perf_file;
      perf_file.open("scan_perf.txt");
      perf_file << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
      perf_file << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
      perf_file << "Size \t partial_sum \t inclusive_scan \t GPU \n";
      for (int i = 0; i < mComplexitySize.size(); i++)
      {
         perf_file << std::to_string(mComplexitySize[i]) << "\t";
         perf_file << std::to_string(mCpuPartialSumComplexityTimes[i]) << "\t";
         perf_file << std::to_string(mCpuScanComplexityTimes[i]) << "\t";
         perf_file << std::to_string(mGpuComplexitySize[i]) << "\n";
      }
      perf_file.close();
   }

   void DrawGui() override
   {
      if (mGuiOpen == false) return;
      ImGui::Begin("Scan Test", &mGuiOpen);

      if (ImGui::BeginTable("##COLUMNS", 2))
      {
         ImGui::TableNextColumn();
         if (ImGui::Button("Validate"))
         {
            Validate();
         }
         ImGui::SameLine(); ImGui::Text("Valid = %d", mValid);

         ImGui::Text("CPU scan time = %f ms", mCpuScanTime);
         ImGui::Text("CPU partial sum time = %f ms", mCpuPartialSumTime);
         ImGui::Text("GPU scan time = %f ms", mGpuScanTime);
         ImGui::Text("The GPU method is an exclusive scan, and the CPU methods are inclusive scans.");
         if (ImGui::BeginTable("Data", 5, ImGuiTableFlags_ScrollY))
         {
            ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("CPU Scan", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("CPU Partial Sum", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("GPU Scan", ImGuiTableColumnFlags_None);
            ImGui::TableHeadersRow();

            ImGuiListClipper clipper;
            clipper.Begin(mInput.size());
            while (clipper.Step())
            {
               for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
               {
                  ImGui::TableNextRow();
                  ImGui::TableNextColumn();
                  ImGui::Text("%d", row);
                  ImGui::TableNextColumn();
                  ImGui::Text("%d", mInput[row]);
                  ImGui::TableNextColumn();
                  ImGui::Text("%d", mCpuScanResults[row]);
                  ImGui::TableNextColumn();
                  ImGui::Text("%d", mCpuPartialSumResults[row]);
                  ImGui::TableNextColumn();
                  ImGui::Text("%d", mGpuScanResults[row]);
               }
            }
            ImGui::EndTable();//DATA
         }
         ImGui::TableNextColumn();
         if (ImGui::Button("Complexity Analysis"))
         {
            Complexity();
         }
         if (ImPlot::BeginPlot("Scan Performance")) {
            ImPlot::SetupAxes("size", "time (ms)", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Square);
            ImPlot::PlotLine("CPU scan", mComplexitySize.data(), mCpuScanComplexityTimes.data(), mComplexitySize.size());
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Up);
            ImPlot::PlotLine("CPU partial sum", mComplexitySize.data(), mCpuPartialSumComplexityTimes.data(), mComplexitySize.size());
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
            ImPlot::PlotLine("GPU", mComplexitySize.data(), mGpuComplexitySize.data(), mComplexitySize.size());
            ImPlot::EndPlot();
         }
         ImGui::EndTable(); //COLUMNS
      }
      ImGui::End();
   }
};
