#include <iostream>
#include <vector>
#include <array>
#include <iterator>
#include <limits>
#include <algorithm>
#include <cassert>
#include <fstream>


template<class TFirst, class TSecond>
std::istream& operator>>(std::istream& is, std::pair<TFirst, TSecond>& p)
{
   is >> p.first;
   is >> p.second;
   return is;
}


template<typename T>
T Read(std::istream& is)
{
   T v{};
   is >> v;
   return v;
}


// перекрёсток - это массив из 4 дорог (индексов других перекрёстков)
using cross_t = std::array<std::uint32_t, 4>;

/*
* FordBellman - алгоритм Форда-Беллмана
* @param graph - граф (индексировать с единицы)
* @param from - откуда строить путь
* @param to - куда строить путь
* @return - минимальный путь в графе от точки from до точки to
*/
std::uint32_t FordBellman(const std::vector<cross_t>& graph, std::uint32_t from, std::uint32_t to)
{
   constexpr std::uint32_t maxWeight = std::numeric_limits<std::uint32_t>::max() - 1;
   std::vector<std::uint32_t> weights(graph.size(), maxWeight);
   weights[from] = 0;
   
   // ВНИМАНИЕ: индексация начинается не с нуля а с единицы для удобства
   for (std::size_t first = 1; first < graph.size(); ++first)
   {
      for (std::size_t k = 0; k < 4; ++k)
      {
         const auto second = graph[first][k];
         if (second == 0)
            break;

         if (weights[first] > weights[second] + 1)
            weights[first] = weights[second] + 1;
         else if (weights[second] > weights[first] + 1)
            weights[second] = weights[first] + 1;
      }
   }

   return weights[to];
}

void AddRoad(cross_t& crossroad, std::uint32_t nextCrossroad)
{
   // ищем первый ноль в массиве дорог
   auto it = std::find(crossroad.begin(), crossroad.end(), 0);
   // assert(it != crossroad.end());
   // ВАЖНО: всегда должен найтись 0 - иначе переполнение

   // пишем вместо нуля к какому перекрёстку ведёт дорога
   *it = nextCrossroad;
}

/*
* ReadConnections - считать соединения и построить из них граф
* @param n - количество перекрёстков
* @param m - количество соединений
*/
std::vector<cross_t> ReadConnections(std::istream& is, std::size_t n, std::size_t m)
{
   // +1 запас для того чтобы индексировать с единицы - нулевой элемент не используется
   std::vector<cross_t> graph(n + 1);
   for (std::size_t i = 0; i < m; ++i)
   {
      const auto [first, second] = Read<std::pair<std::uint32_t, std::uint32_t>>(is);

      // добавляем дороги от first к second 
      AddRoad(graph[first], second);

      // и от second к first
      AddRoad(graph[second], first);
   }
   
   return graph;
}


int main()
{
   using cross_t = std::array<std::uint32_t, 4>;

   std::fstream file;
   file.open("input.txt", std::ios_base::in);

   if (!file)
   {
      std::cerr << "File not found\n";
      return -1;
   }

   const auto n = Read<std::uint32_t>(file);
   const auto m = Read<std::uint32_t>(file);

   const auto graph = ReadConnections(file, n, m);

   // + 1, потому что первая вершина тоже является неизбежным перекрёстком
   std::cout << FordBellman(graph, 1, 2) + 1 << std::endl;
   return 0;
}
