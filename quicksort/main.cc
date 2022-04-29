#include <iostream>
#include <algorithm>
#include <iterator>

// []
template <int...> struct list {};

// (if cond conseq alt)
// in C++,
// if (cond) {
//     return conseq;
// } else {
//     return alt;
// }
template <bool cond, class conseq, class alt>
struct _if;
template <class conseq, class alt>
struct _if<true, conseq, alt>
{
    using value = conseq;
};
template <class conseq, class alt>
struct _if<false, conseq, alt>
{
    using value = alt;
};

template <class l, class m, class r> struct concat;
template <int ...l, int ...m, int ...r>
struct concat<list<l...>, list<m...>, list<r...>>
{
    using value = list<l..., m..., r...>;
};

// filter::(a -> Bool) -> [a] -> [a]
// filter _ [] = []
// filter f [x:xs] = 
//     | f x = x : filter f xs
//     | otherwise = filter f xs
template <template <int P> class F, class L> struct filter;
template <template <int P> class F> struct filter<F, list<>>
{
    using value = list<>;
};
template <template <int P> class F, int x, int ...xs>
struct filter<F, list<x, xs...>>
{
    using value = typename concat<list<>, 
    	typename _if<F<x>::value, list<x>, list<>>::value,
    	typename filter<F, list<xs...>>::value>::value;
};

// less or equal (<=)
template <int rhs>
struct le
{
    template <int lhs>
    struct le_partial
    {
        static constexpr bool value = lhs <= rhs;
    };
    
    template <int lhs>
    using value = le_partial<lhs>;
};

// greater than (>)
template <int rhs>
struct gt
{
    template <int lhs>
    struct gt_partial
    {
        static constexpr bool value = lhs > rhs;
    };
    
    template <int lhs>
    using value = gt_partial<lhs>;
};

// quicksort::(Ord a) => [a] -> [a]
// quicksort [] = []
// quicksort [x:xs] = 
//     let smaller = quicksort (filter (<= x) xs)
//     let larger = quicksort (filter (> x) xs)
//     in smaller ++ [x] ++ larger
template <class L> struct quicksort;
template <> struct quicksort<list<>>
{
    using value = list<>;
};
template <int x, int ...xs> struct quicksort<list<x, xs...>>
{
    template <int lhs>
    using lep = typename le<x>::template value<lhs>;
    
    template <int lhs>
    using gtp = typename gt<x>::template value<lhs>;
    
    using value = typename concat<
        typename quicksort<typename filter<lep, list<xs...>>::value>::value,
    	list<x>,
    	typename quicksort<typename filter<gtp, list<xs...>>::value>::value
        >::value;
};

template <int ...vals>
void print(list<vals...>)
{
    static constexpr int values[] = { vals... };
    std::copy(std::begin(values), std::end(values), std::ostream_iterator<int>(std::cout, " "));
    std::endl(std::cout);
}

int main()
{
	using result = typename quicksort<list<2, 1, 0, 7, 3, 1, -2, 4>>::value;
    print(result());
    return 0;
}