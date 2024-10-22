#pragma once

#include <limits>
#include <memory>

namespace vector {
    namespace _detail {
        template<typename InputIterator>
        concept IsInputIterator = std::is_convertible_v<
            typename std::iterator_traits<InputIterator>::iterator_category,
            std::input_iterator_tag
        >;

        template<typename T>
        class VectorIter {
        public:
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = value_type *;
            using reference = value_type &;
            using iterator_category = std::random_access_iterator_tag;
            using iterator_concept = std::contiguous_iterator_tag;

            VectorIter() = default;

            explicit VectorIter(pointer ptr) : ptr(ptr) {
            }

            VectorIter(const VectorIter &) = default;

            VectorIter(VectorIter &&) noexcept = default;

            VectorIter &operator=(const VectorIter &) = default;

            VectorIter &operator=(VectorIter &&) noexcept = default;

            bool operator==(const VectorIter &) const = default;

            auto operator<=>(const VectorIter &) const = default;

            reference operator*() const {
                return *ptr;
            }

            pointer operator->() const {
                return ptr;
            }

            VectorIter &operator++() {
                ++ptr;
                return *this;
            }

            VectorIter &operator--() {
                --ptr;
                return *this;
            }

            VectorIter operator++(int) {
                return {ptr++};
            }

            VectorIter operator--(int) {
                return {ptr--};
            }

            VectorIter operator+(difference_type n) const {
                return {ptr + n};
            }

            VectorIter operator-(difference_type n) const {
                return {ptr - n};
            }

            VectorIter &operator+=(difference_type n) {
                ptr += n;
                return *this;
            }

            VectorIter &operator-=(difference_type n) {
                ptr -= n;
                return *this;
            }

            difference_type operator-(VectorIter other) const {
                return ptr - other.ptr;
            }

            reference operator[](difference_type pos) const {
                return ptr[pos];
            }

            pointer base() const {
                return ptr;
            }

        private:
            pointer ptr;
        };

        template<typename T>
        VectorIter<T> operator+(typename VectorIter<T>::difference_type n, VectorIter<T> iter) {
            return {n + iter.base()};
        }

        template<typename T, typename Allocator = std::allocator<T> >
        class Vector {
        public:
            // Member types

            using value_type = T;
            using allocator_type = Allocator;
            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;
            using reference = value_type &;
            using const_reference = const value_type &;
            using pointer = typename std::allocator_traits<Allocator>::pointer;
            using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
            using iterator = VectorIter<T>;
            using const_iterator = VectorIter<const T>;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;

            // Constructors

            Vector();

            explicit Vector(const allocator_type &alloc);

            explicit Vector(size_type count, const value_type &value, const allocator_type &alloc = allocator_type());

            explicit Vector(size_type count, const allocator_type &alloc = allocator_type());

            template<IsInputIterator InputIter>
            Vector(InputIter first, InputIter last, const allocator_type &alloc = allocator_type());

            Vector(const Vector &other);

            Vector(const Vector &other, const allocator_type &alloc);

            Vector(Vector &&other) noexcept;

            Vector(Vector &&other, const allocator_type &alloc);

            Vector(std::initializer_list<value_type> init, const allocator_type &alloc = allocator_type());

            // Assignment operators

            Vector &operator=(const Vector &other);

            Vector &operator=(Vector &&other) noexcept;

            Vector &operator=(std::initializer_list<value_type> init);

            // Other member functions

            void assign(size_type count, const value_type &value);

            template<IsInputIterator InputIter>
            void assign(InputIter first, InputIter last);

            void assign(std::initializer_list<value_type> init);

            allocator_type get_allocator() const;

            // Element access

            reference at(size_type pos);

            reference operator[](size_type pos);

            reference front();

            reference back();

            pointer data();

            const_reference at(size_type pos) const;

            const_reference operator[](size_type pos) const;

            const_reference front() const;

            const_reference back() const;

            const_pointer data() const;

            // Iterators

            iterator begin();

            const_iterator begin() const;

            const_iterator cbegin() const;

            iterator end();

            const_iterator end() const;

            const_iterator cend() const;

            reverse_iterator rbegin();

            const_reverse_iterator rbegin() const;

            const_reverse_iterator crbegin() const;

            reverse_iterator rend();

            const_reverse_iterator rend() const;

            const_reverse_iterator crend() const;

            // Size & capacity

            [[nodiscard]] bool empty() const;

            [[nodiscard]] size_type size() const;

            [[nodiscard]] size_type max_size() const;

            void reserve(size_type cap);

            [[nodiscard]] size_type capacity() const;

            void shrink_to_fit();

            // Modifiers

            void clear();

            iterator insert(const_iterator pos, const value_type &value);

            iterator insert(const_iterator pos, value_type &&value);

            iterator insert(const_iterator pos, size_t count, const value_type &value);

            template<IsInputIterator InputIter>
            iterator insert(const_iterator pos, InputIter first, InputIter last);

            iterator insert(const_iterator pos, std::initializer_list<value_type> init);

            template<typename... Args>
            iterator emplace(const_iterator pos, Args &&... args);

            iterator erase(const_iterator pos);

            iterator erase(const_iterator first, const_iterator last);

            iterator push_back(const value_type &value);

            iterator push_back(value_type &&value);

            template<typename... Args>
            iterator emplace_back(Args &&... args);

            void pop_back();

            void resize(size_type count);

            void resize(size_type count, const value_type &value);

            void swap(Vector &other) noexcept;

            // Destructor

            ~Vector();

        private:
            pointer _data();

            const_pointer _data() const;

            void _create_storage(size_type cap);

            void _swap(Vector &other) noexcept;

            struct Storage {
                [[no_unique_address]] allocator_type alloc;
                pointer data;
                size_type cap;

                explicit Storage(const allocator_type &alloc);

                Storage(const allocator_type &alloc, size_type cap);

                Storage(const Storage &) = delete;

                Storage(Storage &&) noexcept;

                Storage &operator=(const Storage &) = delete;

                Storage &operator=(Storage &&) noexcept;

                void swap(Storage &other) noexcept;

                ~Storage();

            private:
                void _release();
            };

            Storage storage;
            size_type sz;
        };

        template<typename T, typename Allocator>
        auto operator<=>(const Vector<T, Allocator> &lhs, const Vector<T, Allocator> &rhs);

        template<typename ForwardIter, typename T, typename Allocator>
        void _uninitialized_fill_n_alloc(ForwardIter first, std::size_t count, const T &value, Allocator &alloc);

        template<typename ForwardIter, typename Allocator>
        void _uninitialized_default_n_alloc(ForwardIter first, std::size_t count, Allocator &alloc);

        template<typename InputIter, typename ForwardIter, typename Allocator>
        void _uninitialized_copy_alloc(InputIter first, InputIter last, ForwardIter dst_first, Allocator &alloc);

        template<typename InputIter, typename ForwardIter, typename Allocator>
        void _uninitialized_move_alloc(InputIter first, InputIter last, ForwardIter dst_first, Allocator &alloc);

        template<typename ForwardIter, typename Allocator>
        void _destroy_alloc(ForwardIter first, ForwardIter last, Allocator &alloc);

        template<typename T, typename Allocator>
        Vector<T, Allocator>::Vector() : storage(allocator_type()), sz(0) {
        }

        template<typename T, typename Allocator>
        Vector<T, Allocator>::Vector(const allocator_type &alloc) : storage(alloc), sz(0) {
        }

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
            ) {
        }

        template<typename T, typename Allocator>
        Vector<T, Allocator>::Vector(const Vector &other, const allocator_type &alloc)
            : Vector(other._data(), other._data() + other.sz, alloc) {
        }

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
            : Vector(init.begin(), init.end(), alloc) {
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
            return iterator{_data()};
        }

        template<typename T, typename Allocator>
        typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::begin() const {
            return const_iterator{_data()};
        }

        template<typename T, typename Allocator>
        typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::cbegin() const {
            return const_iterator{_data()};
        }

        template<typename T, typename Allocator>
        typename Vector<T, Allocator>::iterator Vector<T, Allocator>::end() {
            return iterator{_data() + sz};
        }

        template<typename T, typename Allocator>
        typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::end() const {
            return const_iterator{_data() + sz};
        }

        template<typename T, typename Allocator>
        typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::cend() const {
            return const_iterator{_data() + sz};
        }

        template<typename T, typename Allocator>
        typename Vector<T, Allocator>::reverse_iterator Vector<T, Allocator>::rbegin() {
            return std::reverse_iterator{iterator{_data() + sz}};
        }

        template<typename T, typename Allocator>
        typename Vector<T, Allocator>::const_reverse_iterator Vector<T, Allocator>::rbegin() const {
            return std::reverse_iterator{const_iterator{_data() + sz}};
        }

        template<typename T, typename Allocator>
        typename Vector<T, Allocator>::const_reverse_iterator Vector<T, Allocator>::crbegin() const {
            return std::reverse_iterator{const_iterator{_data() + sz}};
        }

        template<typename T, typename Allocator>
        typename Vector<T, Allocator>::reverse_iterator Vector<T, Allocator>::rend() {
            return std::reverse_iterator{iterator{_data()}};
        }

        template<typename T, typename Allocator>
        typename Vector<T, Allocator>::const_reverse_iterator Vector<T, Allocator>::rend() const {
            return std::reverse_iterator{const_iterator{_data()}};
        }

        template<typename T, typename Allocator>
        typename Vector<T, Allocator>::const_reverse_iterator Vector<T, Allocator>::crend() const {
            return std::reverse_iterator{const_iterator{_data()}};
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
        typename Vector<T, Allocator>::size_type Vector<T, Allocator>::capacity() const {
            return storage.cap;
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
        void Vector<T, Allocator>::_swap(Vector &other) noexcept {
            storage.swap(other.storage);
            std::swap(sz, other.sz);
        }

        template<typename T, typename Allocator>
        Vector<T, Allocator>::Storage::Storage(const allocator_type &alloc): alloc(alloc), data(nullptr), cap(0) {
        }

        template<typename T, typename Allocator>
        Vector<T, Allocator>::Storage::Storage(const allocator_type &alloc, size_type cap)
            : alloc(alloc), data(std::allocator_traits<Allocator>::allocate(this->alloc, cap)), cap(cap) {
        }

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

    using _detail::Vector;
}
