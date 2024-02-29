#ifndef __UNIQUE_PTR_H__
#define __UNIQUE_PTR_H__

namespace selfUtils {
	template<typename T>
	class UniquePtr {
	public:
		UniquePtr() :_ptr(nullptr) {}
		UniquePtr(T* ptr) :_ptr(ptr) {}
		UniquePtr(const UniquePtr& rhs) = delete;
		auto operator=(const UniquePtr& rhs)-> UniquePtr& = delete;
		UniquePtr(UniquePtr&& rhs) noexcept :_ptr(rhs._ptr) {
			rhs._ptr = nullptr;
		}
		auto operator=(UniquePtr&& rhs) noexcept ->UniquePtr& {
			if (_ptr == rhs._ptr) {
				return *this;
			}
			_ptr = rhs._ptr;
			rhs._ptr = nullptr;
			return *this;
		}
		auto operator->() -> const T* {
			return _ptr;
		}
		auto operator*() -> const T& {
			if (_ptr == nullptr) return nullptr;
			return *_ptr;
		}
		operator bool() const {
			return _ptr != nullptr;
		}
		auto get() -> const T* {
			return _ptr;
		}
		auto release() -> T* {
			T* temp = _ptr;
			_ptr = nullptr;
			return temp;
		}
		~UniquePtr() {
			delPtr();
		}
		auto reset() -> void {
			delPtr();
			_ptr = nullptr;
		}
		auto reset(T* ptr) -> void {
			delPtr();
			_ptr = ptr;
		}
		auto swap(UniquePtr& rhs) -> void {
			T* temp = _ptr;
			_ptr = rhs._ptr;
			rhs._ptr = temp;
		}
	private:
		auto delPtr() -> void {
			if (_ptr != nullptr) {
				delete _ptr;
				_ptr = nullptr;
			}
		}
		T* _ptr;
	};
}

#endif