#include <iostream>
#include <tuple>
#include <utility>

template< class T >
struct BiTree
{
  T val;
  BiTree< T >* lt;
  BiTree< T >* rt;
  BiTree< T >* parent;
  BiTree(T val);
};

template< class T >
BiTree< T >::BiTree(T val):
  val(val),
  lt(nullptr),
  rt(nullptr),
  parent(nullptr)
{}

enum Dir 
{
  fall_left,
  parent_dir
};

template< class T >
BiTree<T>* makeNode(T val)
{
  return new BiTree< T >{val};
}

template< class T >
void connect(BiTree< T >* parent, BiTree< T >* left, BiTree< T >* right)
{
  parent->lt = left;
  parent->rt = right;
  if (left){
    left->parent = parent;
  }
  if (right){
    right->parent = parent;
  }
}

template< class T >
void clear(BiTree< T >* root) {
  if (!root) return;
  clear(root->lt);
  clear(root->rt);
  delete root;
}

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
  while( p && p->rt != node)
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
  if (!res.second)
  {
    return { parent_dir, res.first, nullptr };
  }
  return { parent_dir, res.first, res.second };
}

template< class T >
BiTree< T >* maximum(BiTree< T >* node)
{
  while (node && node->rt)
  {
    node = node->rt;
  }
  return node;
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
  if (!lhs || !rhs)
  {
  return {nullptr, nullptr, false};
  }
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
  bool same_dir = std::get< 0 >(ln) == std::get< 0 >(rn) && std::get<2>(rn) == nullptr;
  return { std::get< 2 >(ln), std::get< 2 >(rn), same_dir };
}

template< class T >
std::pair< BiTree< T >*, BiTree< T >* > inclusionStart(BiTree< T >* start, BiTree< T >* pattern)
{
  if (!start) return {nullptr, nullptr};
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

  if (!next_start)
  {
    return { {nullptr, nullptr} };
  }

  return { inclusionStart(next_start, pattern) };
}

template< class T >
bool hasNext(InclusionIt<T> it, BiTree<T>* pattern)
{
  if (!it.incl.first) return false;

  BiTree<T>* next_start = std::get<2>(nextStruct(it.incl.first));

  if (!next_start) return false;
  std::pair< BiTree< T >*, BiTree< T >* > res = inclusionStart(next_start, pattern);

  return res.first != nullptr;
}

template < class T >
void printTree(BiTree< T >* n, int d = 0)
{
  if (!n) return;
  std::cout << std::string(d * 2, ' ') << n->val << "\n";
  if (n->lt)
  {
    std::cout << std::string(d * 2, ' ') << "L:";
    printTree(n->lt, d + 1);
  }
  if (n->rt)
  {
    std::cout << std::string(d * 2, ' ') << "R:";
    printTree(n->rt, d + 1);
  }
}

int main() 
{
  BiTree< int >* n1 = makeNode(1);
  BiTree< int >* n4 = makeNode(4);
  BiTree< int >* n3 = makeNode(3);
  BiTree< int >* n6 = makeNode(6);
  BiTree< int >* n2 = makeNode(2);
  BiTree< int >* n5 = makeNode(5);

  connect< int >(n1, (BiTree<int>*)nullptr, n4);
  connect< int >(n4, n3, n6);
  connect< int >(n3, n2, (BiTree<int>*)nullptr);
  connect< int >(n6, n5, (BiTree<int>*)nullptr);

  BiTree< int >* p2 = makeNode(2);


  std::cout << "LHS tree:\n";
  printTree(n1);

  std::cout << "\nPattern:\n";
  printTree(p2);

  std::cout << "\nMatches:\n";

  InclusionIt< int > it = begin(n1, p2);
  if (!it.incl.first)
  {
  std::cout << "No matches\n";
  }
  while (it.incl.first)
  {
    std::cout << "Match from " << it.incl.first->val << " to " << it.incl.second->val << "\n";
    it = next(it, p2);
  }

  clear(n1);
  clear(p2);

  return 0;
}