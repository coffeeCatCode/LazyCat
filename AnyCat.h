

#ifndef ANYCAT_H
#define ANYCAT_H


#include <memory>
#include <typeindex>
#include <type_traits>
#include <typeinfo>


namespace cat
{


class Any
{
private:
    struct DataBase;
    using dataPtr = std::unique_ptr<DataBase>;

    struct DataBase
    {
        virtual ~DataBase() {}
        virtual dataPtr Clone() const = 0;
        virtual const std::type_info &Type() const = 0;
    };

    template<typename _T>
    struct Data : public DataBase
    {
        template<typename...Args>
        Data(Args&&...args) : data(std::forward<Args>(args)...){ }

        virtual dataPtr Clone() const override
        {
            return dataPtr(new Data(data));
        }

        virtual const std::type_info &Type() const override
        {
            return typeid(_T);
        }

        _T data;
    };

    dataPtr Clone() const
    {
        if (data_) {
            return data_->Clone();
        }
        return nullptr;
    }

public:
    Any() {}
    Any(const Any &other) : data_(other.Clone()) {}
    Any(Any &&other) : data_(std::move(other.data_)){}

    template <typename _T, typename = typename std::enable_if<!std::is_same<typename std::decay<_T>::type, Any>::value, _T>::type>
    Any(_T &&t) : data_(new Data<typename std::decay<_T>::type>(std::forward<_T>(t))) {}

    Any &Swap(Any &other) { data_.swap(other.data_); return *this; }

    bool IsEmpty() const { return !data_; }
    bool IsNUll() const { return IsEmpty(); }
    bool HaveValue() const { return !IsEmpty(); }

    template <typename _T>
    bool IsType() const { return Type() == typeid(_T); }

    const std::type_info &Type() const { return data_ ?  data_->Type() :
typeid(void); }

    void Clear() { dataPtr().swap(data_); }

    ///< 如使用之前未使用IsType检查是否是同一类型, 那么请注意std::bad_cast异常
    template <typename _T>
    _T& Cast() const {
        if (IsType<_T>() == false) {
            throw std::bad_cast();
        }

        auto p = dynamic_cast<Data<_T>*>(this->data_.get());
        return p->data;
    }

    Any &operator=(const Any &other) {
        if (this->data_ == other.data_) {
            return *this;
        }

        this->data_ = other.Clone();
        return *this;
    }


private:
    dataPtr data_;
};


}


#endif // ANYCAT_H
