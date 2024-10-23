#pragma once
#include "vector.hpp"

namespace vector::_detail {

    template<typename T>
    VectorIter<T>::VectorIter(pointer ptr): ptr(ptr) { }

    template<typename T>
    typename VectorIter<T>::reference VectorIter<T>::operator*() const {
        return *ptr;
    }

    template<typename T>
    typename VectorIter<T>::pointer VectorIter<T>::operator->() const {
        return ptr;
    }

    template<typename T>
    VectorIter<T> &VectorIter<T>::operator++() {
        ++ptr;
        return *this;
    }

    template<typename T>
    VectorIter<T> &VectorIter<T>::operator--() {
        --ptr;
        return *this;
    }

    template<typename T>
    VectorIter<T> VectorIter<T>::operator++(int) {
        return VectorIter {ptr++};
    }

    template<typename T>
    VectorIter<T> VectorIter<T>::operator--(int) {
        return VectorIter {ptr--};
    }

    template<typename T>
    VectorIter<T> VectorIter<T>::operator+(difference_type n) const {
        return VectorIter {ptr + n};
    }

    template<typename T>
    VectorIter<T> VectorIter<T>::operator-(difference_type n) const {
        return VectorIter {ptr - n};
    }

    template<typename T>
    VectorIter<T> &VectorIter<T>::operator+=(difference_type n) {
        ptr += n;
        return *this;
    }

    template<typename T>
    VectorIter<T> &VectorIter<T>::operator-=(difference_type n) {
        ptr -= n;
        return *this;
    }

    template<typename T>
    typename VectorIter<T>::difference_type VectorIter<T>::operator-(VectorIter other) const {
        return ptr - other.ptr;
    }

    template<typename T>
    typename VectorIter<T>::reference VectorIter<T>::operator[](difference_type pos) const {
        return ptr[pos];
    }

    template<typename T>
    typename VectorIter<T>::pointer VectorIter<T>::base() const {
        return ptr;
    }

    template<typename T>
    VectorIter<T>::operator VectorIter<const T>() { // NOLINT(*-explicit-constructor)
        return VectorIter<const T> {ptr};
    }

    template<typename T, typename Allocator>
    Vector<T, Allocator>::Vector() : storage(allocator_type()), sz(0) { }

    template<typename T, typename Allocator>
    Vector<T, Allocator>::Vector(const allocator_type &alloc) : storage(alloc), sz(0) { }

    template<typename T, typename Allocator>
    Vector<T, Allocator>::Vector(const size_type count, const value_type &value, const allocator_type &alloc)
        : Vector(alloc) {
        _create_storage(count);
        _uninitialized_fill_n_alloc(_data(), count, value, alloc);
        sz = count;
    }

    template<typename T, typename Allocator>
    Vector<T, Allocator>::Vector(size_type count, const allocator_type &alloc)
        : Vector(alloc) {
        _create_storage(count);
        _uninitialized_default_n_alloc(_data(), count, alloc);
        sz = count;
    }

    template<typename T, typename Allocator>
    template<IsInputIterator InputIter>
    Vector<T, Allocator>::Vector(InputIter first, InputIter last, const allocator_type &alloc)
        : Vector(alloc) {
        const size_type count = std::distance(first, last);
        _create_storage(count);
        _uninitialized_copy_alloc(first, last, _data(), alloc);
        sz = count;
    }

    template<typename T, typename Allocator>
    Vector<T, Allocator>::Vector(const Vector &other)
        : Vector(
            other._data(), other._data() + other.sz,
            std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.storage.alloc)
        ) { }

    template<typename T, typename Allocator>
    Vector<T, Allocator>::Vector(const Vector &other, const allocator_type &alloc)
        : Vector(other._data(), other._data() + other.sz, alloc) { }

    template<typename T, typename Allocator>
    Vector<T, Allocator>::Vector(Vector &&other) noexcept
        : Vector() {
        _swap(other);
    }

    template<typename T, typename Allocator>
    Vector<T, Allocator>::Vector(Vector &&other, const allocator_type &alloc)
        : storage(alloc), sz(0) {
        if (alloc == other.storage.alloc) {
            _swap(other);
            return;
        }
        _create_storage(other.sz);
        _uninitialized_move_alloc(other._data(), other._data() + size(), _data(), alloc);
        sz = other.sz;
    }

    template<typename T, typename Allocator>
    Vector<T, Allocator>::Vector(std::initializer_list<value_type> init, const allocator_type &alloc)
        : Vector(init.begin(), init.end(), alloc) { }

    template<typename T, typename Allocator>
    Vector<T, Allocator> &Vector<T, Allocator>::operator=(const Vector &other) {
        _clear();
        _reserve_exact(other.sz);
        if constexpr (std::allocator_traits<allocator_type>::propagate_on_container_copy_assignment::value) {
            storage = Storage(other.storage.alloc, other.sz);
        }
        _uninitialized_copy_alloc(other._data(), other._data() + other.sz);
        sz = other.sz;
        return *this;
    }

    template<typename T, typename Allocator>
    Vector<T, Allocator> &Vector<T, Allocator>::operator=(Vector &&other) noexcept {
        _clear();
        _swap(other);
        return *this;
    }

    template<typename T, typename Allocator>
    Vector<T, Allocator> &Vector<T, Allocator>::operator=(std::initializer_list<value_type> init) {
        assign(init);
        return *this;
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::assign(size_type count, const value_type &value) {
        _clear();
        _reserve_exact(count);
        _uninitialized_fill_n_alloc(_data(), count, value, storage.alloc);
        sz = count;
    }

    template<typename T, typename Allocator>
    template<IsInputIterator InputIter>
    void Vector<T, Allocator>::assign(InputIter first, InputIter last) {
        size_type count = std::distance(first, last);
        _clear();
        _reserve_exact(count);
        _uninitialized_copy_alloc(first, last, _data(), storage.alloc);
        sz = count;
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::assign(std::initializer_list<value_type> init) {
        assign(init.begin(), init.end());
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::allocator_type Vector<T, Allocator>::get_allocator() const {
        return storage.alloc;
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::reference Vector<T, Allocator>::at(size_type pos) {
        if (pos >= sz) {
            throw std::out_of_range("pos");
        }
        return _data()[pos];
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::reference Vector<T, Allocator>::operator[](size_type pos) {
        return _data()[pos];
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::reference Vector<T, Allocator>::front() {
        return _data()[0];
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::reference Vector<T, Allocator>::back() {
        return _data()[sz - 1];
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::pointer Vector<T, Allocator>::data() {
        return _data();
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reference Vector<T, Allocator>::at(size_type pos) const {
        if (pos >= sz) {
            throw std::out_of_range("pos");
        }
        return _data()[pos];
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reference Vector<T, Allocator>::operator[](size_type pos) const {
        return _data()[pos];
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reference Vector<T, Allocator>::front() const {
        return _data()[0];
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reference Vector<T, Allocator>::back() const {
        return _data()[sz - 1];
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::const_pointer Vector<T, Allocator>::data() const {
        return _data();
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::iterator Vector<T, Allocator>::begin() {
        return iterator {_data()};
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::begin() const {
        return const_iterator {_data()};
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::cbegin() const {
        return const_iterator {_data()};
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::iterator Vector<T, Allocator>::end() {
        return iterator {_data() + sz};
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::end() const {
        return const_iterator {_data() + sz};
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::cend() const {
        return const_iterator {_data() + sz};
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::reverse_iterator Vector<T, Allocator>::rbegin() {
        return std::reverse_iterator {iterator {_data() + sz}};
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reverse_iterator Vector<T, Allocator>::rbegin() const {
        return std::reverse_iterator {const_iterator {_data() + sz}};
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reverse_iterator Vector<T, Allocator>::crbegin() const {
        return std::reverse_iterator {const_iterator {_data() + sz}};
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::reverse_iterator Vector<T, Allocator>::rend() {
        return std::reverse_iterator {iterator {_data()}};
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reverse_iterator Vector<T, Allocator>::rend() const {
        return std::reverse_iterator {const_iterator {_data()}};
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reverse_iterator Vector<T, Allocator>::crend() const {
        return std::reverse_iterator {const_iterator {_data()}};
    }

    template<typename T, typename Allocator>
    bool Vector<T, Allocator>::empty() const {
        return sz == 0;
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::size_type Vector<T, Allocator>::size() const {
        return sz;
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::size_type Vector<T, Allocator>::max_size() const {
        return std::numeric_limits<size_type>::max();
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::reserve(size_type cap) {
        _reserve_exact(cap);
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::size_type Vector<T, Allocator>::capacity() const {
        return storage.cap;
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::shrink_to_fit() {
        Storage new_storage {storage.alloc, sz};
        _uninitialized_move_alloc(_data(), _data() + sz, new_storage.data, storage.alloc);
        storage = std::move(new_storage);
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::clear() {
        _clear();
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::iterator Vector<T, Allocator>::insert(
        const_iterator pos, const value_type &value
    ) {
        return emplace(pos, value);
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::iterator Vector<T, Allocator>::insert(const_iterator pos, value_type &&value) {
        return emplace(pos, std::move(value));
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::iterator Vector<T, Allocator>::insert(
        const_iterator pos, size_t count, const value_type &value
    ) {
        size_t s_pos = pos - begin();
        _expand_storage(s_pos, count);
        sz += count;
        try {
            _uninitialized_fill_n_alloc(_data() + s_pos, count, value, storage.alloc);
            return iterator {_data() + s_pos};
        } catch (...) {
            _collapse_storage(s_pos, count);
            sz -= count;
            throw;
        }
    }

    template<typename T, typename Allocator>
    template<IsInputIterator InputIter>
    typename Vector<T, Allocator>::iterator Vector<T, Allocator>::insert(
        const_iterator pos, InputIter first, InputIter last
    ) {
        size_t count = std::distance(first, last);
        size_t s_pos = pos - begin();
        _expand_storage(s_pos, count);
        sz += count;
        try {
            _uninitialized_copy_alloc(first, last, _data() + s_pos, storage.alloc);
            return iterator {_data() + s_pos};
        } catch (...) {
            _collapse_storage(s_pos, count);
            sz -= count;
            throw;
        }
    }

    template<typename T, typename Allocator>
    template<typename ...Args>
    typename Vector<T, Allocator>::iterator Vector<T, Allocator>::emplace(const_iterator pos, Args && ...args) {
        size_t s_pos = pos - begin();
        if (pos == end()) {
            emplace_back(std::forward<Args>(args) ...);
            return iterator {_data() + s_pos};
        }
        value_type tmp(std::forward<Args>(args) ...);
        _expand_storage(s_pos, 1);
        sz++;
        try {
            std::allocator_traits<allocator_type>::construct(storage.alloc, _data() + s_pos, std::move(tmp));
            return iterator {_data() + s_pos};
        } catch (...) {
            _collapse_storage(s_pos, 1);
            sz--;
            throw;
        }
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::iterator Vector<T, Allocator>::erase(const_iterator pos) {
        resize(pos - begin());
        return end();
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::iterator Vector<T, Allocator>::erase(
        const_iterator first, const_iterator last
    ) {
        size_t s_pos = first - begin();
        size_t count = std::distance(first, last);
        if (count == 0) {
            return iterator {_data() + s_pos};
        }
        if (last == end()) {
            return erase(first);
        }
        _destroy_alloc(_data() + s_pos, _data() + (s_pos + count), storage.alloc);
        _collapse_storage(s_pos, count);
        sz -= count;
        return iterator {_data() + s_pos};
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::iterator Vector<T, Allocator>::insert(
        const_iterator pos, std::initializer_list<value_type> init
    ) {
        return insert(pos, init.begin(), init.end());
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::push_back(const value_type &value) {
        _reserve_amortized(sz + 1);
        std::allocator_traits<allocator_type>::construct(storage.alloc, _data() + sz++, value);
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::push_back(value_type &&value) {
        _reserve_amortized(sz + 1);
        std::allocator_traits<allocator_type>::construct(storage.alloc, _data() + sz++, std::move(value));
    }

    template<typename T, typename Allocator>
    template<typename ...Args>
    typename Vector<T, Allocator>::reference Vector<T, Allocator>::emplace_back(Args && ...args) {
        _reserve_amortized(sz + 1);
        std::allocator_traits<allocator_type>::construct(storage.alloc, _data() + sz, std::forward<Args>(args) ...);
        return *(_data() + sz++);
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::pop_back() {
        std::allocator_traits<allocator_type>::destroy(storage.alloc, _data() + (sz - 1));
        sz--;
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::resize(size_type count) {
        if (count < sz) {
            _destroy_alloc(_data() + count, _data() + sz, storage.alloc);
        } else if (count > sz) {
            _reserve_exact(count);
            _uninitialized_default_n_alloc(_data() + sz, count - sz, storage.alloc);
        }
        sz = count;
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::resize(size_type count, const value_type &value) {
        if (count < sz) {
            _destroy_alloc(_data() + count, _data() + sz, storage.alloc);
        } else if (count > sz) {
            _reserve_exact(count);
            _uninitialized_fill_n_alloc(_data() + sz, count - sz, value, storage.alloc);
        }
        sz = count;
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::swap(Vector &other) noexcept {
        _swap(other);
    }

    template<typename T, typename Allocator>
    Vector<T, Allocator>::~Vector() {
        _destroy_alloc(_data(), _data() + sz, storage.alloc);
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::pointer Vector<T, Allocator>::_data() {
        return storage.data;
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::const_pointer Vector<T, Allocator>::_data() const {
        return storage.data;
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::_create_storage(size_type cap) {
        storage = Storage(storage.alloc, cap);
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::_clear() {
        _destroy_alloc(_data(), _data() + sz, storage.alloc);
        sz = 0;
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::_reserve_exact(size_type cap) {
        if (storage.cap >= cap) {
            return;
        }
        Storage new_storage {storage.alloc, cap};
        _uninitialized_move_alloc(_data(), _data() + sz, new_storage.data, storage.alloc);
        storage = std::move(new_storage);
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::_reserve_amortized(size_type cap) {
        if (storage.cap >= cap) {
            return;
        }
        _reserve_exact(static_cast<size_type>(1) << (CHAR_BIT * sizeof(size_type) - __builtin_clzll(cap)));
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::_expand_storage(size_type pos, size_type cnt) {
        if (cnt == 0) {
            return;
        }
        _reserve_amortized(sz + cnt);
        if (pos == sz) {
            return;
        }
        if (sz - pos <= cnt) {
            _uninitialized_move_alloc(_data() + pos, _data() + sz, _data() + (pos + cnt), storage.alloc);
            _destroy_alloc(_data() + pos, _data() + sz, storage.alloc);
        } else {
            _uninitialized_move_alloc(_data() + (sz - cnt), _data() + sz, _data() + sz, storage.alloc);
            std::move_backward(_data() + pos, _data() + (sz - cnt), _data() + sz);
            _destroy_alloc(_data() + pos, _data() + pos + cnt, storage.alloc);
        }
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::_collapse_storage(size_type pos, size_type cnt) {
        if (cnt == 0) {
            return;
        }
        if (sz - (pos + cnt) <= cnt) {
            _uninitialized_move_alloc(_data() + pos + cnt, _data() + sz, _data() + pos, storage.alloc);
            _destroy_alloc(_data() + pos + cnt, _data() + sz, storage.alloc);
        } else {
            _uninitialized_move_alloc(_data() + pos + cnt, _data() + pos + 2 * cnt, _data() + pos, storage.alloc);
            std::move(_data() + pos + 2 * cnt, _data() + sz, _data() + pos + cnt);
            _destroy_alloc(_data() + (sz - cnt), _data() + sz, storage.alloc);
        }
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::_swap(Vector &other) noexcept {
        storage.swap(other.storage);
        std::swap(sz, other.sz);
    }

    template<typename T, typename Allocator>
    Vector<T, Allocator>::Storage::Storage(const allocator_type &alloc): alloc(alloc), data(nullptr), cap(0) { }

    template<typename T, typename Allocator>
    Vector<T, Allocator>::Storage::Storage(const allocator_type &alloc, size_type cap)
        : alloc(alloc), data(std::allocator_traits<Allocator>::allocate(this->alloc, cap)), cap(cap) { }

    template<typename T, typename Allocator>
    Vector<T, Allocator>::Storage::Storage(Storage &&other) noexcept
        : Storage(other.alloc) {
        swap(other);
    }

    template<typename T, typename Allocator>
    typename Vector<T, Allocator>::Storage &Vector<T, Allocator>::Storage::operator=(Storage &&other) noexcept {
        if (&other == this) {
            return *this;
        }
        _release();
        swap(other);
        return *this;
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::Storage::swap(Storage &other) noexcept {
        std::swap(data, other.data);
        std::swap(alloc, other.alloc);
        std::swap(cap, other.cap);
    }

    template<typename T, typename Allocator>
    Vector<T, Allocator>::Storage::~Storage() {
        _release();
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::Storage::_release() {
        std::allocator_traits<allocator_type>::deallocate(alloc, data, cap);
        data = nullptr;
        cap = 0;
    }

    template<typename T, typename Allocator>
    auto operator<=>(const Vector<T, Allocator> &lhs, const Vector<T, Allocator> &rhs) {
        return std::lexicographical_compare_three_way(
            lhs.begin(), lhs.end(),
            rhs.begin(), rhs.end()
        );
    }

    template<typename T, typename Allocator>
    bool operator==(const Vector<T, Allocator> &lhs, const Vector<T, Allocator> &rhs) {
        return std::equal(
            lhs.begin(), lhs.end(),
            rhs.begin(), rhs.end()
        );
    }

    template<typename ForwardIter, typename T, typename Allocator>
    void _uninitialized_fill_n_alloc(ForwardIter first, std::size_t count, const T &value, Allocator &alloc) {
        auto current = first;
        try {
            for (; count > 0; --count, ++current) {
                std::allocator_traits<Allocator>::construct(alloc, std::addressof(*current), value);
            }
        } catch (...) {
            _destroy_alloc(first, current, alloc);
            throw;
        }
    }

    template<typename ForwardIter, typename Allocator>
    void _uninitialized_default_n_alloc(ForwardIter first, std::size_t count, Allocator &alloc) {
        auto current = first;
        try {
            for (; count > 0; --count, ++current) {
                std::allocator_traits<Allocator>::construct(alloc, std::addressof(*current));
            }
        } catch (...) {
            _destroy_alloc(first, current, alloc);
            throw;
        }
    }

    template<typename InputIter, typename ForwardIter, typename Allocator>
    void _uninitialized_copy_alloc(InputIter first, InputIter last, ForwardIter dst_first, Allocator &alloc) {
        auto dst_current = dst_first;
        try {
            for (auto current = first; current != last; ++current, ++dst_current) {
                std::allocator_traits<Allocator>::construct(alloc, std::addressof(*dst_current), *current);
            }
        } catch (...) {
            _destroy_alloc(dst_first, dst_current, alloc);
            throw;
        }
    }

    template<typename InputIter, typename ForwardIter, typename Allocator>
    void _uninitialized_move_alloc(InputIter first, InputIter last, ForwardIter dst_first, Allocator &alloc) {
        auto dst_current = dst_first;
        try {
            for (auto current = first; current != last; ++current, ++dst_current) {
                std::allocator_traits<Allocator>::construct(
                    alloc, std::addressof(*dst_current), std::move(*current)
                );
            }
        } catch (...) {
            _destroy_alloc(dst_first, dst_current, alloc);
            throw;
        }
    }

    template<typename ForwardIter, typename Allocator>
    void _destroy_alloc(ForwardIter first, ForwardIter last, Allocator &alloc) {
        for (; first != last; ++first) {
            std::allocator_traits<Allocator>::destroy(alloc, std::addressof(*first));
        }
    }

}
