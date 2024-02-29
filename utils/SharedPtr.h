#ifndef __SHARED_PTR_H__
#define __SHARED_PTR_H__

namespace selfUtils {
	template<typename T>
	class SharedPtr {
	public:
		SharedPtr() :_ptr(nullptr), _refCount(nullptr) {}
		SharedPtr(T* ptr) :_ptr(ptr), _refCount(new int(1)) {}
		SharedPtr(const SharedPtr& rhs) :_ptr(rhs._ptr), _refCount(rhs._refCount){
			(*_refCount)++;
		}
		auto operator=(const SharedPtr& rhs)->SharedPtr& {
			if (_ptr == rhs._ptr) {
				return *this;
			}
			releaseCount();
			_ptr = rhs._ptr;
			_refCount = rhs._refCount;
			if (_ptr != nullptr) {
				(*_refCount)++;
			}
			return *this;
		}
		SharedPtr(SharedPtr&& rhs) noexcept :_ptr(rhs._ptr), _refCount(rhs._refCount) {
			rhs._ptr = nullptr;
			rhs._refCount = nullptr;
		}
		auto operator=(SharedPtr&& rhs) noexcept ->SharedPtr& {
			if (_ptr == rhs._ptr) {
				return *this;
			}
			releaseCount();
			_ptr = rhs._ptr;
			_refCount = rhs._refCount;
			rhs._ptr = nullptr;
			rhs._refCount = nullptr;
			return *this;
		}
		auto operator->() -> const T* {
			return _ptr;
		}
		auto operator*() -> const T& {
			if (_ptr == nullptr) return nullptr;
			return *_ptr;
		}
		auto get() -> const T* {
			return _ptr;
		}
		auto use_count() -> const int {
			if (_refCount == nullptr) return 0;
			return *_refCount;
		}
		~SharedPtr() {
			releaseCount();
		}
		auto reset() -> void {
			releaseCount();
			_ptr = nullptr;
			_refCount = nullptr;
		}
		auto reset(T* ptr) -> void {
			releaseCount();
			_ptr = ptr;
			_refCount = new int(1);
		}
		auto swap(SharedPtr& rhs) -> void {
			T* temp = _ptr;
			int* tempCount = _refCount;
			_ptr = rhs._ptr;
			_refCount = rhs._refCount;
			rhs._ptr = temp;
			rhs._refCount = tempCount;
		}
	private:
		auto releaseCount() -> void {
			if (_ptr == nullptr) return;
			if (--(*_refCount) == 0) {
				delete _ptr;
				_ptr = nullptr;
				delete _refCount;
				_refCount = nullptr;
			}
		}
		T* _ptr;
		int* _refCount;
	};
}

#endif