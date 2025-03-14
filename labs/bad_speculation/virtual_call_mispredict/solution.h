#include <cstdint>
#include <vector>
#include <memory>

// Assume this constant never changes
constexpr std::size_t N = 64 * 1024;

struct BaseClass {
    virtual ~BaseClass() = default;

    virtual void handle(std::size_t& data) const = 0;
};

struct ClassA : public BaseClass {
#ifdef SOLUTION
    void handle(std::size_t& data) const override final {
#else
    void handle(std::size_t& data) const override {
#endif
        data += 1;
    }
};

struct ClassB : public BaseClass {
#ifdef SOLUTION
    void handle(std::size_t& data) const override final {
#else
    void handle(std::size_t& data) const override {
#endif
        data += 2;
    }
};

struct ClassC : public BaseClass {
#ifdef SOLUTION
    void handle(std::size_t& data) const override final {
#else
    void handle(std::size_t& data) const override {
#endif
        data += 3;
    }
};

using InstanceArray = std::vector<std::unique_ptr<BaseClass>>;

void generateObjects(InstanceArray& array);
void invoke(InstanceArray& array, std::size_t& data);
