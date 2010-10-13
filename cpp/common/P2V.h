#ifndef P2V_H
#define P2V_H

template<class T>
class P2V //pointer to value
{
 public:
 P2V(T *p) : ptr(p) {}

template<class U>
	P2V(const P2V<U> &o) {
	ptr = o.ptr;
 }
	T *ptr;

 private:
	P2V();
};

template <class T>
P2V<T> _(T *o) {
	return P2V<T>(o);
}
#endif //P2V_H
