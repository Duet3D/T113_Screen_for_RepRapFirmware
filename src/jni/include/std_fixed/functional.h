/*
 * functional.h
 *
 *  Created on: 11 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_INCLUDE_STD_FIXED_FUNCTIONAL_H_
#define JNI_INCLUDE_STD_FIXED_FUNCTIONAL_H_


#include <stdexcept>

template <typename Signature>
class function;

template <typename R, typename... Args>
class function<R(Args...)>
{
  private:
    struct base_callable
    {
        virtual R operator()(Args...) const = 0;
        virtual base_callable* clone() const = 0;
        virtual ~base_callable() {}
    };

    template <typename F>
    struct callable : base_callable
    {
        F func;

        callable(const F& f) : func(f) {}

        R operator()(Args... args) const override { return func(args...); }

        base_callable* clone() const override { return new callable(*this); }
    };

    base_callable* callable_obj;

  public:
    function() : callable_obj(nullptr) {}

    template <typename F>
    function(const F& f) : callable_obj(new callable<F>(f))
    {
    }

    function(const function& other) : callable_obj(nullptr)
    {
        if (other.callable_obj) { callable_obj = other.callable_obj->clone(); }
    }

    function& operator=(const function& other)
    {
        if (this != &other)
        {
            delete callable_obj;
            callable_obj = nullptr;

            if (other.callable_obj) { callable_obj = other.callable_obj->clone(); }
        }
        return *this;
    }

    ~function() { delete callable_obj; }

    R operator()(Args... args) const
    {
        if (callable_obj) { return (*callable_obj)(args...); }
        throw std::runtime_error();
    }
};


#endif /* JNI_INCLUDE_STD_FIXED_FUNCTIONAL_H_ */
