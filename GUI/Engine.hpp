#pragma once

enum class Engine
{
   Neat,
   HyperNeat,
   SnakeGA
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

   throw -1;
}