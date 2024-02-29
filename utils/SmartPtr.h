#ifndef __SMART_PTR_H__
#define __SMART_PTR_H__

namespace selfUtils {
	template<typename T>
	struct ResourceObj {
		ResourceObj() : _ptr(nullptr), _sRefCount(nullptr), _wRefCount(nullptr) {}
		ResourceObj(T* ptr) : _ptr(ptr), _sRefCount(new int(1)), _wRefCount(nullptr) {}
		//ResourceObj(const ResourceObj& rhs) : _ptr(rhs._ptr), _sRefCount(rhs._sRefCount), _wRefCount(rhs._wRefCount) {}
		auto sRefCount() -> const int {
			return _sRefCount == nullptr ? 0 : *_sRefCount;
		}
		auto wRefCount() -> const int {
			return _wRefCount == nullptr ? 0 : *_wRefCount;
		}
		auto retatinSRef() -> void {
			if (_sRefCount == nullptr) return;
			(*_sRefCount)++;
		}
		auto releaseSRef() -> bool {
			if (_ptr == nullptr) return false;
			if (--(*_sRefCount) == 0) {
				delete _ptr;
				_ptr = nullptr;
				delete _sRefCount;
				_sRefCount = nullptr;
				return wRefCount() == 0;
			}
			return false;
		}
		auto retatinWRef(bool needInit = false) -> void {
			if (_wRefCount == nullptr) {
				if(!needInit) return;
				_wRefCount = new int(0);
			}
			(*_wRefCount)++;
		}
		auto releaseWRef() -> bool {
			if (_wRefCount == nullptr) return false;
			if (--(*_wRefCount) == 0) {
				delete _wRefCount;
				_wRefCount = nullptr;
				return sRefCount() == 0;
			}
			return false;
		}
		T* _ptr;
		int* _sRefCount;
		int* _wRefCount;
	};

	template<typename T>
	class WeakPtr;

	template<typename T>
	class SharedPtr {
	public:
		SharedPtr() :_obj(nullptr) {}
		SharedPtr(T* ptr) :_obj(new ResourceObj<T>(ptr)) {}
		SharedPtr(const SharedPtr& rhs) :_obj(rhs._obj) {
			retain();
		}
		auto operator=(const SharedPtr& rhs)->SharedPtr& {
			if (_obj == rhs._obj) {
				return *this;
			}
			release();
			_obj = rhs._obj;
			retain();
			return *this;
		}
		SharedPtr(SharedPtr&& rhs) noexcept :_obj(rhs._obj) {
			rhs._obj = nullptr;
		}
		auto operator=(SharedPtr&& rhs) noexcept ->SharedPtr& {
			if (_obj == rhs._obj) {
				return *this;
			}
			release();
			_obj = rhs._obj;
			rhs._obj = nullptr;
			return *this;
		}

		auto operator->() -> const T* {
			return _obj->_ptr;
		}
		auto operator*() -> const T& {
			return *(_obj->_ptr);
		}
		auto get() -> const T* {
			if (_obj == nullptr) return nullptr;
			return _obj->_ptr;
		}
		auto use_count() -> const int {
			if (_obj == nullptr) return 0;
			return _obj->sRefCount();
		}
		~SharedPtr() {
			release();
		}
		auto reset() -> void {
			release();
			_obj = nullptr;
		}
		auto reset(T* ptr) -> void {
			release();
			_obj = new ResourceObj<T>(ptr);
		}
		auto swap(SharedPtr& rhs) -> void {
			ResourceObj<T>* temp = _obj;
			_obj = rhs._obj;
			rhs._obj = temp;
		}
		operator bool() const {
			return _obj != nullptr && _obj->_ptr != nullptr;
		}
		friend class WeakPtr<T>;
	private:
		auto retain() -> void {
			if (_obj == nullptr) return;
			_obj->retatinSRef();
		}
		auto release() -> void {
			if (_obj == nullptr) return;
			if (_obj->releaseSRef()) {
				delete _obj;
				_obj = nullptr;
			}
		}
		ResourceObj<T>* _obj;
	};

	template<typename T>
	class UniquePtr {
	public:
		UniquePtr() :_ptr(nullptr) {}
		UniquePtr(T* ptr) :_ptr(ptr) {}
		UniquePtr(const UniquePtr& rhs) = delete;
		auto operator=(const UniquePtr& rhs)->UniquePtr & = delete;
		UniquePtr(UniquePtr&& rhs) noexcept :_ptr(rhs._ptr) {
			rhs._ptr = nullptr;
		}
		auto operator=(UniquePtr&& rhs) noexcept ->UniquePtr& {
			if (_ptr == rhs._ptr) {
				return *this;
			}
			delPtr();
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

	template<typename T>
	class WeakPtr {
	public:
		WeakPtr() :_obj(nullptr) {}
		WeakPtr(const SharedPtr<T>& rhs) : _obj(rhs._obj) {
			retain(true);
		}
		WeakPtr(const WeakPtr& rhs) : _obj(rhs._obj) {
			retain();
		}
		auto operator=(const SharedPtr<T>& rhs)->WeakPtr& {
			if (_obj == rhs._obj) return *this;
			release();
			_obj = rhs._obj;
			retain(true);
			return *this;
		}
		auto operator=(const WeakPtr& rhs)->WeakPtr& {
			if (_obj == rhs._obj) return *this;
			release();
			_obj = rhs._obj;
			retain();
			return *this;
		}
		WeakPtr(WeakPtr&& rhs) noexcept :_obj(rhs._obj) {
			rhs._obj = nullptr;
		}
		auto operator=(WeakPtr&& rhs) noexcept ->WeakPtr& {
			if (_obj == rhs._obj) {
				return *this;
			}
			release();
			_obj = rhs._obj;
			rhs._obj = nullptr;
			return *this;
		}
		auto reset() -> void {
			release();
			_obj = nullptr;
		}
		auto swap(WeakPtr& rhs) -> void {
			ResourceObj<T>* temp = _obj;
			_obj = rhs._obj;
			rhs._obj = temp;
		}
		auto use_count() -> const int {
			if (_obj == nullptr) return 0;
			return _obj->sRefCount();
		}
		auto expired() -> const bool {
			return use_count() == 0;
		}
		auto lock() -> SharedPtr<T> {
			SharedPtr<T> sp = SharedPtr<T>();
			if (expired()) return sp;
			sp._obj = _obj;
			if (sp._obj != nullptr) {
				sp._obj->retatinSRef();
			}
			return sp;
		}

		~WeakPtr() {
			if (_obj == nullptr) return;
			if (_obj->releaseWRef()) {
				delete _obj;
				_obj = nullptr;
			}
		}
		
	private:
		auto retain(bool needInit = false) -> void {
			if (_obj == nullptr) return;
			_obj->retatinWRef(needInit);
		}
		auto release() -> void {
			if (_obj == nullptr) return;
			if (_obj->releaseWRef()) {
				delete _obj;
				_obj = nullptr;
			}
		}
		ResourceObj<T>* _obj;
	};
}

#endif