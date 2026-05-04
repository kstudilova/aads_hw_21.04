#include <iostream>
#include <tuple>
#include <utility>

template< class T >
struct BiTree
{
  T value;
  BiTree< T >* lt;
  BiTree< T >* rt;
  BiTree< T >* parent;
};

enum Dir 
{
  fall_left,
  parent_dir
};

template< class T >
std::pair< size_t, BiTree< T >* > fallLeft(BiTree< T >* node)
{
  size_t len = 0;
  while (node && node->lt)
  {
    node = node->lt;
    len++;
  }
  return { len, node };
}

template< class T >
std::pair< size_t, BiTree< T >* > goParent( BiTree< T >* node)
{
  if (!node)
  {
    return {0, nullptr};
  }
  size_t len = 0;
  BiTree< T >* p = node->parent;
  while( node && p->rt != node)
  {
    node = p;
    p = node->parent;
    len++;
  }
  return { len, p };
}

template< class T >
std::tuple< Dir, size_t, BiTree< T >* > nextStruct( BiTree< T >* node)
{
  if (!node)
  {
    return { parent_dir, 0, nullptr };
  }
  if (node->rt) 
  {
    std::pair< size_t, BiTree< T >* > res = fallLeft(node->rt);
    return { fall_left, res.first, res.second };
  }
  std::pair< size_t, BiTree< T >* > res = goParent(node);
  return { parent_dir, res.first, res.second };
}

template< class T >
BiTree< T >* maximum(BiTree< T >* node)
{
  while (node && node->rt)
  {
    node = node->rt;
    return node;
  }
}

template< class T >
BiTree< T >* prev(BiTree< T >* node)
{
  if (!node)
  {
    return nullptr;
  }
  if (node->lt)
  {
    return maximum(node->lt);
  }
  BiTree< T >* p = node->parent;
  while (p && p->lt == node)
  {
    node = p;
    p = node->parent;
  }
  return p;
}

template< class T >
std::tuple< BiTree< T >*, BiTree< T >*, bool > isEqualStructStart(BiTree< T >* lhs, BiTree< T >* rhs)
{
  std::tuple< Dir, size_t, BiTree< T >* > ln = nextStruct(lhs);
  std::tuple< Dir, size_t, BiTree< T >* > rn = nextStruct(rhs);
  while (
    std::get< 0 >(ln) == std::get< 0 >(rn) &&
    std::get< 1 >(ln) == std::get< 1 >(rn) &&
    std::get< 2 >(ln) &&
    std::get< 2 >(rn)
  ) {
    ln = nextStruct(std::get< 2 >(ln));
    rn = nextStruct(std::get< 2 >(rn));
  }
  bool same_dir = std::get< 0 >(ln) == std::get< 0 >(rn);
  return { std::get< 2 >(ln), std::get< 2 >(rn), same_dir };
}

template< class T >
std::pair< BiTree< T >*, BiTree< T >* > inclusionStart(BiTree< T >* start, BiTree< T >* pattern)
{
  BiTree<  T>* pattern_begin = fallLeft(pattern).second;
  BiTree< T >* curr = start;
  while (curr)
  {
    std::tuple< BiTree< T >*, BiTree< T >*, bool > result = isEqualStructStart(curr, pattern_begin);
    if (!std::get< 1 >(result) && std::get< 2 >(result))
    {
      BiTree< T >* last_next = std::get< 0 >(result);
      if (!last_next)
      {
        BiTree< T >* end = curr;
        while (end->rt)
        {
          end = end->rt;
        }
        return { curr, end };
      }
      return { curr, prev(last_next) };
    }
    curr = std::get< 2 >(nextStruct(curr));
  }
  return { nullptr, nullptr };
}

template< class T >
std::pair< BiTree< T >*, BiTree< T >* > inclusion(BiTree< T >* lhs, BiTree< T >* pattern)
{
  if (!lhs || !pattern)
  {
    return {nullptr, nullptr};
  }
  BiTree< T >* lhs_begin = fallLeft(lhs).second;

  return inclusionStart(lhs_begin, pattern);
}

template< class T >
struct InclusionIt
{
  std::pair< BiTree< T >*, BiTree< T >* > incl;
};

template< class T >
InclusionIt< T > begin(BiTree< T >* lhs, BiTree< T >* pattern)
{
  return { inclusion(lhs, pattern) };
}

template< class T >
InclusionIt< T > next(InclusionIt< T > it, BiTree< T >* pattern)
{
  if (!it.incl.first) return it;

  BiTree< T >* next_start = std::get<2>(nextStruct(it.incl.first));

  return { inclusionStart(next_start, pattern) };
}

template< class T >
bool hasNext(InclusionIt<T> it, BiTree<T>* pattern)
{
  if (!it.incl.first) return false;

  BiTree<T>* next_start = std::get<2>(nextStruct(it.incl.first));

  auto res = inclusionStart(next_start, pattern);

  return res.first != nullptr;
}

int main() 
{
  
}