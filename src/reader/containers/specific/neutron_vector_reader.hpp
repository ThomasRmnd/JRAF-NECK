#ifndef JRAFNECK_READER_CONTAINERS_SPECIFIC_NEUTRONVECTORREADER_HPP_
#define JRAFNECK_READER_CONTAINERS_SPECIFIC_NEUTRONVECTORREADER_HPP_

#include <ctime>
#include <iterator>
#include <type_traits>

#include "reader/containers/vector_reader.hpp"

template<typename _Tp>
struct add_identify { typedef _Tp type; };

template<typename _Tp>
struct add_cref { typedef const _Tp& type; };

template<typename _Tp>
struct add_cptr { typedef const _Tp* type; };

template <typename T>
struct remove_cvref : std::remove_cv<typename std::remove_reference<T>::type> {};

template<class _Derived, template<typename> class _AddType>
struct neutron_reader_base {

protected:

    typedef typename _AddType<double>::type floating_type;
    typedef typename _AddType<time_t>::type time_type;
    typedef typename _AddType<int>::type    integer_type;

public:

    neutron_reader_base(floating_type posx_, floating_type posy_, floating_type posz_, floating_type e_, time_type sec_, integer_type nsec_) :
        posx{posx_},
        posy{posy_},
        posz{posz_},
        e{e_},
        sec{sec_},
        nsec{nsec_}
    {}

protected:

    neutron_reader_base() :
        posx{0.0},
        posy{0.0},
        posz{0.0},
        e{0.0},
        sec{0l},
        nsec{0}
    {}

    floating_type posx;
    floating_type posy;
    floating_type posz;
    floating_type e;
    time_type     sec;
    integer_type  nsec;

};

struct neutron_reader_value : neutron_reader_base<neutron_reader_value, add_identify> {

protected:

    typedef neutron_reader_base<neutron_reader_value, add_identify> base_type;
    typedef typename base_type::floating_type                       floating_type;
    typedef typename base_type::time_type                           time_type;
    typedef typename base_type::integer_type                        integer_type;

public:

    using base_type::posx;
    using base_type::posy;
    using base_type::posz;
    using base_type::e;
    using base_type::sec;
    using base_type::nsec;

    neutron_reader_value() : base_type() {}
    neutron_reader_value(floating_type posx_, floating_type posy_, floating_type posz_, floating_type e_, time_type sec_, integer_type nsec_) : 
        base_type{posx_, posy_, posz_, e_, sec_, nsec_} 
    {}

};

struct neutron_reader_const_reference : neutron_reader_base<neutron_reader_const_reference, add_cref> {

protected:

    typedef neutron_reader_base<neutron_reader_const_reference, add_cref> base_type;
    typedef typename base_type::floating_type                             floating_type;
    typedef typename base_type::time_type                                 time_type;
    typedef typename base_type::integer_type                              integer_type;

    friend struct neutron_reader_reference;
    friend struct neutron_reader_iterator;
    friend struct neutron_reader_const_iterator;
    friend class neutron_vector_reader;

    neutron_reader_const_reference(floating_type posx_, floating_type posy_, floating_type posz_, floating_type e_, time_type sec_, integer_type nsec_) : 
        base_type{posx_, posy_, posz_, e_, sec_, nsec_} 
    {}

public:

    using base_type::posx;
    using base_type::posy;
    using base_type::posz;
    using base_type::e;
    using base_type::sec;
    using base_type::nsec;

    neutron_reader_const_reference(const neutron_reader_const_reference&) = default;

    operator neutron_reader_value() const { return neutron_reader_value{posx, posy, posz, e, sec, nsec}; }

};

struct neutron_reader_reference : neutron_reader_base<neutron_reader_reference, std::add_lvalue_reference> {

protected:

    typedef neutron_reader_base<neutron_reader_reference, std::add_lvalue_reference> base_type;
    typedef typename base_type::floating_type                                        floating_type;
    typedef typename base_type::time_type                                            time_type;
    typedef typename base_type::integer_type                                         integer_type;

    friend struct neutron_reader_iterator;
    friend struct neutron_reader_const_iterator;
    friend class neutron_vector_reader;

    neutron_reader_reference(floating_type posx_, floating_type posy_, floating_type posz_, floating_type e_, time_type sec_, integer_type nsec_) :
        base_type{posx_, posy_, posz_, e_, sec_, nsec_}
    {}

public:

    using base_type::posx;
    using base_type::posy;
    using base_type::posz;
    using base_type::e;
    using base_type::sec;
    using base_type::nsec;

    neutron_reader_reference(const neutron_reader_reference&) = default;

    operator neutron_reader_value() const { return neutron_reader_value{posx, posy, posz, e, sec, nsec}; }
    operator neutron_reader_const_reference() const { return neutron_reader_const_reference{posx, posy, posz, e, sec, nsec}; }

    neutron_reader_reference& operator=(const neutron_reader_value& other) {
        posx = other.posx;
        posy = other.posy;
        posz = other.posz;
        e    = other.e;
        sec  = other.sec;
        nsec = other.nsec;
        return *this;
    }

    neutron_reader_reference& operator=(const neutron_reader_reference& other) {
        posx = other.posx;
        posy = other.posy;
        posz = other.posz;
        e    = other.e;
        sec  = other.sec;
        nsec = other.nsec;
        return *this;
    }

    neutron_reader_reference& operator=(const neutron_reader_const_reference& other) {
        posx = other.posx;
        posy = other.posy;
        posz = other.posz;
        e    = other.e;
        sec  = other.sec;
        nsec = other.nsec;
        return *this;
    }

};

class neutron_reader_iterator_base : public neutron_reader_base<neutron_reader_iterator_base, std::add_pointer> {

protected:

    typedef neutron_reader_base<neutron_reader_iterator_base, std::add_pointer> base_type;
    typedef typename base_type::floating_type                                   floating_type;
    typedef typename base_type::time_type                                       time_type;
    typedef typename base_type::integer_type                                    integer_type;

public:

    typedef std::random_access_iterator_tag                                     iterator_category;
    typedef neutron_reader_value                                                value_type;
    typedef std::ptrdiff_t                                                      difference_type;
    typedef neutron_reader_value*                                               pointer_type;

    neutron_reader_iterator_base(floating_type posx_, floating_type posy_, floating_type posz_, floating_type e_, time_type sec_, integer_type nsec_) :
        base_type{posx_, posy_, posz_, e_, sec_, nsec_}
    {}

    void bump_up() {
        ++posx;
        ++posy;
        ++posz;
        ++e;
        ++sec;
        ++nsec;
    }

    void bump_down() {
        --posx;
        --posy;
        --posz;
        --e;
        --sec;
        --nsec;
    }

    void incr(difference_type n) {
        posx += n;
        posy += n;
        posz += n;
        e    += n;
        sec  += n;
        nsec += n;
    }

    friend bool operator==(const neutron_reader_iterator_base& lhs, const neutron_reader_iterator_base& rhs) {
        return lhs.posx == rhs.posx;
    }

    friend bool operator!=(const neutron_reader_iterator_base& lhs, const neutron_reader_iterator_base& rhs) {
        return lhs.posx != rhs.posx;
    }

    friend bool operator<(const neutron_reader_iterator_base& lhs, const neutron_reader_iterator_base& rhs) {
        return lhs.posx < rhs.posx;
    }

    friend bool operator>(const neutron_reader_iterator_base& lhs, const neutron_reader_iterator_base& rhs) {
        return lhs.posx > rhs.posx;
    }

    friend bool operator<=(const neutron_reader_iterator_base& lhs, const neutron_reader_iterator_base& rhs) {
        return lhs.posx <= rhs.posx;
    }

    friend bool operator>=(const neutron_reader_iterator_base& lhs, const neutron_reader_iterator_base& rhs) {
        return lhs.posx >= rhs.posx;
    }

    friend difference_type operator-(const neutron_reader_iterator_base& lhs, const neutron_reader_iterator_base& rhs) {
        return lhs.posx - rhs.posx;
    }

};

class neutron_reader_iterator : public neutron_reader_iterator_base {

protected:

    typedef neutron_reader_iterator_base      base_type;
    typedef typename base_type::floating_type floating_type;
    typedef typename base_type::time_type     time_type;
    typedef typename base_type::integer_type  integer_type;

    friend struct neutron_reader_const_iterator;

public:

    typedef neutron_reader_reference          reference;
    typedef neutron_reader_reference*         pointer;
    typedef neutron_reader_iterator           iterator;
    typedef std::ptrdiff_t                    difference_type;

    neutron_reader_iterator() :
        neutron_reader_iterator_base{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}
    {}

    neutron_reader_iterator(floating_type posx_, floating_type posy_, floating_type posz_, floating_type e_, time_type sec_, integer_type nsec_) :
        neutron_reader_iterator_base{posx_, posy_, posz_, e_, sec_, nsec_}
    {}

    neutron_reader_iterator _const_cast() const {
        return *this;
    }

    reference operator*() const {
        return reference{*posx, *posy, *posz, *e, *sec, *nsec};
    }

    struct reference_wrapper {
        
        reference ref;

        reference_wrapper(reference r) : ref{r} {}
        reference* operator->() { return &ref; }

    };

    reference_wrapper operator->() const {
        return reference_wrapper{**this};
    }

    iterator& operator++() {
        bump_up();
        return *this;
    }

    iterator operator++(int) {
        iterator tmp = *this;
        bump_up();
        return tmp;
    }

    iterator& operator--() {
        bump_down();
        return *this;
    }

    iterator operator--(int) {
        iterator tmp = *this;
        bump_down();
        return tmp;
    }

    iterator& operator+=(difference_type n) {
        incr(n);
        return *this;
    }

    iterator& operator-=(difference_type n) {
        incr(-n);
        return *this;
    }

    reference operator[](difference_type n) const {
        return *(*this + n);
    }

    friend iterator operator+(const iterator& it, difference_type n) {
        iterator tmp = it;
        tmp += n;
        return tmp;
    }

    friend iterator operator+(difference_type n, const iterator& it) {
        return it + n;
    }

    friend iterator operator-(const iterator& it, difference_type n) {
        iterator tmp = it;
        tmp -= n;
        return tmp;
    }

};

class neutron_reader_const_iterator : public neutron_reader_iterator_base {

protected:

    typedef neutron_reader_iterator_base                 base_type;
    typedef typename base_type::floating_type            floating_type;
    typedef typename base_type::time_type                time_type;
    typedef typename base_type::integer_type             integer_type;
    typedef typename std::add_pointer<typename std::add_const<typename std::remove_pointer<typename remove_cvref<floating_type>::type>::type>::type>::type const_floating_type;
    typedef typename std::add_pointer<typename std::add_const<typename std::remove_pointer<typename remove_cvref<time_type>::type>::type>::type>::type     const_time_type;
    typedef typename std::add_pointer<typename std::add_const<typename std::remove_pointer<typename remove_cvref<integer_type>::type>::type>::type>::type  const_integer_type;

public:

    typedef neutron_reader_const_reference               reference;
    typedef neutron_reader_const_reference               const_reference;
    typedef neutron_reader_const_reference*              pointer;
    typedef neutron_reader_const_iterator                const_iterator;
    typedef std::ptrdiff_t                               difference_type;

    neutron_reader_const_iterator() :
        base_type{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}
    {}

    neutron_reader_const_iterator(floating_type posx_, floating_type posy_, floating_type posz_, floating_type e_, time_type sec_, integer_type nsec_) :
        base_type{posx_, posy_, posz_, e_, sec_, nsec_}
    {}

    neutron_reader_const_iterator(const_floating_type posx_, const_floating_type posy_, const_floating_type posz_, const_floating_type e_, const_time_type sec_, const_integer_type nsec_) :
        base_type{const_cast<floating_type>(posx_), const_cast<floating_type>(posy_), const_cast<floating_type>(posz_), const_cast<floating_type>(e_), const_cast<time_type>(sec_), const_cast<integer_type>(nsec_)}
    {}

    neutron_reader_const_iterator(const neutron_reader_iterator& it) :
        base_type{it.posx, it.posy, it.posz, it.e, it.sec, it.nsec}
    {}

    neutron_reader_iterator _const_cast() const {
        return neutron_reader_iterator{posx, posy, posz, e, sec, nsec};
    }

    const_reference operator*() const {
        return const_reference{*posx, *posy, *posz, *e, *sec, *nsec};
    }

    struct reference_wrapper {

        const_reference ref;

        reference_wrapper(const_reference r) : ref{r} {}
        const const_reference* operator->() const { return &ref; }

    };

    reference_wrapper operator->() const {
        return reference_wrapper{**this};
    }

    const_iterator& operator++() {
        bump_up();
        return *this;
    }

    const_iterator operator++(int) {
        const_iterator tmp = *this;
        bump_up();
        return tmp;
    }

    const_iterator& operator--() {
        bump_down();
        return *this;
    }

    const_iterator operator--(int) {
        const_iterator tmp = *this;
        bump_down();
        return tmp;
    }
    
    const_iterator& operator+=(difference_type n) {
        incr(n);
        return *this;
    }

    const_iterator& operator-=(difference_type n) {
        incr(-n);
        return *this;
    }

    const_reference operator[](difference_type n) {
        return *(*this + n);
    }

    friend const_iterator operator+(const const_iterator& it, difference_type n) {
        const_iterator tmp = it;
        tmp += n;
        return tmp;
    }

    friend const_iterator operator+(difference_type n, const const_iterator& it) {
        return it + n;
    }

    friend const_iterator operator-(const const_iterator& it, difference_type n) {
        const_iterator tmp = it;
        tmp -= n;
        return tmp;
    }

};

class neutron_vector_reader {

public:

    typedef neutron_reader_value                  value_type;
    typedef std::size_t                           size_type;
    typedef std::ptrdiff_t                        difference_type;
    typedef neutron_reader_reference              reference;
    typedef neutron_reader_const_reference        const_reference;
    typedef neutron_reader_reference*             pointer;
    typedef neutron_reader_const_reference*       const_pointer;
    typedef neutron_reader_iterator               iterator;
    typedef neutron_reader_const_iterator         const_iterator;
    typedef std::reverse_iterator<iterator>       reverse_itertor;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    const_iterator begin() const { 
        return const_iterator{m_posx.data()->data(), m_posy.data()->data(), m_posz.data()->data(), m_e.data()->data(), m_sec.data()->data(), m_nsec.data()->data()};
    }
    const_iterator cbegin() const {
        return const_iterator{m_posx.data()->data(), m_posy.data()->data(), m_posz.data()->data(), m_e.data()->data(), m_sec.data()->data(), m_nsec.data()->data()};
    }

    const_iterator end() const {
        return const_iterator{m_posx.data()->data() + m_posx.size(), m_posy.data()->data() + m_posy.size(), m_posz.data()->data() + m_posz.size(), m_e.data()->data() + m_e.size(), m_sec.data()->data() + m_sec.size(), m_nsec.data()->data() + m_nsec.size()};
    }
    const_iterator cend() const {
        return const_iterator{m_posx.data()->data() + m_posx.size(), m_posy.data()->data() + m_posy.size(), m_posz.data()->data() + m_posz.size(), m_e.data()->data() + m_e.size(), m_sec.data()->data() + m_sec.size(), m_nsec.data()->data() + m_nsec.size()};
    }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(cbegin()); }
    
    const_reverse_iterator rend() const { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(cend()); }

    bool empty() const { return m_posx.empty(); }
    size_type size() const { return m_posx.size(); }

    const_reference operator[](size_type n) const { return const_reference{m_posx[n], m_posy[n], m_posz[n], m_e[n], m_sec[n], m_nsec[n]}; }
    const_reference at(size_type n) const { return const_reference{m_posx.at(n), m_posy.at(n), m_posz.at(n), m_e.at(n), m_sec.at(n), m_nsec.at(n)}; }

private:

    jraf::vector_reader<double> m_posx;
    jraf::vector_reader<double> m_posy;
    jraf::vector_reader<double> m_posz;
    jraf::vector_reader<double> m_e;
    jraf::vector_reader<time_t> m_sec;
    jraf::vector_reader<int> m_nsec;

};

class neutron_metadata_vector_reader {

private:

    jraf::vector_reader<double> m_totq;
    jraf::vector_reader<double> m_meanq;
    jraf::vector_reader<double> m_stdq;
    jraf::vector_reader<double> m_minq;
    jraf::vector_reader<double> m_maxq;
    jraf::vector_reader<double> m_meant;
    jraf::vector_reader<double> m_stdt;
    jraf::vector_reader<std::size_t> m_npmt;
    jraf::vector_reader<std::size_t> m_nhit;
    jraf::vector_reader<double> m_meanhit;
    jraf::vector_reader<double> m_stdhit;

};

#endif // JRAFNECK_READER_CONTAINERS_SPECIFIC_NEUTRONVECTORREADER_HPP_