#include <stdio.h>

// string in the compile time

template <char...> struct StaticString {};

// map raw string to StaticString
// see more details in https://zhuanlan.zhihu.com/p/377145104

#define GETCHAR(n, str) (str[n < sizeof(str) ? n : sizeof(str) - 1])

#define MAKEINDEX16(n, str) \
    GETCHAR(0x##n##0, str), GETCHAR(0x##n##1, str), \
    GETCHAR(0x##n##2, str), GETCHAR(0x##n##3, str), \
    GETCHAR(0x##n##4, str), GETCHAR(0x##n##5, str), \
    GETCHAR(0x##n##6, str), GETCHAR(0x##n##7, str), \
    GETCHAR(0x##n##8, str), GETCHAR(0x##n##9, str), \
    GETCHAR(0x##n##a, str), GETCHAR(0x##n##b, str), \
    GETCHAR(0x##n##c, str), GETCHAR(0x##n##d, str), \
    GETCHAR(0x##n##e, str), GETCHAR(0x##n##f, str)

#define MAKEINDEX(str) \
    MAKEINDEX16(0, str), MAKEINDEX16(1, str), \
    MAKEINDEX16(2, str), MAKEINDEX16(3, str), \
    MAKEINDEX16(4, str), MAKEINDEX16(5, str), \
    MAKEINDEX16(6, str), MAKEINDEX16(7, str)

#define MAKESTRING(str) decltype(breakString(StaticString<MAKEINDEX(str)>{}))

#define R2(expr) eval_v<parse_v<tokenize_v<MAKESTRING(expr)>>>
#define DISPLAY(expr) display<expr>()

template <char ...c1, char c, char ...c2>
auto makeString(StaticString<c1...>, StaticString<c>, StaticString<c2>...)
    -> decltype(makeString(StaticString<c1..., c>{}, StaticString<c2>{}...));
template <char ...c1, char ...c2>
auto makeString(StaticString<c1...>, StaticString<'\0'>, StaticString<c2>...)
    -> StaticString<c1...>;

template <char ...cs>
auto breakString(StaticString<cs...>)
    -> decltype(makeString(StaticString<cs>{}...));

// List

template <class...> struct List;

// (if cond conseq alt)

template <bool, class conseq, class alt> struct _if
{
    using value = conseq;
};
template <class conseq, class alt>
struct _if<false, conseq, alt>
{
    using value = alt;
};
template <bool cond, class conseq, class alt>
using if_v = typename _if<cond, conseq, alt>::value;

// judge if the two types is equal to each other

template <class t, class f>
struct equal
{
    static constexpr bool value = false;
};
template <class t>
struct equal<t, t>
{
    static constexpr bool value = true;
};
template <class t, class f>
inline constexpr auto equal_v = equal<t, f>::value;

// concat StaticStrings or Lists

template <class, class> struct concat;
template <char ...l, char ...r>
struct concat<StaticString<l...>, StaticString<r...>>
{
    using value = StaticString<l..., r...>;
};
template <class ...l, class ...r>
struct concat<List<l...>, List<r...>>
{
    using value = List<l..., r...>;
};
template <class l, class r>
using concat_v = typename concat<l, r>::value;

// tokensize

template <class, class, class> struct tokenize;
template <class r, class t>
struct tokenize<r, t, StaticString<>>
{
    using value = if_v<equal_v<t, StaticString<>>, r, concat_v<r, List<t>>>;
};
template <class r, class t, char ...cs>
struct tokenize<r, t, StaticString<'(', cs...>>
{
    using value = typename tokenize<
        concat_v<r, List<StaticString<'('>>>, 
        StaticString<>, 
        StaticString<cs...>>::value;
};
template <class r, class t, char ...cs>
struct tokenize<r, t, StaticString<')', cs...>>
{
    using value = typename tokenize<
        concat_v<r, if_v<equal_v<t, StaticString<>>, List<StaticString<')'>>, List<t, StaticString<')'>>>>, 
        StaticString<>, 
        StaticString<cs...>>::value;
};
template <class r, class t, char ...cs>
struct tokenize<r, t, StaticString<' ', cs...>>
{
    using value = typename tokenize<
        if_v<equal_v<t, StaticString<>>, r, concat_v<r, List<t>>>,
        StaticString<>, 
        StaticString<cs...>>::value;
};
template <class r, class t, char c, char ...cs>
struct tokenize<r, t, StaticString<c, cs...>>
{
    using value = typename tokenize<
        r, 
        concat_v<t, StaticString<c>>, 
        StaticString<cs...>>::value;
};
template <class str>
using tokenize_v = typename tokenize<List<>, StaticString<>, str>::value;

// some types
template <int N> struct Number;
template <char ...cs> using Symbol = StaticString<cs...>; 

// parse

// parse number (only interger)

template <class, class> struct parseNumber;
template <int N> struct parseNumber<Number<N>, StaticString<>>
{
    using value = Number<N>;
};
template <int N, char c, char ...cs>
struct parseNumber<Number<N>, StaticString<c, cs...>>
{
    using value = typename parseNumber<Number<N * 10 + c - '0'>, StaticString<cs...>>::value;
};
template <class n>
using parseNumber_v = typename parseNumber<Number<0>, n>::value;

// (car list)
template <class> struct car;
template <class h, class ...t>
struct car<List<h, t...>>
{
    using value = h;
};
template <class l>
using car_v = typename car<l>::value;

// (push l v) => l.push(v)
template <class, class> struct push;
template <class ...vs, class v>
struct push<List<vs...>, v>
{
    using value = List<vs..., v>;
};
template <class l, class v>
using push_v = typename push<l, v>::value;

template <class, class> struct parenthesize;
template <class r> struct parenthesize<List<>, r>
{
    using value = car_v<r>;
    using rest = List<>;
};
template <class ...ts, class r>
struct parenthesize<List<StaticString<'('>, ts...>, r>
{
private:
    using son = parenthesize<List<ts...>, List<>>;
    using next = parenthesize<typename son::rest, push_v<r, typename son::value>>;
public:
    using value = typename parenthesize<typename son::rest, push_v<r, typename son::value>>::value;
    using rest = typename next::rest;
};
template <class ...ts, class r>
struct parenthesize<List<StaticString<')'>, ts...>, r>
{
    using value = r;
    using rest = List<ts...>;
};
template <char c, char ...cs, class ...ts, class r>
struct parenthesize<List<StaticString<c, cs...>, ts...>, r>
{
private:
    using atom = if_v<'0'<=c&&c<='9', parseNumber_v<StaticString<c, cs...>>, Symbol<c, cs...>>;
    using next = parenthesize<List<ts...>, push_v<r, atom>>;
public:
    using value = typename next::value;
    using rest = typename next::rest;
};

template <class tokens>
using parse_v = typename parenthesize<tokens, List<>>::value;

// some types and keywords

struct Null;
template <class, class> struct Environment;
template <class, class> struct Closure;
using lambda = Symbol<'l', 'a', 'm', 'b', 'd', 'a'>;
using let = Symbol<'l', 'e', 't'>;

// look up keys in the environment

template <class, class> struct lookup;
template <class t>
struct lookup<t, Environment<List<>, List<>>>
{
    using value = Null;
};
template <class t, class k, class ...ks, class v, class ...vs>
struct lookup<t, Environment<List<k, ks...>, List<v, vs...>>>
{
    using value = if_v<equal_v<t, k>, v, typename lookup<t, Environment<List<ks...>, List<vs...>>>::value>;
};
template <class t, class e>
using lookup_v = typename lookup<t, e>::value;

// add key-value to the environment

template <class, class, class> struct extenv;
template <class k, class v, class ...ks, class ...vs>
struct extenv<k, v, Environment<List<ks...>, List<vs...>>>
{
    using value = Environment<List<k, ks...>, List<v, vs...>>;
};
template <class k, class v, class e>
using extenv_v = typename extenv<k, v, e>::value;

// builtin calculation

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

// evaluate

template <class, class> struct eval;
template <int N, class e> struct eval<Number<N>, e>
{
    using value = Number<N>;
};
template <char ...cs, class e>
struct eval<Symbol<cs...>, e>
{
    using value = lookup_v<Symbol<cs...>, e>;
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
using eval_v = typename eval<expr, Environment<List<>, List<>>>::value;

// for debug

template <class THE_RESULT>
void display() { printf("%s\n", __PRETTY_FUNCTION__ + 15); }

int main()
{
// (let ([x 2])
//   (let ([f (lambda (y) (* x y))])
//     (let ([x 4])
//       (f 3))))
    using res = R2("(let (x 2) (let (f (lambda (y) (* x y))) (let (x 4) (f 3))))");
    DISPLAY(res);
    return 0;
}