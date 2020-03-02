#pragma once

enum class Engine
{
   Neat,
   HyperNeat,
   SnakeGA,
   Seg
};

inline std::string to_string(const Engine engine)
{
   if(engine == Engine::Neat)
   {
      return "Neat";
   }
   else if(engine == Engine::HyperNeat)
   {
      return "HyperNeat";
   }
   else if(engine == Engine::SnakeGA)
   {
      return "SnakeGA";
   }
   else if(engine == Engine::Seg)
   {
      return "SEG";
   }

   throw -1;
}

inline Engine to_engine(const std::string& str)
{
   if(str == "Neat")
   {
      return Engine::Neat;
   }
   else if(str == "HyperNeat")
   {
      return Engine::HyperNeat;
   }
   else if(str == "SnakeGA")
   {
      return Engine::SnakeGA;
   }
   else if(str == "SEG")
   {
      return Engine::Seg;
   }

   throw -1;
}