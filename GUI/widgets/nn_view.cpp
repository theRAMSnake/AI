#include "nn_view.hpp"

Nn_view::Nn_view(std::unique_ptr<neat::NeuroNet>&& ann)
: mForm(nana::API::make_center(1000, 800))
, mDraw(mForm)
, mAnn(ann)
{
   mForm.caption("ANN View");

   mDraw.draw([=](nana::paint::graphics& graph){
		
      auto topology = mAnn->createTopology();

      const int widthPerLayer = graph.width() / topology.getNumLayers();

      for(int i = 0; i < topology.getNumLayers(); ++i)
      {
         auto layerNodes = topology.getLayer(i);

         const int x = widthPerLayer * i + widthPerLayer / 2;
      }

	});

   mForm.show();

   mForm.events.close([=](){delete this;});
}