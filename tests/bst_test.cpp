#include <gtest/gtest.h>

#include <lib/BST.cpp>
#include <string>
#include <vector>

TEST(BstTestSuite, InorderTraversalTest) {
  BinarySearchTree<int> bst(
      std::initializer_list<int>{4, 2, 6, 10, 1, 7, 13, 5, 3});
  std::vector<int> inorder{1, 2, 3, 4, 5, 6, 7, 10, 13};
  ASSERT_EQ(inorder.size(), bst.size());
  auto it1 = inorder.begin();
  for (auto it2 = bst.begin(); it2 != bst.end(); ++it2) {
    ASSERT_EQ(*it1, *it2);
    ++it1;
  }
  for (auto it2 = bst.end(); it2 != bst.begin();) {
    --it1;
    --it2;
    ASSERT_EQ(*it1, *it2);
  }
}

TEST(BstTestSuite, PreorderTraversalTest) {
  BinarySearchTree<int> bst(
      std::initializer_list<int>{4, 2, 6, 10, 1, 7, 13, 5, 3});
  std::vector<int> preorder{4, 2, 1, 3, 6, 5, 10, 7, 13};
  ASSERT_EQ(preorder.size(), bst.size());
  auto it1 = preorder.begin();
  for (auto it2 = bst.begin<preorder_tag>(); it2 != bst.end<preorder_tag>();
       ++it2) {
    ASSERT_EQ(*it1, *it2);
    ++it1;
  }
  for (auto it2 = bst.end<preorder_tag>(); it2 != bst.begin<preorder_tag>();) {
    --it1;
    --it2;
    ASSERT_EQ(*it1, *it2);
  }
}

TEST(BstTestSuite, PostorderTraversalTest) {
  BinarySearchTree<int> bst(std::initializer_list<int>{6, 2, 5, 3, 10, 7, 6});
  std::vector<int> postorder{3, 5, 2, 7, 10, 6};
  ASSERT_EQ(postorder.size(), bst.size());
  auto it1 = postorder.begin();
  for (auto it2 = bst.begin<postorder_tag>(); it2 != bst.end<postorder_tag>();
       ++it2) {
    ASSERT_EQ(*it1, *it2);
    ++it1;
  }
  for (auto it2 = bst.end<postorder_tag>();
       it2 != bst.begin<postorder_tag>();) {
    --it1;
    --it2;
    ASSERT_EQ(*it1, *it2);
  }
}

TEST(BstTestSuite, SizeTest) {
  BinarySearchTree<std::string> bst(std::initializer_list<std::string>{
      "a", "b", "cde", "ffff", "cat", "star", "aram"});
  ASSERT_EQ(bst.size(), 7);
  bst.erase("cd");
  bst.erase("llalala");
  ASSERT_EQ(bst.size(), 7);
  bst.erase("aram");
  bst.erase("cde");
  bst.erase("cat");
  ASSERT_EQ(bst.size(), 4);
  bst.clear();
  ASSERT_EQ(bst.size(), 0);
}

TEST(BstTestSuite, EmptyTest) {
  BinarySearchTree<int> bst;
  ASSERT_TRUE(bst.empty());
  bst.insert(7);
  bst.insert(13);
  bst.insert(2);
  bst.insert(6);
  ASSERT_FALSE(bst.empty());
  bst.erase(13);
  bst.erase(7);
  bst.erase(6);
  bst.erase(2);
  ASSERT_TRUE(bst.empty());
  bst.insert(100);
  bst.insert(12);
  bst.clear();
  ASSERT_TRUE(bst.empty());
}

TEST(BstTestSuite, TrivialInsertTest) {
  BinarySearchTree<std::string> bst;
  bst.insert("ffff");
  bst.insert("cat");
  bst.insert("star");
  bst.insert("aram");
  ASSERT_TRUE(bst.contains("ffff"));
  ASSERT_EQ(bst.count("ffff"), 1);
  ASSERT_FALSE(bst.contains("asa"));
  ASSERT_EQ(bst.count("asa"), 0);
  auto p = bst.insert("aram");
  ASSERT_EQ(p.first, bst.find("aram"));
  ASSERT_FALSE(p.second);
  auto t = bst.insert<preorder_tag>("simple");
  ASSERT_EQ(*t.first, "simple");
  ASSERT_TRUE(t.second);
}

TEST(BstTestSuite, InsertTest) {
  BinarySearchTree<int> bst;
  std::vector<int> v{1, 8, 5, 4, 2, 9};
  bst.insert(v.begin(), v.end());
  ASSERT_EQ(bst.size(), v.size());
  ASSERT_TRUE(bst.contains(8));
  ASSERT_TRUE(bst.contains(4));
  ASSERT_TRUE(bst.contains(5));
  std::initializer_list<int> il{1, 6, 7, 8, 10};
  bst.insert(il);
  ASSERT_TRUE(bst.contains(7));
  ASSERT_TRUE(bst.contains(10));
}

TEST(BstTestSuite, EraseTest) {
  BinarySearchTree<int> bst(std::initializer_list<int>{4, 2, 6, 10, 5, 3});
  auto n = bst.erase(3);
  ASSERT_FALSE(bst.contains(3));
  ASSERT_EQ(n, 1);
  auto it1 = bst.erase<preorder_tag>(bst.begin<preorder_tag>());
  ASSERT_FALSE(bst.contains(4));
  bst.erase<preorder_tag>(it1);
  ASSERT_FALSE(bst.contains(2));
  bst.insert(4);
  bst.insert(2);
  bst.insert(3);
  bst.insert(8);
  bst.erase(++(++bst.begin()), bst.end());
  ASSERT_EQ(bst.size(), 2);
}

TEST(BstTestSuite, FindTest) {
  BinarySearchTree<std::string> bst(std::initializer_list<std::string>{
      "frfrfrfr", "ct", "star", "ma", "ser"});
  auto it1 = bst.find("star");
  auto it2 = bst.find("weather");
  auto it3 = bst.find<postorder_tag>("caca");
  auto it4 = bst.find<preorder_tag>("ser");
  ASSERT_EQ(*it1, "star");
  ASSERT_EQ(it2, bst.end());
  ASSERT_EQ(it3, bst.end<postorder_tag>());
  ASSERT_EQ(*it4, "ser");
}

TEST(BstTestSuite, EqualRangeTest) {
  BinarySearchTree<int> bst(std::initializer_list<int>{4, 2, 6, 10, 5, 3});
  auto a = bst.equal_range(7);
  auto b = bst.equal_range(3);
  auto c = bst.equal_range(10);
  ASSERT_EQ(*a.first, 10);
  ASSERT_EQ(*a.second, 10);
  ASSERT_EQ(*b.first, 3);
  ASSERT_EQ(*b.second, 4);
  ASSERT_EQ(*c.first, 10);
  ASSERT_EQ(c.second, bst.end());
}

TEST(BstTestSuite, SwapTest) {
  BinarySearchTree<int> a;
  a.insert(7);
  a.insert(13);
  a.insert(2);
  a.insert(6);
  BinarySearchTree<int> b;
  b.insert(18);
  a.swap(b);
  ASSERT_EQ(b.size(), 4);
  ASSERT_TRUE(b.contains(7));
  ASSERT_TRUE(b.contains(13));
  ASSERT_TRUE(b.contains(2));
  ASSERT_TRUE(b.contains(6));
  ASSERT_EQ(a.size(), 1);
  ASSERT_TRUE(a.contains(18));
  b.insert(10);
  ASSERT_EQ(a.size(), 1);
}

TEST(BstTestSuite, EmptySwapTest) {
  BinarySearchTree<int> a;
  BinarySearchTree<int> b(std::initializer_list<int>{4, 2, 6, 10, 5, 3});
  swap(a, b);
  ASSERT_TRUE(b.empty());
  ASSERT_EQ(a.size(), 6);
}

TEST(BstTestSuite, TrivialConstructorTest) {
  BinarySearchTree<int> bst1;
  bst1.insert(7);
  bst1.insert(13);
  bst1.insert(2);
  bst1.insert(6);
  BinarySearchTree<int> bst2(bst1);
  ASSERT_EQ(bst1, bst2);
  bst1.erase(7);
  bst1.insert(7);
  ASSERT_FALSE(bst1 == bst2);
  bst2 = bst2;
  bst1 = bst2;
  ASSERT_EQ(bst1, bst2);
}

TEST(BstTestSuite, ConstructorTest) {
  std::vector<int> v{1, 8, 5, 4, 2, 9};
  BinarySearchTree<int> bst(v.begin(), v.end());
  ASSERT_EQ(bst.size(), v.size());
  ASSERT_TRUE(bst.contains(8));
  ASSERT_TRUE(bst.contains(4));
  ASSERT_TRUE(bst.contains(5));
  std::initializer_list<int> il{1, 6, 7, 8, 10};
  bst = il;
  ASSERT_EQ(bst.size(), il.size());
  ASSERT_TRUE(bst.contains(1));
  ASSERT_TRUE(bst.contains(7));
  ASSERT_TRUE(bst.contains(10));
}