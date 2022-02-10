#define N(n) Numb<n>
#define S(s) Symb<s>
#define _(...) List<__VA_ARGS__>
#define R2(expr) eval_t<expr, Envi<_(), _()>>

struct Null;
struct True;
struct False;
template <int> struct Numb;
template <char> struct Symb;
template <class...> struct List;
template <class, class> struct Envi;
template <class, class> struct Clos;
struct lambda;
struct let;

template <class t, class u> struct equal 
{
    using type = False;
};
template <class t> struct equal<t, t>
{
    using type = True;
};
template <class t, class u>
using equal_t = typename equal<t, u>::type;

template <class, class, class> struct _if;
template <class conseq, class alt> struct _if<True, conseq, alt>
{
    using type = conseq;
};
template <class conseq, class alt> struct _if<False, conseq, alt>
{
    using type = alt;
};
template <class cond, class conseq, class alt>
using if_t = typename _if<cond, conseq, alt>::type;

template <class, class> struct lookup;
template <class t>
struct lookup<t, Envi<List<>, List<>>>
{
    using type = Null;
};
template <class t, class k, class ...ks, class v, class ...vs>
struct lookup<t, Envi<List<k, ks...>, List<v, vs...>>>
{
    using type = if_t<equal_t<t, k>, v, typename lookup<t, Envi<List<ks...>, List<vs...>>>::type>;
};
template <class k, class e>
using lookup_t = typename lookup<k, e>::type;

template <class, class, class> struct extenv;
template <class k, class v, class ...ks, class ...vs>
struct extenv<k, v, Envi<List<ks...>, List<vs...>>>
{
    using type = Envi<List<k, ks...>, List<v, vs...>>;
};
template <class k, class v, class e>
using extenv_t = typename extenv<k, v, e>::type;

template <class, class> struct add;
template <int x, int y> struct add<Numb<x>, Numb<y>>
{
    using type = Numb<x + y>;
};
template <class x, class y>
using add_t = typename add<x, y>::type;

template <class, class> struct sub;
template <int x, int y> struct sub<Numb<x>, Numb<y>>
{
    using type = Numb<x - y>;
};
template <class x, class y>
using sub_t = typename sub<x, y>::type;

template <class, class> struct mul;
template <int x, int y> struct mul<Numb<x>, Numb<y>>
{
    using type = Numb<x * y>;
};
template <class x, class y>
using mul_t = typename mul<x, y>::type;

template <class, class> struct div;
template <int x, int y> struct div<Numb<x>, Numb<y>>
{
    using type = Numb<x / y>;
};
template <class x, class y>
using div_t = typename div<x, y>::type;

template <class, class> struct eval;
template <int n, class e> struct eval<Numb<n>, e>
{
    using type = Numb<n>;
};
template <char s, class e> struct eval<Symb<s>, e>
{
    using type = lookup_t<Symb<s>, e>;
};
template <class k, class v, class b, class e>
struct eval<List<let, List<k, v>, b>, e>
{
private:
    using val = typename eval<v, e>::type;
public:
    using type = typename eval<b, extenv_t<k, val, e>>::type;
};
template <class p, class b, class e>
struct eval<List<lambda, List<p>, b>, e>
{
    using type = Clos<List<lambda, List<p>, b>, e>;
};
template <class f, class p, class e>
struct eval<List<f, p>, e>
{
private:
    using func = typename eval<f, e>::type;
    using para = typename eval<p, e>::type;
public:
    using type = typename eval<List<func, para>, e>::type;
};
template <class p, class b, class l, class a, class e>
struct eval<List<Clos<List<lambda, List<p>, b>, l>, a>, e>
{
    using type = typename eval<b, extenv_t<p, a, l>>::type;
};
template <class x, class y, class e>
struct eval<List<Symb<'+'>, x, y>, e>
{
private:
    using v1 = typename eval<x, e>::type;
    using v2 = typename eval<y, e>::type;
public:
    using type = add_t<v1, v2>;
};
template <class x, class y, class e>
struct eval<List<Symb<'-'>, x, y>, e>
{
private:
    using v1 = typename eval<x, e>::type;
    using v2 = typename eval<y, e>::type;
public:
    using type = sub_t<v1, v2>;
};
template <class x, class y, class e>
struct eval<List<Symb<'*'>, x, y>, e>
{
private:
    using v1 = typename eval<x, e>::type;
    using v2 = typename eval<y, e>::type;
public:
    using type = mul_t<v1, v2>;
};
template <class x, class y, class e>
struct eval<List<Symb<'/'>, x, y>, e>
{
private:
    using v1 = typename eval<x, e>::type;
    using v2 = typename eval<y, e>::type;
public:
    using type = div_t<v1, v2>;
};
template <class expr, class envi>
using eval_t = typename eval<expr, envi>::type;

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
// => Numb(6)
}