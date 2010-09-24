#ifndef __REF_H__
#define __REF_H__

#include <stdint.h>
#include "debug.h"
#include "HeapManager.h"

template< typename T >
class Ref
{
public:	
	typedef void (*Deleter)(T*);
	
public:
 Ref()
	: ptr_(0)
	, deleter_(0)
	, ref_count_(0)
	{
	}

 Ref(T *p, Deleter del = 0)
	 : ptr_(p)
		, deleter_(del)
		, ref_count_(0)
	{
		if ( ptr_ ) {
			ref_count_ = new unsigned int(1);
		}
	}
	
	virtual ~Ref()
	{
		release();
	}
	
	
	Ref(const Ref<T> &other )
	: ptr_(other.ptr_)
	, deleter_(other.deleter_)
	, ref_count_(other.ref_count_)
	{
		if ( ptr_ ) {
			add();
		}
	}
	
	Ref<T> & operator=(const Ref<T> &other)
	{
		if ( (this == &other) || (ptr_ == other.ptr_) ) {
			return *this;
		}
		
		if ( ptr_ ) {
			release();
		}
		
		ptr_       = other.ptr_;
		deleter_   = other.deleter_;
		ref_count_ = other.ref_count_;
		add();
		
		return *this;
	}

	template < class U >
	   Ref(const Ref<U> &other) 
		: ptr_(static_cast<T*>(other.ptr_)) 
		, deleter_((Deleter)((void*)(other.deleter_)))
		, ref_count_(other.ref_count_)
	{
		if (ptr_) {
			add(); 
		}
	}
	
	template <class U >
	Ref<T> & operator=(const Ref<U> &other)
	{
		if (ptr_ == other.ptr_) {
			return *this;
		}
		
		if (ptr_) {
			release();
		}
		
		ptr_       = static_cast<T*>(other.ptr_);
		deleter_   = (Deleter)((void*)(other.deleter_));
		ref_count_ = other.ref_count_;
		add();
		
		return *this;
	}
	/*
	operator T*() const
	{
		return ptr_; 
	}
	*/
	T * operator->() const
	{
		return ptr_;
	}
	
	bool operator==(const Ref<T> &other) const
	{
		return ptr_ == other.ptr_;
	}
	
	bool operator!=(const Ref<T>& other) const 
	{
		return ptr_ != other.ptr_;
	}

	operator bool () const
	{
		return ptr_ != 0;
	}

	const T *get() const
	{
		return ptr_;
	}

	T *get()
	{
		return ptr_;
	}
protected:
	void add()
	{
		if (ref_count_) {
			++(*ref_count_);
		}
	}
	
	void release()
	{
		if (ref_count_) {
			--(*ref_count_);
			if ( 0 == (*ref_count_)) {
				HeapManager::instance()->unregist(ptr_);
				if (deleter_) {
					(*deleter_)(ptr_);
				} else {
					DBG("delete %p", ptr_);
					delete ptr_;
				}
				ptr_ = 0;
				delete ref_count_;
				ref_count_ = 0;
			}
		}
	}
		
public:
	T *ptr_;
	Deleter deleter_;
	unsigned int *ref_count_;
};

#endif //__REF_H__

