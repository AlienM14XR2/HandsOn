/**
  Filter について考えてみる（C++）。
  g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror -I../inc Filter.cpp -o ../bin/Filter
*/
#include <iostream>
#include <memory>

class Filter
{
public:
  virtual void chain() const = 0;
};

class FooFilter final : public Filter
{
private:
  const Filter* const filter;
public:
  explicit FooFilter(const Filter* const _filter): filter{_filter} 
  {}
  virtual void chain() const 
  {
    puts("Foo");
    if(filter) {
      filter->chain();
    }
  }
};

class BarFilter final : public Filter
{
private:
  const Filter* const filter;
public:
  explicit BarFilter(const Filter* const _filter): filter{_filter} 
  {}
  virtual void chain() const 
  {
    puts("Bar");
    if(filter) {
      filter->chain();
    }
  }
};

class BazzFilter final : public Filter
{
private:
  const Filter* const filter;
public:
  explicit BazzFilter(const Filter* const _filter): filter{_filter} 
  {}
  virtual void chain() const 
  {
    // Bazz 独自の処理を行う。
    puts("Bazz");
    if(filter) {
      filter->chain();
    }
  }
};


int main(void)
{
  puts("=== START main");
  if(1) {
    std::unique_ptr<BazzFilter>  bazz = std::make_unique<BazzFilter>(nullptr);
    std::unique_ptr<BarFilter>  bar = std::make_unique<BarFilter>(bazz.get());
    FooFilter foo(bar.get());
    foo.chain();
  }
  puts("END main ===");
  return EXIT_SUCCESS;  
}
