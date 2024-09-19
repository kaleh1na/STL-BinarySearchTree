
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>

struct inorder_tag {};
struct preorder_tag {};
struct postorder_tag {};

template <typename T, typename Compare = std::less<T>,
          typename Allocator = std::allocator<T>>
class BinarySearchTree {
 private:
  class Node;

  struct BaseNode {
    Node* left;
    Node* right;
    Node* parent;
    BaseNode() : left(nullptr), right(nullptr), parent(nullptr) {}
  };

  struct Node : public BaseNode {
    T key;
    Node(const T& key) : key(key) {}
  };

  template <typename traversal_type = inorder_tag>
  class base_iterator {
    friend BinarySearchTree;

   public:
    using value_type = const T;
    using key_type = const T;
    using pointer_type = const T*;
    using reference_type = const T&;
    using difference_type = size_t;

   private:
    const BaseNode* ptr;
    base_iterator(const BaseNode* ptr) : ptr(ptr) {}

    bool is_base_node(const BaseNode* node) {
      return (node == node->parent ||
              (node != node->parent->left && node != node->parent->right));
    }

    base_iterator& increment(inorder_tag) {
      if (ptr->right != nullptr) {
        ptr = ptr->right;
        while (ptr->left != nullptr) {
          ptr = ptr->left;
        }
      } else {
        node_type* par = ptr->parent;
        while (!is_base_node(par) && (par->right == ptr)) {
          ptr = par;
          par = par->parent;
        }
        ptr = par;
      }
      return *this;
    }

    base_iterator& increment(preorder_tag) {
      if (ptr->left != nullptr) {
        ptr = ptr->left;
      } else if (ptr->right != nullptr) {
        ptr = ptr->right;
      } else {
        node_type* par = ptr->parent;
        while (!(par->left == ptr && par->right != nullptr)) {
          ptr = par;
          par = par->parent;
        }
        if (!is_base_node(par)) {
          ptr = par->right;
        } else {
          ptr = par;
        }
      }
      return *this;
    }

    base_iterator& increment(postorder_tag) {
      node_type* par = ptr->parent;
      if (!(par->right == ptr || par->right == nullptr || is_base_node(par))) {
        par = par->right;
        while (!(par->left == nullptr && par->right == nullptr)) {
          if (par->left != nullptr) {
            par = par->left;
          } else {
            par = par->right;
          }
        }
      }
      ptr = par;
      return *this;
    }

    base_iterator& decrement(inorder_tag) {
      if (ptr->left != nullptr) {
        ptr = ptr->left;
        while (ptr->right != nullptr) {
          ptr = ptr->right;
        }
      } else {
        node_type* par = ptr->parent;
        while (!is_base_node(par) && (par->left == ptr)) {
          ptr = par;
          par = par->parent;
        }
        ptr = par;
      }
      return *this;
    }

    base_iterator& decrement(preorder_tag) {
      node_type* par;
      if (is_base_node(ptr)) {
        par = const_cast<Node*>(static_cast<const Node*>(ptr));
      } else {
        par = ptr->parent;
      }
      if (!(par->left == ptr || par->left == nullptr)) {
        par = par->left;
        while (!(par->left == nullptr && par->right == nullptr)) {
          if (par->right != nullptr) {
            par = par->right;
          } else {
            par = par->left;
          }
        }
      }
      ptr = par;
      return *this;
    }

    base_iterator& decrement(postorder_tag) {
      if (is_base_node(ptr)) {
        ptr = ptr->left;
        return *this;
      }
      if (ptr->right != nullptr) {
        ptr = ptr->right;
      } else if (ptr->left != nullptr) {
        ptr = ptr->left;
      } else {
        node_type* par = ptr->parent;
        while (!(par->right == ptr && par->left != nullptr) &&
               !is_base_node(par)) {
          ptr = par;
          par = par->parent;
        }
        ptr = par->left;
      }
      return *this;
    }

   public:
    base_iterator(const base_iterator&) = default;
    base_iterator& operator=(const base_iterator&) = default;

    bool operator==(const base_iterator& other) const = default;
    bool operator!=(const base_iterator& other) const = default;

    reference_type operator*() const {
      return (static_cast<const Node*>(ptr))->key;
    }
    pointer_type operator->() const {
      return &((static_cast<const Node*>(ptr))->key);
    }

    base_iterator& operator++() { return increment(traversal_type{}); }

    base_iterator operator++(int) {
      base_iterator copy = *this;
      ++(*this);
      return copy;
    }

    base_iterator& operator--() { return decrement(traversal_type{}); }

    base_iterator operator--(int) {
      base_iterator copy = *this;
      --(*this);
      return copy;
    }
  };

 public:
  template <typename traversal_type = inorder_tag>
  using iterator = base_iterator<traversal_type>;
  template <typename traversal_type = inorder_tag>
  using const_iterator = base_iterator<traversal_type>;
  template <typename traversal_type = inorder_tag>
  using reverse_iterator = std::reverse_iterator<iterator<traversal_type>>;
  template <typename traversal_type = inorder_tag>
  using const_reverse_iterator =
      std::reverse_iterator<const_iterator<traversal_type>>;

  using reference = T&;
  using const_reference = const T&;
  using key_type = T;
  using key_compare = Compare;
  using value_type = T;
  using value_compare = Compare;
  using node_type = Node;
  using allocator_type = Allocator;
  using size_type = size_t;

  template <typename traversal_type = inorder_tag>
  using difference_type =
      std::iterator_traits<iterator<traversal_type>>::difference_type;

  using AllocTraits = std::allocator_traits<typename std::allocator_traits<
      Allocator>::template rebind_alloc<node_type>>;

 private:
  BaseNode base_node_;

  size_type size_;
  Compare comp;
  std::allocator_traits<Allocator>::template rebind_alloc<Node> alloc;

  node_type* begin_(inorder_tag) const { return base_node_.right; }

  node_type* begin_(preorder_tag) const { return base_node_.left; }

  node_type* begin_(postorder_tag) const { return base_node_.parent; }

 public:
  BinarySearchTree(Compare comp = Compare(), Allocator alloc = Allocator())
      : base_node_(), size_(0), comp(comp), alloc(alloc) {
    base_node_.parent = static_cast<Node*>(&base_node_);
    base_node_.right = static_cast<Node*>(&base_node_);
  }

  template <typename It>
  BinarySearchTree(It it1, It it2, Compare comp = Compare(),
                   Allocator alloc = Allocator())
      : base_node_(), size_(0), comp(comp), alloc(alloc) {
    base_node_.parent = static_cast<Node*>(&base_node_);
    base_node_.right = static_cast<Node*>(&base_node_);
    insert(it1, it2);
  }

  BinarySearchTree(const std::initializer_list<value_type>& il,
                   Compare comp = Compare(), Allocator alloc = Allocator())
      : base_node_(), size_(0), comp(comp), alloc(alloc) {
    base_node_.parent = static_cast<Node*>(&base_node_);
    base_node_.right = static_cast<Node*>(&base_node_);
    insert(il);
  }

  BinarySearchTree(const BinarySearchTree& other)
      : base_node_(), size_(0), comp(other.comp), alloc(other.alloc) {
    base_node_.parent = static_cast<Node*>(&base_node_);
    base_node_.right = static_cast<Node*>(&base_node_);
    insert(other.begin<preorder_tag>(), other.end<preorder_tag>());
  }

  BinarySearchTree& operator=(const BinarySearchTree& other) {
    if (*this == other) {
      return *this;
    }
    clear();
    comp = other.comp;
    alloc = other.alloc;
    for (auto it = other.begin<preorder_tag>(); it != other.end<preorder_tag>();
         ++it) {
      insert(*it);
    }
    return *this;
  }

  BinarySearchTree& operator=(std::initializer_list<value_type> il) {
    clear();
    comp = Compare();
    alloc = Allocator();
    insert(il);
    return *this;
  }

  ~BinarySearchTree() { clear(); }

  template <typename traversal_type = inorder_tag>
  iterator<traversal_type> begin() {
    return const_iterator<traversal_type>{begin_(traversal_type{})};
  }

  template <typename traversal_type = inorder_tag>
  iterator<traversal_type> end() {
    return const_iterator<traversal_type>{static_cast<Node*>(&base_node_)};
  }

  template <typename traversal_type = inorder_tag>
  const_iterator<traversal_type> begin() const {
    return const_iterator<traversal_type>{begin_(traversal_type{})};
  }

  template <typename traversal_type = inorder_tag>
  const_iterator<traversal_type> end() const {
    return const_iterator<traversal_type>{
        static_cast<const Node*>(&base_node_)};
  }

  template <typename traversal_type = inorder_tag>
  const_iterator<traversal_type> cbegin() const {
    return const_iterator<traversal_type>{begin_(traversal_type{})};
  }

  template <typename traversal_type = inorder_tag>
  const_iterator<traversal_type> cend() const {
    return const_iterator<traversal_type>{
        static_cast<const Node*>(&base_node_)};
  }

  template <typename traversal_type = inorder_tag>
  reverse_iterator<traversal_type> rbegin() {
    return std::reverse_iterator(static_cast<Node*>(&base_node_));
  }

  template <typename traversal_type = inorder_tag>
  reverse_iterator<traversal_type> rend() {
    return std::reverse_iterator(begin_(traversal_type{}));
  }

  template <typename traversal_type = inorder_tag>
  const_reverse_iterator<traversal_type> rbegin() const {
    return std::reverse_iterator(static_cast<const Node*>(&base_node_));
  }

  template <typename traversal_type = inorder_tag>
  const_reverse_iterator<traversal_type> rend() const {
    return std::reverse_iterator(begin_(traversal_type{}));
  }

  template <typename traversal_type = inorder_tag>
  const_reverse_iterator<traversal_type> crbegin() const {
    return std::reverse_iterator(static_cast<const Node*>(&base_node_));
  }

  template <typename traversal_type = inorder_tag>
  const_reverse_iterator<traversal_type> crend() const {
    return std::reverse_iterator(begin_(traversal_type{}));
  }

  void swap(BinarySearchTree& other) {
    if (*this == other) {
      return;
    }
    if (base_node_.left != nullptr) {
      base_node_.left->parent = static_cast<Node*>(&(other.base_node_));
    }
    if (other.base_node_.left != nullptr) {
      other.base_node_.left->parent = static_cast<Node*>(&base_node_);
    }
    std::swap(base_node_, other.base_node_);
    std::swap(size_, other.size_);
    std::swap(comp, other.comp);
    std::swap(alloc, other.alloc);
    if (base_node_.left == nullptr) {
      base_node_.right = static_cast<Node*>(&base_node_);
      base_node_.parent = static_cast<Node*>(&base_node_);
    }
    if (other.base_node_.left == nullptr) {
      other.base_node_.right = static_cast<Node*>(&other.base_node_);
      other.base_node_.parent = static_cast<Node*>(&other.base_node_);
    }
  }

  size_type size() const { return size_; }

  size_type max_size() const { return std::numeric_limits<size_type>::max(); }

  bool empty() const { return begin() == end(); }

  key_compare key_comp() const { return comp; }

  value_compare value_comp() const { return comp; }

  template <typename traversal_type = inorder_tag>
  std::pair<iterator<traversal_type>, bool> insert(const_reference value) {
    iterator<traversal_type> it = find<traversal_type>(value);
    if (it != end<traversal_type>()) {
      return std::make_pair(it, false);
    }
    ++size_;
    node_type* leaf = alloc.allocate(1);
    AllocTraits::construct(alloc, leaf, value);
    if (base_node_.left == nullptr) {
      base_node_.left = leaf;
      base_node_.right = leaf;
      base_node_.parent = leaf;
      leaf->parent = static_cast<Node*>(&base_node_);
      return std::make_pair(iterator<traversal_type>{leaf}, true);
    }
    node_type* temp = base_node_.left;
    while (temp != nullptr) {
      if (comp(leaf->key, temp->key)) {
        if (base_node_.right == temp) {
          base_node_.right = leaf;
          leaf->parent = temp;
          temp->left = leaf;
          break;
        } else if (temp->left != nullptr) {
          temp = temp->left;
        } else {
          leaf->parent = temp;
          temp->left = leaf;
          break;
        }
      } else {
        if (temp->right != nullptr) {
          temp = temp->right;
        } else {
          leaf->parent = temp;
          temp->right = leaf;
          break;
        }
      }
    }
    node_type* node = base_node_.left;
    while (!(node->left == nullptr && node->right == nullptr)) {
      if (node->left != nullptr) {
        node = node->left;
      } else {
        node = node->right;
      }
    }
    base_node_.parent = node;
    return std::make_pair(iterator<traversal_type>{leaf}, true);
  }

  template <typename It>
  void insert(It it1, It it2) {
    for (auto it = it1; it != it2; ++it) {
      insert(*it);
    }
  }

  void insert(const std::initializer_list<value_type>& il) {
    insert(il.begin(), il.end());
  }

  template <typename traversal_type = inorder_tag>
  iterator<traversal_type> erase(iterator<traversal_type> it) {
    --size_;
    node_type* root = base_node_.left;
    node_type* temp = const_cast<Node*>(static_cast<const Node*>(it.ptr));
    node_type* par = temp->parent;
    iterator<> it_copy = iterator<>{it.ptr};
    ++it_copy;
    ++it;
    if (temp->left == nullptr && temp->right == nullptr) {
      if (base_node_.left == temp) {
        base_node_.left = nullptr;
        base_node_.right = static_cast<Node*>(&base_node_);
        base_node_.parent = static_cast<Node*>(&base_node_);
      } else {
        if (par->left == temp) {
          par->left = nullptr;
          if (base_node_.right == temp) {
            base_node_.right = par;
          }
        } else {
          par->right = nullptr;
        }
      }
    } else if (temp->left == nullptr || temp->right == nullptr) {
      if (temp->left == nullptr) {
        if (base_node_.left == temp) {
          root->right->parent = static_cast<Node*>(&base_node_);
          root = root->right;
          base_node_.left = root;
          base_node_.right = root;
        } else {
          if (par->left == temp) {
            par->left = temp->right;
          } else {
            par->right = temp->right;
          }
          temp->right->parent = par;
        }
      } else {
        if (base_node_.left == temp) {
          root->left->parent = static_cast<Node*>(&base_node_);
          root = root->left;
          base_node_.left = root;
        } else {
          if (par->left == temp) {
            par->left = temp->left;
          } else {
            par->right = temp->left;
          }
          temp->left->parent = par;
        }
      }
    } else {
      Node* next = const_cast<Node*>(static_cast<const Node*>(it_copy.ptr));
      if (it.ptr == it_copy.ptr) {
        it = iterator<traversal_type>{temp};
      }
      std::swap(temp, next);
      next->key = temp->key;
      if (temp->parent->left == temp) {
        temp->parent->left = temp->right;
        if (temp->right != nullptr) {
          temp->right->parent = temp->parent;
        }
      } else {
        temp->parent->right = temp->right;
        if (temp->right != nullptr) {
          temp->right->parent = temp->parent;
        }
      }
    }
    if (size_ != 0) {
      node_type* node = base_node_.left;
      while (!(node->left == nullptr && node->right == nullptr)) {
        if (node->left != nullptr) {
          node = node->left;
        } else {
          node = node->right;
        }
      }
      base_node_.parent = node;
    }
    AllocTraits::destroy(alloc, temp);
    alloc.deallocate(temp, 1);
    if (size_ == 0) {
      return end<traversal_type>();
    }
    return it;
  }

  template <typename traversal_type = inorder_tag>
  iterator<traversal_type> erase(iterator<traversal_type> it1,
                                 iterator<traversal_type> it2) {
    while (it1 != it2) {
      it1 = erase<traversal_type>(it1);
    }
    return it2;
  }

  size_type erase(const_reference value) {
    iterator<> it = find(value);
    if (it == end()) {
      return 0;
    }
    erase(it);
    return 1;
  }

  void clear() { erase(begin(), end()); }

  template <typename traversal_type = inorder_tag>
  iterator<traversal_type> find(const_reference value) const {
    if (base_node_.left == nullptr) {
      return end<traversal_type>();
    }
    node_type* temp = base_node_.left;
    while (temp != nullptr && value != temp->key) {
      if (comp(value, temp->key)) {
        temp = temp->left;
      } else {
        temp = temp->right;
      }
    }
    if (temp == nullptr) {
      return end<traversal_type>();
    }
    return {temp};
  }

  size_type count(const_reference value) const {
    if (contains(value)) {
      return 1;
    }
    return 0;
  }

  bool contains(const_reference value) const {
    if (find(value) == end()) {
      return false;
    }
    return true;
  }

  template <typename traversal_type = inorder_tag>
  iterator<traversal_type> lower_bound(const_reference value) const {
    if (base_node_.left == nullptr) {
      return end();
    }
    node_type* temp = base_node_.left;
    node_type* best = nullptr;
    while (temp != nullptr) {
      if (!comp(temp->key, value) &&
          (best == nullptr || comp(temp->key, best->key))) {
        best = temp;
      }
      if (comp(value, temp->key)) {
        temp = temp->left;
      } else {
        temp = temp->right;
      }
    }
    if (best == nullptr) {
      return end();
    }
    return {best};
  }

  template <typename traversal_type = inorder_tag>
  iterator<traversal_type> upper_bound(const_reference value) const {
    if (base_node_.left == nullptr) {
      return end();
    }
    node_type* temp = base_node_.left;
    node_type* best = nullptr;
    while (temp != nullptr) {
      if (comp(value, temp->key) &&
          (best == nullptr || comp(temp->key, best->key))) {
        best = temp;
      }
      if (comp(value, temp->key)) {
        temp = temp->left;
      } else {
        temp = temp->right;
      }
    }
    if (best == nullptr) {
      return end();
    }
    return {best};
  }

  template <typename traversal_type = inorder_tag>
  std::pair<iterator<traversal_type>, iterator<traversal_type>> equal_range(
      const_reference value) const {
    return std::make_pair(lower_bound(value), upper_bound(value));
  }
};

template <typename T, typename Compare = std::less<T>,
          typename Allocator = std::allocator<T>>
bool operator==(const BinarySearchTree<T, Compare, Allocator>& first,
                const BinarySearchTree<T, Compare, Allocator>& second) {
  if (first.size() != second.size()) {
    return false;
  }
  auto it1 = first.template begin<preorder_tag>();
  auto it2 = second.template begin<preorder_tag>();
  for (size_t i = 0; i < first.size(); ++i) {
    if (*it1 != *it2) {
      return false;
    }
    ++it1;
    ++it2;
  }
  return true;
}

template <typename T, typename Compare = std::less<T>,
          typename Allocator = std::allocator<T>>
bool operator!=(const BinarySearchTree<T, Compare, Allocator>& first,
                const BinarySearchTree<T, Compare, Allocator>& second) {
  return !(first == second);
}

template <typename T, typename Compare = std::less<T>,
          typename Allocator = std::allocator<T>>
void swap(BinarySearchTree<T, Compare, Allocator>& first,
          BinarySearchTree<T, Compare, Allocator>& second) {
  first.swap(second);
}