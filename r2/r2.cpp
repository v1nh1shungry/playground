#include <stdio.h>

#define N(n) Number<n>
#define S(c) Symbol<c>
#define _(...) List<__VA_ARGS__>
#define R2(expr) eval_v<expr>
#define DISPLAY(expr) display<expr>()

// some types

struct Null;
template <int> struct Number;
template <char> struct Symbol;
template <class...> struct List;
template <class> struct Environment;
template <class, class> struct Closure;

// some keywords

struct let;
struct lambda;

// (if cond conseq alt)
template <bool, class conseq, class alt> struct _if
{
    using value = conseq;
};
template <class conseq, class alt> struct _if<false, conseq, alt>
{
    using value = alt;
};
template <bool cond, class conseq, class alt>
using if_v = typename _if<cond, conseq, alt>::value;

// judge if the two types are equal to each other
template <class t, class u> struct equal
{
    static constexpr bool value = false;
};
template <class t> struct equal<t, t>
{
    static constexpr bool value = true;
};
template <class t, class u>
inline constexpr bool equal_v = equal<t, u>::value;

// look up keywords in the Environment
template <class, class> struct lookup;
template <class t> struct lookup<t, Environment<List<>>>
{
    using value = Null;
};
template <class t, class k, class v, class ...ks, class ...vs>
struct lookup<t, Environment<List<List<k, v>, List<ks, vs>...>>>
{
    using value = if_v<equal_v<t, k>, v, typename lookup<t, Environment<List<List<ks, vs>...>>>::value>;
};
template <class t, class e>
using lookup_v = typename lookup<t, e>::value;

// add new key-value pair to the Environment
template <class, class, class> struct extenv;
template <class k, class v, class ...ks, class ...vs>
struct extenv<k, v, Environment<List<List<ks, vs>...>>>
{
    using value = Environment<List<List<k, v>, List<ks, vs>...>>;
};
template <class k, class v, class e>
using extenv_v = typename extenv<k, v, e>::value;

// builtin calculations

template <class, class> struct add;
template <int x, int y> struct add<Number<x>, Number<y>>
{
    using value = Number<x + y>;
};
template <class x, class y>
using add_v = typename add<x, y>::value;

template <class, class> struct sub;
template <int x, int y> struct sub<Number<x>, Number<y>>
{
    using value = Number<x - y>;
};
template <class x, class y>
using sub_v = typename sub<x, y>::value;

template <class, class> struct mul;
template <int x, int y> struct mul<Number<x>, Number<y>>
{
    using value = Number<x * y>;
};
template <class x, class y>
using mul_v = typename mul<x, y>::value;

template <class, class> struct div;
template <int x, int y> struct div<Number<x>, Number<y>>
{
    using value = Number<x / y>;
};
template <class x, class y>
using div_v = typename div<x, y>::value;

// eval(expr, env)
template <class, class> struct eval;
template <int n, class e> struct eval<Number<n>, e>
{
    using value = Number<n>;
};
template <char c, class e> struct eval<Symbol<c>, e>
{
    using value = lookup_v<Symbol<c>, e>;
};
template <class k, class v, class b, class e>
struct eval<List<let, List<k, v>, b>, e>
{
private:
    using val = typename eval<v, e>::value;
public:
    using value = typename eval<b, extenv_v<k, val, e>>::value;
};
template <class p, class b, class e>
struct eval<List<lambda, List<p>, b>, e>
{
    using value = Closure<List<lambda, List<p>, b>, e>;
};
template <class f, class a, class e>
struct eval<List<f, a>, e>
{
private:
    using func = typename eval<f, e>::value;
    using args = typename eval<a, e>::value;
public:
    using value = typename eval<List<func, args>, e>::value;
};
template <class p, class b, class l, class a, class e>
struct eval<List<Closure<List<lambda, List<p>, b>, l>, a>, e>
{
    using value = typename eval<b, extenv_v<p, a, l>>::value;
};
template <class x, class y, class e>
struct eval<List<Symbol<'+'>, x, y>, e>
{
private:
    using v1 = typename eval<x, e>::value;
    using v2 = typename eval<y, e>::value;
public:
    using value = add_v<v1, v2>;
};
template <class x, class y, class e>
struct eval<List<Symbol<'-'>, x, y>, e>
{
private:
    using v1 = typename eval<x, e>::value;
    using v2 = typename eval<y, e>::value;
public:
    using value = sub_v<v1, v2>;
};
template <class x, class y, class e>
struct eval<List<Symbol<'*'>, x, y>, e>
{
private:
    using v1 = typename eval<x, e>::value;
    using v2 = typename eval<y, e>::value;
public:
    using value = mul_v<v1, v2>;
};
template <class x, class y, class e>
struct eval<List<Symbol<'/'>, x, y>, e>
{
private:
    using v1 = typename eval<x, e>::value;
    using v2 = typename eval<y, e>::value;
public:
    using value = div_v<v1, v2>;
};
template <class expr>
using eval_v = typename eval<expr, Environment<List<>>>::value;

// only gcc/clang, print a type, for debug
template <class THE_RESULT>
void display() { printf("%s\n", __PRETTY_FUNCTION__ + 15); }

int main()
{
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
    DISPLAY(res);
    return 0;
}