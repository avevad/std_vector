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

            explicit VectorIter(pointer ptr);
            VectorIter(const VectorIter &) = default;
            VectorIter(VectorIter &&) noexcept = default;

            VectorIter &operator=(const VectorIter &) = default;
            VectorIter &operator=(VectorIter &&) noexcept = default;

            bool operator==(const VectorIter &) const = default;
            auto operator<=>(const VectorIter &) const = default;

            reference operator*() const;
            pointer operator->() const;

            VectorIter &operator++();
            VectorIter &operator--();
            VectorIter operator++(int);
            VectorIter operator--(int);

            VectorIter operator+(difference_type n) const;
            VectorIter operator-(difference_type n) const;
            VectorIter &operator+=(difference_type n);
            VectorIter &operator-=(difference_type n);

            difference_type operator-(VectorIter other) const;

            reference operator[](difference_type pos) const;

            pointer base() const;

            // ReSharper disable once CppNonExplicitConversionOperator
            operator VectorIter<const T>(); // NOLINT(*-explicit-constructor)

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
            template<typename ...Args>
            iterator emplace(const_iterator pos, Args && ...args);
            iterator erase(const_iterator pos);
            iterator erase(const_iterator first, const_iterator last);
            void push_back(const value_type &value);
            void push_back(value_type &&value);
            template<typename ...Args>
            reference emplace_back(Args && ...args);
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
            void _clear();
            void _reserve_exact(size_type cap);
            void _reserve_amortized(size_type cap);
            void _expand_storage(size_type pos, size_type cnt);
            void _collapse_storage(size_type pos, size_type cnt);

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

    }

    using _detail::Vector;
}
