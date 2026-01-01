
#include "etl/vector.h"

namespace advt
{

template <typename t_t, size_t n_t, size_t m_t>
class multi_vector;

template <typename t_t, size_t n_t, size_t m_t>
class multi_vector_iterator;

template <typename t_t, size_t n_t, size_t m_t>
class multi_vector_subview {
private:
    multi_vector<t_t, n_t, m_t> *parent;
    size_t m;

public:
    multi_vector_subview(multi_vector<t_t, n_t, m_t> *parent, size_t m) : parent(parent), m(m)
    {
    }

    void push_back(const t_t &value)
    {
        parent->push_back(m, value);
    }

    size_t size()
    {
        return parent->size(m);
    }

    auto &&at(size_t i)
    {
        return parent->at(m, i);
    }

    void erase(size_t i)
    {
        return parent->erase(m, i);
    }

    t_t *begin()
    {
        return parent->data(m);
    }

    t_t *end()
    {
        return parent->data(m) + parent->size(m);
    }
};

template <typename t_t, size_t n_t, size_t m_t>
class multi_vector_iterator {
private:
    multi_vector<t_t, n_t, m_t> *parent;
    size_t m;

public:
    friend class multi_vector<t_t, n_t, m_t>;
    using iterator_category = etl::forward_iterator_tag;
    //using difference_type   = etl::ptrdiff_t;

    multi_vector_iterator(multi_vector<t_t, n_t, m_t> *parent, size_t m) : parent(parent), m(m)
    {
    }

    // Prefix increment
    multi_vector_iterator &operator++()
    {
        m++;
        *this = parent->get_iterator(m);
        return *this;
    }

    friend bool operator==(const multi_vector_iterator &a, const multi_vector_iterator &b)
    {
        return a.parent == b.parent && a.m == b.m;
    };

    friend bool operator!=(const multi_vector_iterator &a, const multi_vector_iterator &b)
    {
        return !(a == b);
    };

    multi_vector_subview<t_t, n_t, m_t> operator*() const
    {
        return multi_vector_subview<t_t, n_t, m_t>(parent, m);
    }
};

template <typename t_t, size_t n_t, size_t m_t>
class multi_vector {
private:
    etl::vector<size_t, m_t> indexes = etl::vector<size_t, m_t>(1); //m
    etl::vector<t_t, n_t> m_data; //n

    multi_vector_iterator<t_t, n_t, m_t> get_iterator(size_t m)
    {
        assert(m <= size());
        // if (!(m <= size()))
        //     throw std::runtime_error("Out of range!");
        return multi_vector_iterator<t_t, n_t, m_t>(this, m);
    }

    multi_vector_subview<t_t, n_t, m_t> get_subview(size_t m)
    {
        assert(m <= size());
        // if (!(m <= size()))
        //     throw std::runtime_error("Out of range!");
        return multi_vector_subview<t_t, n_t, m_t>(this, m);
    }

public:
    using subview = multi_vector_subview<t_t, n_t, m_t>;
    friend class multi_vector_iterator<t_t, n_t, m_t>;

    multi_vector_iterator<t_t, n_t, m_t> begin()
    {
        return get_iterator(0);
    }

    multi_vector_iterator<t_t, n_t, m_t> end()
    {
        return get_iterator(size());
    }

    void emplace_back()
    {
        indexes.insert(indexes.begin() + indexes.size() - 1, indexes.back());
    }

    void push_back(size_t m, const t_t &value)
    {
        assert(m <= size());
        // if (!(m < size()))
        //     throw std::runtime_error("Out of range!");
        //get m's back
        size_t m_back = indexes.at(m + 1);
        etl::for_each(indexes.begin() + m + 1, indexes.end(), [](auto &a) { ++a; });
        m_data.insert(m_data.begin() + m_back, value);
    }

    void insert(size_t m, size_t i, const t_t &value)
    {
        assert(m < size() && i <= size(m));
        // if (!(m < size() && i <= size(m)))
        //     throw std::runtime_error("Out of range!");
        //get m's back
        size_t m_pos = indexes.at(m) + i;
        etl::for_each(indexes.begin() + m + 1, indexes.end(), [](auto &a) { ++a; });
        m_data.insert(m_data.begin() + m_pos, value);
    }

    size_t size()
    {
        return indexes.size() - 1;
    }

    size_t size(size_t m)
    {
        assert(m < size());
        // if (!(m < size()))
        //     throw std::runtime_error("Out of range!");
        return indexes.at(m + 1) - indexes.at(m);
    }

    multi_vector_subview<t_t, n_t, m_t> at(size_t m)
    {
        assert(m < size());
        // if (!(size() != 0))
        //     throw std::runtime_error("Out of range!");
        return get_subview(m);
    }

    auto &&at(size_t m, size_t i)
    {
        assert(m < size() && i < size(m));
        // if (!(m < size() && i < size(m)))
        //     throw std::runtime_error("Out of range!");
        return m_data.at(indexes.at(m) + i);
    }

    multi_vector_subview<t_t, n_t, m_t> front()
    {
        assert(size() != 0);
        // if (!(size() != 0))
        //     throw std::runtime_error("Out of range!");
        return get_subview(0);
    }

    multi_vector_subview<t_t, n_t, m_t> back()
    {
        assert(size() != 0);
        // if (!(size() != 0))
        //     throw std::runtime_error("Out of range!");
        return get_subview(size() - 1);
    }

    void erase(multi_vector_iterator<t_t, n_t, m_t> iter)
    {
        erase(iter.m);
    }

    void erase(size_t m)
    {
        assert(m < size());
        // if (!(m < size()))
        //     throw std::runtime_error("Out of range!");
        size_t m_ind_start = indexes.at(m);
        size_t m_ind_end = indexes.at(m) + size(m);
        size_t m_size = size(m);
        etl::for_each(indexes.begin() + m + 1, indexes.end(), [&m_size](auto &a) { a -= m_size; });
        indexes.erase(indexes.begin() + m);
        m_data.erase(m_data.begin() + m_ind_start, m_data.begin() + m_ind_end);
    }

    void erase(size_t m, size_t i)
    {
        assert(m < size() && i < size(m));
        // if (!(m < size() && i < size(m)))
        //     throw std::runtime_error("Out of range!");
        size_t m_ind = indexes.at(m) + i;
        etl::for_each(indexes.begin() + m + 1, indexes.end(), [](auto &a) { --a; });
        m_data.erase(m_data.begin() + m_ind);
    }

    t_t *data(size_t m)
    {
        return m_data.data() + indexes.at(m);
    }
};

}