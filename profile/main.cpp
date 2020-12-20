#include <iostream>
#include <neuroevolution/neuro_net2.hpp>
#include <neuroevolution/rng.hpp>

#include <ctime>

int main()
{
    /*std::vector<neuroevolution::NodeId> inputs;
    for(int i = 0; i < 1000; ++i)
    {
        inputs.push_back(i);
    }

    std::vector<neuroevolution::NodeId> outputs;
    for(int i = 1000; i < 1005; ++i)
    {
        outputs.push_back(i);
    }

    std::vector<neuroevolution::NeuroNet2::HiddenNodeDef> hiddens;
    for(int i = 1005; i < 4000; ++i)
    {
        hiddens.push_back({i, ActivationFunctionType::SIGMOID, 0.0});
    }

    std::vector<neuroevolution::NeuroNet2::ConnectionDef> cons;
    for(int i = 0; i < 1000; ++i)
    {
        for(int j = 1005; j < 4000; ++j)
        {
            cons.push_back({i, j, Rng::genWeight()});
        }
    }

    for(int i = 1005; i < 4000; ++i)
    {
        for(int j = 1000; j < 1005; ++j)
        {
            cons.push_back({i, j, Rng::genWeight()});
        }
    }

    bool connect = false;
    for(int i = 1005; i < 4000; ++i)
    {
        for(int j = 1005; j < 4000; ++j)
        {
            connect = !connect;
            if(connect)
            {
                cons.push_back({i, j, Rng::genWeight()});
            }
        }
    }

    std::clock_t c_start = std::clock();

    neuroevolution::NeuroNet2 nn(inputs, outputs, hiddens, cons);

    std::clock_t c_end = std::clock();

    auto time_elapsed_ms = 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC;
    std::cout << "CPU time used: " << time_elapsed_ms << " ms\n";

    c_start = std::clock();

    for(int i = 0; i < 10000; ++i)
    {
        nn.activate();
    }

    c_end = std::clock();

    time_elapsed_ms = 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC;
    std::cout << "CPU time used: " << time_elapsed_ms << " ms\n";*/

    return 0;
}