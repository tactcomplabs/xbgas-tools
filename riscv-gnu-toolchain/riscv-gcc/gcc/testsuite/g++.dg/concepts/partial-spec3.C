// { dg-options "-std=c++1z -fconcepts" }

template <class T> struct A { };
template <class T> requires false struct A<T*> { };
template <class T> struct A<T*> { static int i; };

int i = A<int*>::i;
