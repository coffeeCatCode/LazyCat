

#ifndef FUNCTION_H
#define FUNCTION_H


#include "AnyCat.h"

#include <functional>


namespace cat
{


class FunctionCat
{
private:
    template <typename _T>
    struct ExtractReturnValue {
        explicit ExtractReturnValue(Any *value) : result(value) {}

        Any *result;
    };

    template <typename _Ret, typename _Result>
    void ExtractResult(_Ret &&value, const ExtractReturnValue<_Result> &result)
    {
        *result.result = value;
    }

    template <typename _Ret>
    void ExtractResult(_Ret , const ExtractReturnValue<void>) {}

public:
    template <typename _Func, typename... _Args>
    FunctionCat(_Func func, _Args&&... args) {
        auto f = std::bind(func, std::forward<_Args>(args)...);
        func_ = [f, this]() {
            Any ret;
            using Return_type = decltype(func(std::forward<_Args>(args)...));
            ExtractReturnValue<Return_type> result(&ret);
            ExtractResult(f(), result);
            return ret;
        };
    }

    Any Call() { return func_(); }

private:
    std::function<Any()> func_;
};


}



#endif // FUNCTION_H
