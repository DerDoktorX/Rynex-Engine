#pragma once
#include <rypch.h>
#include <GTest/GTest.h>

#define EXPECT_ASSERT(stmt)     EXPECT_THROW(stmt, ::Rynex::Testing::AssertFailure)
#define EXPECT_NO_ASSERT(stmt)  EXPECT_NO_THROW(stmt)
namespace Rynex::Testing
{
    struct AssertFailure : std::runtime_error
    {
        AssertFailure(const char* f, int l) : std::runtime_error(std::string(f) + ":" + std::to_string(l)) {}
    };

    inline void ThrowHandler(const char* f, int l) { throw AssertFailure(f, l); }

    // For Tests, counting Assert Throws
    class ScopedAssertHandler
    {
    public:
        explicit ScopedAssertHandler(AssertHook::HandlerFn fn) : m_Prev(AssertHook::Set(fn)) {}
        ~ScopedAssertHandler() { AssertHook::Set(m_Prev); }
    private:
        AssertHook::HandlerFn m_Prev;
    };
}
