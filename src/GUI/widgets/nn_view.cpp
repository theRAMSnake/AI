#include "nn_view.hpp"

nana::colors colorByWeight(const double weight)
{
   if(weight < -0.25)
   {
      return nana::colors::red;
   }

   if(weight > 0.25)
   {
      return nana::colors::green;
   }

   return nana::colors::blue;
}

Nn_view::Nn_view(std::unique_ptr<gacommon::IAgent>&& ann)
: mForm(nana::API::make_center(1000, 800))
, mDraw(mForm)
, mAnn(std::move(ann))
{
   mForm.caption("ANN View");

   mDraw.draw([=](nana::paint::graphics& graph){
		
      auto topology = static_cast<gacommon::NNAgent&>(*mAnn).getNN().createTopology();
      std::map<gacommon::NodeId, nana::point> posPerNode;

      const int widthPerLayer = graph.width() / topology.getNumLayers();

      for(std::size_t i = 0; i < topology.getNumLayers(); ++i)
      {
         auto layerNodes = topology.getLayer(i);

         if(layerNodes.empty())
         {
            continue;
         }

         const int x = widthPerLayer * i + widthPerLayer / 2;
         const int heightPerRow = graph.height() / layerNodes.size();

         if(heightPerRow < 7)
         {
            graph.rectangle(nana::rectangle(x - 2, 25, 5, graph.height() / 2 - 25), true, nana::colors::black);
            graph.string(nana::point(x - 10, graph.height() / 2 + 5), std::to_string(layerNodes.size()), nana::colors::blue);
            graph.rectangle(nana::rectangle(x - 2, graph.height() / 2 + 25, 5, graph.height() / 2 - 50), true, nana::colors::black);

            for(std::size_t j = 0; j < layerNodes.size(); ++j)
            {
               const int y = heightPerRow * j + heightPerRow / 2;
               posPerNode[layerNodes[j].id] = nana::point(x, y);
            }
         }
         else
         {
            for(std::size_t j = 0; j < layerNodes.size(); ++j)
            {
               const int y = heightPerRow * j + heightPerRow / 2;

               graph.round_rectangle(nana::rectangle(x - 2, y - 2, 5, 5), 5, 5, nana::colors::black, true, nana::colors::black);

               posPerNode[layerNodes[j].id] = nana::point(x, y);
            }
         }
      }

      for(std::size_t i = 0; i < topology.getNumLayers(); ++i)
      {
         auto layerNodes = topology.getLayer(i);

         if(layerNodes.empty())
         {
            continue;
         }

         for(std::size_t j = 0; j < layerNodes.size(); ++j)
         {
            auto nodePos = posPerNode[layerNodes[j].id];
            for(auto& c : layerNodes[j].inputs)
            {
               graph.line(posPerNode[c.first], nodePos, colorByWeight(c.second));

               if(c.first == layerNodes[j].id)
               {
                  graph.string(nana::point{nodePos.x + 10, nodePos.y}, "r", nana::colors::black);
               }
            }
         }
      }
	});

   mForm.show();
}

Nn_view::~Nn_view()
{
   mForm.close();
}
