/**
 * Decorator パターンについて
 * 
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror -I../inc Decorator.cpp -o ../bin/Decorator
 */
#include <iostream>
#include <memory>
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

/**
 * 商品の基底クラス
 * 
 */
class Item 
{
public:
    virtual ~Item() = default;
    virtual double cost() const = 0;
    virtual void description() const = 0;
};

/**
 * 商品デコレータ基底クラス
 * 
 */
class ItemDecorator : public Item
{
protected:
    const Item* const item;
public:
    ItemDecorator(const Item* const _item) : item{_item}
    {}
    virtual ~ItemDecorator() = default;
};

/**
 * ブックカバー料金クラス（Decorator パターンの具象化）。
 * 
 */
class BookCoverFee final : public ItemDecorator
{
private:
    const double price = 100;
public:
    BookCoverFee(const Item* const _item) : ItemDecorator(_item)
    {}
    virtual double cost() const
    {
        return item->cost() + price;
    }
    virtual void description() const
    {
        puts("Book cover fee.");
    }
};

/**
 * 本クラス、Item の具象クラス。
 * 
 * 料金はレジ打ち、コンストラクタにて決定される。
 */
class Book final : public Item
{
private:
    const double price;
public:
    Book(const double _price): price{_price}
    {}
    virtual double cost() const
    {
        return price;
    }
    virtual void description() const
    {
        puts("Book fee.");
    }
};

int test_BookCoverFee() {
    puts("------ test_BookCoverFee");
    try {
        // 既存の Book オブジェクトへの変更は行わず。
        // Decorator にて、修正点、変更点を吸収できる。
        Book book(3000);
        book.description();
        std::unique_ptr<BookCoverFee> cover = std::make_unique<BookCoverFee>(&book);
        cover->description();
        double price = cover->cost();
        printf("price: %lf\n", price);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        // std::cerr << "Error: " << e.what() << std::endl;
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main() {
    puts("START main ===");
    if(1) {
        int result = 1;
        ptr_print_debug<const char*, int&>("Play and Result ... test_BookCoverFee", result = test_BookCoverFee());
        assert( result == 0);
    }
    puts("=== main END");
    return EXIT_SUCCESS;
}