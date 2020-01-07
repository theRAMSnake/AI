#include "./CheckpointPG.hpp"
#include "neuroevolution/neuro_net.hpp"
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/textbox.hpp>

bool sameCorridor(int a, int b)
{
   return (a - 1) / 2 == (b - 1) / 2;
}

class CPFitnessEvaluator : public neuroevolution::IFitnessEvaluator
{
public:
   CPFitnessEvaluator()
   {
      for(auto& f: std::filesystem::directory_iterator("cp_samples"))
      {
         mSamples.push_back(loadSample(f.path()));
      }
   }

   void step()
   {
      
   }

   std::string play(neuroevolution::NeuroNet& ann)
   {
      std::string result;
      int numSolved = 0;

      for(auto &s: mSamples)
      {
         for(auto &m : s.measurements)
         {
            auto inputIter = ann.begin_input();

            inputIter = std::copy(m.pscs, m.pscs + 13, inputIter);
            *inputIter = m.timeDelta;

            ann.activate();
         }

         auto pos = std::max_element(ann.begin_output(), ann.end_output());
         auto counter = std::distance(ann.begin_output(), pos) + 1;

         if(counter == s.result)
         {
            result += "+";
            numSolved++;
         }
         else
         {
            result += "-";
         }

         if(result.size() % 25 == 0)
         {
            result += "\n";
         }
      }

      result += "\nSolved " + std::to_string(numSolved) + "/" + std::to_string(mSamples.size());

      return result;
   }

   neuroevolution::Fitness evaluate(neuroevolution::NeuroNet& ann) override
   {
      neuroevolution::Fitness result = 0;

      for(auto &s: mSamples)
      {
         for(auto &m : s.measurements)
         {
            auto inputIter = ann.begin_input();

            inputIter = std::copy(m.pscs, m.pscs + 13, inputIter);
            *inputIter = m.timeDelta;

            ann.activate();
         }

         auto pos = std::max_element(ann.begin_output(), ann.end_output());
         auto counter = std::distance(ann.begin_output(), pos) + 1;

         if(counter == s.result)
         {
            result ++;
         }
      }

      return result;
   }

private:
   struct Measurement
   {
      double pscs[13];
      double timeDelta;
   };

   struct Sample
   {
      int result;
      std::vector<Measurement> measurements;
   };

   int getSensorIndex(const int sensor)
   {
      switch(sensor)
      {
         case 1: return 0;
         case 11: return 1;
         case 2: return 2;
         case 21: return 3;
         case 3: return 4;
         case 31: return 5;
         case 4: return 6;
         case 41: return 7;
         case 5: return 8;
         case 51: return 9;
         case 6: return 10;
         case 61: return 11;
         case 170: return 12;
         default:
            throw-1;
      }
   }

   double dbToLinear(const double x)
   {
      return std::pow(10, x / 10);
   }

   std::time_t extractTs(const std::string& s)
   {
      auto pos = s.find("(");
      auto pos2 = s.find(")");

      auto result = std::atoll(s.substr(pos + 1, pos2 - pos - 1).c_str());
      return result;
   }

   Sample loadSample(const std::filesystem::path& p)
   {
      std::time_t prevTs = 0;
      Sample result;

      boost::property_tree::ptree tree;
      boost::property_tree::read_json(p.string().c_str(), tree);

      result.result = tree.get<int>("TrainData.Counter.OrdinalNumber");

      const boost::property_tree::ptree& meas = tree.get_child("Measurements");
      for( const auto& kv : meas )
      {
         Measurement m;
         memset(&m, 0, sizeof(Measurement));

         auto ts = extractTs(kv.second.get<std::string>("Timestamp"));

         if(prevTs == 0)
         {
            prevTs = ts;
         }

         m.timeDelta = double(ts - prevTs) / 1000;

         prevTs = ts;

         const boost::property_tree::ptree& details = kv.second.get_child("MeasurementReportDetails");
         for( const auto& detail : details )
         {
            m.pscs[getSensorIndex(detail.second.get<int>("SensorId"))] =
               dbToLinear(detail.second.get<double>("Ecn0.Db"));
         }

         result.measurements.push_back(m);
      }

      return result;
   }

   std::vector<Sample> mSamples;
};

neuroevolution::IFitnessEvaluator& CheckpointPG::getFitnessEvaluator()
{
   return *mFitnessEvaluator;
}

CheckpointPG::CheckpointPG()
: mFitnessEvaluator(new CPFitnessEvaluator)
{
   
}

unsigned int CheckpointPG::getNumInputs() const
{
   return 14;
}

unsigned int CheckpointPG::getNumOutputs() const
{
   return 6;
}

void CheckpointPG::step()
{
   mFitnessEvaluator->step();
}

void CheckpointPG::play(neuroevolution::NeuroNet& ann)
{
   nana::form frm(nana::API::make_center(500, 800));
   frm.caption("Checkpoint output");

   nana::textbox tb(frm);
   std::string out = mFitnessEvaluator->play(ann);
   tb.caption(out);

   nana::place layout(frm);
   layout.div("vert <a weight=10><vert d margin=10 gap=10>");
   layout.field("d") << tb;
   layout.collocate();

   frm.modality();
}

std::string CheckpointPG::getName() const
{
   return "Checkpoint";
}