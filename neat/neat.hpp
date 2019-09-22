namespace neat
{

class NeatResult
{

};

class IFitnessEvaluator
{

};

class IObserver
{

};

struct Config
{
    
};

class Neat
{
public:
    static NeatResult evolve(const Config& cfg, IFitnessEvaluator& fitnessEvaluator, IObserver& observer);
};

}