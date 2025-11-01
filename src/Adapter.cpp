/**
 * Adapter パターン
 * 
 * 修正できない既存機能を新たなクラス内で別の名前（関数）で利用
 * したい場合。
 * 
 * - Target : 新たな関数インタフェース、関数名。
 * - Adapter : Target の具象化クラス。
 * - Adaptee : 既存の修正できないクラス。
 * 
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror -I../inc Adapter.cpp -o ../bin/Adapter
 */
#include <iostream>
#include <cassert>

template <class M, class D>
void (*ptr_print_debug)(M, D) = [](const auto message, const auto debug) -> void
{
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept ErrReasonable = requires(Error& e) {
    e.what();
};
template <class Error>
requires ErrReasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    std::cerr << "ERROR: " << e.what() << std::endl;
};

// 既存の修正できないクラス
// Adaptee
class Product
{
private:
    int cost = 0;
public:
    explicit Product(int _cost) : cost{_cost}
    {}
    int getCost() const
    {
        return cost;
    }
};

// Target
class ProductPrice
{
public:
    virtual int getPrice() const = 0;
};

// Adapter ... 継承による解決。
class ProductAdapter final : public Product, public ProductPrice
{
public:
    ProductAdapter(int _cost) : Product(_cost)
    {}
    virtual int getPrice() const
    {
        return getCost();
    }
};

// Adapter ... 委譲による解決。
class ProductAdapterV2 final : public ProductPrice
{
private:
    Product& product;
public:
    ProductAdapterV2(Product& _product) : product{_product}
    {}
    virtual int getPrice() const
    {
        return product.getCost();
    }
};


int test_ProductAdapter()
{
    puts("------ test_ProductAdapter");
    try {
        ProductAdapter adapter(1000);
        printf("price: %d\n", adapter.getPrice());
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_ProductAdapterV2()
{
    puts("------ test_ProductAdapterV2");
    try {
        Product product(2000);
        ProductAdapterV2 adapter(product);
        printf("price: %d\n", adapter.getPrice());
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}


int main(void)
{
    puts("START main ===");
    int ret = -1;
    if(1) {
        ptr_print_debug<const char*, int&>("Play and Result ... test_ProductAdapter", ret = test_ProductAdapter());
        assert(ret == 0);
    }
    if(1) {
        ret = -1;        
        ptr_print_debug<const char*, int&>("Play and Result ... test_ProductAdapterV2", ret = test_ProductAdapterV2());
        assert(ret == 0);
    }
    puts("=== main END");
    return EXIT_SUCCESS;
}