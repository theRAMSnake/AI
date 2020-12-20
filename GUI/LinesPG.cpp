
#include "neuroevolution/neuro_net2.hpp"
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

   void genPicture(const bool vert, double picture[25])
   {
      auto pos = Rng::genChoise(5);

      for(int i = 0; i < 5; ++i)
      {
         for(int j = 0; j < 5; ++j)
         {
            picture[i * 5 + j] = 0.0;
         }
      }

      if(vert)
      {
         for(int i = 0; i < 5; ++i)
         {
            picture[pos * 5 + i] = 1.0;
         }
      }
      else
      {
         for(int i = 0; i < 5; ++i)
         {
            picture[i * 5 + pos] = 1.0;
         }
      }
   }

   neuroevolution::Fitness evaluate(neuroevolution::IAgent& agent) override
   {
        neuroevolution::Fitness result = 0;

        double picture[25];

        for(int i = 0; i < 100; ++i)
        {
            agent.reset();

            bool vert = Rng::genProbability(0.5);
            genPicture(vert, picture);

            auto actRes = agent.run(picture);

            if(actRes)
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

void LinesPG::play(neuroevolution::IAgent& agent)
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