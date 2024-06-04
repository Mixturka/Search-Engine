#include <gtest/gtest.h>
#include "/Users/deniv/Documents/labwork11-Mixturka/src/Searcher/RequestParser/Parser.hpp"
#include "/Users/deniv/Documents/labwork11-Mixturka/src/Searcher/RequestParser/Scanner.cpp"
#include "/Users/deniv/Documents/labwork11-Mixturka/src/Indexer/DataBase/BTree/BTree.hpp"
#include <string>
#include <sstream>
#include <fstream>


TEST(SearchEngineTest, InitializationTest) {
  uint64_t k = 3;
  Interpreter iter(k);
  std::string query = "for";
  Scanner scanner(query);
  Parser parser(scanner.ScanTokens());
  iter.Init("/Users/deniv/Documents/labwork11-Mixturka/Base/");
  Expression* expr = parser.ParseExpression();
  EXPECT_EQ(iter.Evaluate(expr), "for");
  delete expr;
}

TEST(SearchEngineTest, ParseSimpleAND) {

  uint64_t k = 3;
  Interpreter iter(k);
  std::string query = "for AND vector";
  Scanner scanner(query);
  Parser parser(scanner.ScanTokens());
  iter.Init("/Users/deniv/Documents/labwork11-Mixturka/Base/");
  Expression* expr = parser.ParseExpression();
  EXPECT_EQ(iter.Evaluate(expr), "forANDvector");
  delete expr;
}

TEST(SearchEngineTest, ParseSimpleOR) {

  uint64_t k = 3;
  Interpreter iter(k);
  std::string query = "for OR vector";
  Scanner scanner(query);
  Parser parser(scanner.ScanTokens());
  iter.Init("/Users/deniv/Documents/labwork11-Mixturka/Base/");
  Expression* expr = parser.ParseExpression();
  EXPECT_EQ(iter.Evaluate(expr), "forORvector");
  delete expr;
}

TEST(SearchEngineTest, ParserParents) {

  uint64_t k = 3;
  Interpreter iter(k);
  std::string query = "(for OR vector)";
  Scanner scanner(query);
  Parser parser(scanner.ScanTokens());
  iter.Init("/Users/deniv/Documents/labwork11-Mixturka/Base/");
  Expression* expr = parser.ParseExpression();
  EXPECT_EQ(iter.Evaluate(expr), "forORvector");
}

TEST(BTreeTest, SimpleInsertStr) {
  std::string name = "btree.dat";
  BTree bTree(3);
  bTree.Create(name);

  bTree.Insert("aboba", "amogus");

  ASSERT_EQ(bTree.Search("aboba", BTree::STRING{}), "amogus");
  std::ofstream file("btree.dat");
  file.close();
}

TEST(BTreeTest, SimpleInsertData) {
  std::string name = "btree.dat";
  BTree bTree(3);
  bTree.Create(name);

  bTree.Insert("aboba", 0, 23, 1, 25);

  std::vector<Data> v = bTree.Search("aboba", BTree::DATA{});
  EXPECT_EQ(v[0].DID, 0);
  EXPECT_EQ(v[0].lines[0], 23);
  EXPECT_EQ(v[0].freq, 1);
  EXPECT_EQ(v[0].size, 25);
  std::ofstream file("btree.dat");
  file.close();
}

TEST(BTreeTest, SimpleInsertDataWithLines) {
  std::string name = "btree.dat";
  BTree bTree(3);
  bTree.Create(name);

  for (int i = 0; i < 1000; ++i){
    bTree.Insert("aboba", 1, i,1, 25);
  }

  std::vector<Data> v = bTree.Search("aboba", BTree::DATA{});
  int i = 0;
  for (Data& data : v) {
    EXPECT_EQ(data.DID, 1);
    for (int j = 0; j < data.lines.size(); ++j) {
      EXPECT_EQ(data.lines[j], i);
      ++i;
    }
  }
  std::ofstream file("btree.dat");
  file.close();
}

TEST(BTreeTest, SimpleInsertSeveralData) {
  std::string name = "btree.dat";
  BTree bTree(3);
  bTree.Create(name);

  for (int i = 0; i < 1000; ++i){
    bTree.Insert("aboba", 1, i,1, 25);
    bTree.Insert("boba", 2, i, 2, 25);
  }

  std::vector<Data> v = bTree.Search("aboba", BTree::DATA{});
  int i = 0;
  for (Data& data : v) {
    EXPECT_EQ(data.DID, 1);
    for (int j = 0; j < data.lines.size(); ++j) {
      EXPECT_EQ(data.lines[j], i);
      ++i;
    }
  }

  std::vector<Data> w = bTree.Search("biba", BTree::DATA{});
  i = 0;
  for (Data& data : w) {
    EXPECT_EQ(data.DID, 2);
    for (int j = 0; j < data.lines.size(); ++j) {
      EXPECT_EQ(data.lines[j], i);
      ++i;
    }
  }
  std::ofstream file("btree.dat");
  file.close();
}

TEST(BTreeTest, StressTest) {
  std::string name = "btree.dat";
  BTree bTree(3);
  bTree.Create(name);

  for (int i = 0; i < 1000; ++i){
    bTree.Insert("aboba" + std::to_string(i), i, 1,1, 25);
  }

  for (int i = 0; i < 1000; ++i) {
    std::vector<Data> v = bTree.Search("aboba" + std::to_string(i), BTree::DATA{});

    for (Data& data : v) {
      EXPECT_EQ(data.DID, i);
      EXPECT_EQ(data.lines[0], 1);
    }
  }
  std::ofstream file("btree.dat");
  file.close();
}

TEST(BTreeTest, SuperStressTest) {
  std::string name = "btree.dat";
  BTree bTree(3);
  bTree.Create(name);

  for (int i = 0; i < 30000; ++i){
    bTree.Insert("aboba" + std::to_string(i), i, 1,1, 25);
  }

  for (int i = 0; i < 30000; ++i) {
    std::vector<Data> v = bTree.Search("aboba" + std::to_string(i), BTree::DATA{});

    for (Data& data : v) {
      EXPECT_EQ(data.DID, i);
      EXPECT_EQ(data.lines[0], 1);
    }
  }
  std::ofstream file("btree.dat");
  file.close();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
