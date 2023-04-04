# Playground

Personal code snippets just for fun.

## [Quick Sort](quicksort/main.cc)

A simple quick sort algorithm implemented with C++ template metaprogramming skills.

## [r2](r2/r2.cpp)

The simplest lisp interpreter inspired by [怎样写一个解释器](http://www.yinwang.org/blog-cn/2012/08/01/interpreter) but implemented with C++ template metaprogramming.

```cpp
// (let ([x 2])
//    (let ([f (lambda (y) (* x y))])
//      (let ([x 4])
//        (f 3))))
    using res = R2(
        _(let, _(S('x'), N(2)),
            _(let, _(S('f'), _(lambda, _(S('y')), _(S('*'), S('x'), S('y')))),
                _(let, _(S('x'), N(4)),
                    _(S('f'), N(3)))))
    );
// => Numb<6>
```

## [r2_v2](r2/r2_v2.cpp)

The simplest lisp interpreter inspired by [怎样写一个解释器](http://www.yinwang.org/blog-cn/2012/08/01/interpreter) but implemented with C++ template metaprogramming. Allow you to write codes like this:

```cpp
// (let ([x 2])
//   (let ([f (lambda (y) (* x y))])
//     (let ([x 4])
//       (f 3))))
    using res = R2("(let (x 2) (let (f (lambda (y) (* x y))) (let (x 4) (f 3))))");
// => Number<6>
```

## [my::\*](my/)

The literally friendly and modern enough implementation of few STL ultilities.
**No, they're just jokes. Don't take them seriously.**

* any
* shared_ptr
* string
