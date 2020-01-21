
#include "neuroevolution/neuro_net.hpp"
#include "neuroevolution/rng.hpp"
#include "LinesPG.hpp"

class LinesFitnessEvaluator : public neuroevolution::IFitnessEvaluator
{
public:
   LinesFitnessEvaluator()
   {
       
   }

   void step()
   {
       
   }

   void genPicture(const bool vert, double picture[5][5])
   {
      auto pos = Rng::genChoise(5);

      for(int i = 0; i < 5; ++i)
      {
         for(int j = 0; j < 5; ++j)
         {
            picture[i][j] = 0.0;
         }
      }

      if(vert)
      {
         for(int i = 0; i < 5; ++i)
         {
            picture[pos][i] = 1.0;
         }
      }
      else
      {
         for(int i = 0; i < 5; ++i)
         {
            picture[i][pos] = 1.0;
         }
      }
   }

   neuroevolution::Fitness evaluate(neuroevolution::NeuroNet& ann) override
   {
        neuroevolution::Fitness result = 0;

        double picture[5][5];

        for(int i = 0; i < 100; ++i)
        {
            ann.reset();

            bool vert = Rng::genProbability(0.5);
            genPicture(vert, picture);

            auto inputIter = ann.begin_input();

            for (unsigned int i = 0; i < 5; i++)
            {
               for (unsigned int j = 0; j < 5; j++)
               {
                  *inputIter = picture[i][j];
                  ++inputIter;
               }
            }

            ann.activate();

            auto outputIter = ann.begin_output();
            auto vertProb = *outputIter;
            ++outputIter;
            auto horzProb = *outputIter; 

            if(vertProb > horzProb)
            {
                result += vert ? 1 : 0;
            }
            else
            {
                result += !vert ? 1 : 0;
            }
        }

        return result;
   }
};

neuroevolution::IFitnessEvaluator& LinesPG::getFitnessEvaluator()
{
   return *mFitnessEvaluator;
}

LinesPG::LinesPG()
: mFitnessEvaluator(new LinesFitnessEvaluator)
{
   
}

unsigned int LinesPG::getNumInputs() const
{
   return 25;
}

unsigned int LinesPG::getNumOutputs() const
{
   return 2;
}

void LinesPG::step()
{
   mFitnessEvaluator->step();
}

void LinesPG::play(neuroevolution::NeuroNet& ann)
{  
   
}

std::string LinesPG::getName() const
{
   return "Lines";
}

neuroevolution::DomainGeometry LinesPG::getDomainGeometry() const
{
   neuroevolution::DomainGeometry result;

   result.size = {5, 5, 3};
   result.outputs = {{0, 0}, {4, 4}};

   for (unsigned int i = 0; i < 5; i++)
   {
      for (unsigned int j = 0; j < 5; j++)
      {
         result.inputs.push_back({i, j});
      }
   }

   return result;
}