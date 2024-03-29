/**
 * @file MatrixView.h
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#ifndef MESHCAT_CPP_MATRIX_VIEW_H
#define MESHCAT_CPP_MATRIX_VIEW_H

#include <cassert>

#include <cstring>
#include <type_traits>

namespace MeshcatCpp
{

namespace MatrixViewInternal
{
/**
 * has_IsRowMajor is used to build a type-dependent expression that check if an
 * element has IsRowMajor argument. This specific implementation is used when
 * the the object has not IsRowMajor.
 */
template <typename T, typename = void> struct has_IsRowMajor : std::false_type
{
};

/**
 * has_IsRowMajor is used to build a type-dependent expression that check if an
 * element has IsRowMajor argument. This specific implementation is used when
 * the the object has not IsRowMajor, indeed <code>void_t<\endcode> is used to
 * detect ill-formed types in SFINAE context.
 */
template <typename T>
struct has_IsRowMajor<T, std::void_t<decltype(T::IsRowMajor)>> : std::true_type
{
};
} // namespace MatrixViewInternal

namespace details
{
template <class From, class To>
struct is_allowed_element_type_conversion
    : public std::integral_constant<bool, std::is_convertible<From (*)[], To (*)[]>::value>
{
};

} // namespace details

/**
 * Enum describing the possible matrix storage ordering
 */
enum class MatrixStorageOrdering
{
    RowMajor, /*!< Row Major ordering, i.e. matrix is serialized row by row */
    ColumnMajor /*!< Column Major ordering, i.e. matrix is serialized row by column */
};

/**
 * MatrixView implements a view interface of Matrices. Both RowMajor and ColumnMajor matrices are
 * supported.
 * @note The user should define the storage ordering when the MatrixView is created (the default
 order is RowMajor). However if the MatrixView is generated:
 *   - from an object having a public member called <code>IsRowMajor</code>, the correct storage
 order is chosen.
 *   - from another MatrixView, the correct storage order is chosen.
 * @note the original implementation can be found in
 https://github.com/robotology/idyntree/blob/a7fdef001d38e47aabcdc541607a255101212dd9/src/core/include/iDynTree/Core/MatrixView.h
 */
template <class ElementType> class MatrixView
{
public:
    using element_type = ElementType;
    using value_type = std::remove_cv_t<ElementType>;
    using index_type = std::ptrdiff_t;
    using pointer = element_type*;
    using reference = element_type&;

private:
    pointer m_storage;
    index_type m_rows;
    index_type m_cols;

    MatrixStorageOrdering m_storageOrder;

    index_type m_innerStride;
    index_type m_outerStride;

    index_type rawIndex(index_type row, index_type col) const
    {
        if (m_storageOrder == MatrixStorageOrdering::RowMajor)
        {
            return (col + this->m_outerStride * row);
        } else
        {
            return (this->m_outerStride * col + row);
        }
    }

public:
    MatrixView()
        : MatrixView(nullptr, 0, 0, MatrixStorageOrdering::RowMajor)
    {
    }
    MatrixView(const MatrixView& other)
        : MatrixView(other.m_storage, other.m_rows, other.m_cols, other.m_storageOrder)
    {
    }

    template <class OtherElementType,
              class = std::enable_if_t<details::is_allowed_element_type_conversion<
                  typename MatrixView<OtherElementType>::value_type,
                  value_type>::value>>
    constexpr MatrixView(const MatrixView<OtherElementType>& other)
        : MatrixView(other.data(), other.rows(), other.cols(), other.storageOrder())
    {
    }

    template <class Container,
              std::enable_if_t<std::is_const<element_type>::value
                                   && std::is_convertible<decltype(std::declval<Container>().data()),
                                                          pointer>::value
                                   && MatrixViewInternal::has_IsRowMajor<Container>::value
                                   && !std::is_same<Container, MatrixView>::value,
                               int> = 0>
    MatrixView(const Container& matrix)
        : MatrixView(matrix.data(),
                     matrix.rows(),
                     matrix.cols(),
                     Container::IsRowMajor ? MatrixStorageOrdering::RowMajor
                                           : MatrixStorageOrdering::ColumnMajor)
    {
    }

    template <class Container,
              std::enable_if_t<std::is_const<element_type>::value
                                   && std::is_convertible<decltype(std::declval<Container>().data()),
                                                          pointer>::value
                                   && !MatrixViewInternal::has_IsRowMajor<Container>::value
                                   && !std::is_same<Container, MatrixView>::value,
                               int> = 0>
    MatrixView(const Container& matrix,
               const MatrixStorageOrdering& order = MatrixStorageOrdering::RowMajor)
        : MatrixView(matrix.data(), matrix.rows(), matrix.cols(), order)
    {
    }

    template <class Container,
              std::enable_if_t<
                  std::is_convertible<decltype(std::declval<Container>().data()), pointer>::value
                      && MatrixViewInternal::has_IsRowMajor<Container>::value
                      && !std::is_same<Container, MatrixView>::value,
                  int> = 0>
    MatrixView(Container& matrix)
        : MatrixView(matrix.data(),
                     matrix.rows(),
                     matrix.cols(),
                     Container::IsRowMajor ? MatrixStorageOrdering::RowMajor
                                           : MatrixStorageOrdering::ColumnMajor)
    {
    }

    template <class Container,
              std::enable_if_t<
                  std::is_convertible<decltype(std::declval<Container>().data()), pointer>::value
                      && !MatrixViewInternal::has_IsRowMajor<Container>::value
                      && !std::is_same<Container, MatrixView>::value
                      && !std::is_same<Container,
                                       MatrixView<std::remove_pointer_t<
                                           decltype(std::declval<Container>().data())>>>::value,
                  int> = 0>
    MatrixView(Container& matrix,
               const MatrixStorageOrdering& order = MatrixStorageOrdering::RowMajor)
        : MatrixView(matrix.data(), matrix.rows(), matrix.cols(), order)
    {
    }

    MatrixView(pointer in_data,
               index_type in_rows,
               index_type in_cols,
               const MatrixStorageOrdering& order = MatrixStorageOrdering::RowMajor)
        : m_storage(in_data)
        , m_rows(in_rows)
        , m_cols(in_cols)
        , m_storageOrder(order)
        , m_innerStride(1)
        , m_outerStride(order == MatrixStorageOrdering::RowMajor ? in_cols : in_rows)
    {
    }

    const MatrixStorageOrdering& storageOrder() const noexcept
    {
        return m_storageOrder;
    }

    pointer data() const noexcept
    {
        return m_storage;
    }

    template <class OtherElementType,
              class = std::enable_if_t<
                  std::is_same_v<typename MatrixView<OtherElementType>::value_type, value_type>>>
    MatrixView& operator=(MatrixView<OtherElementType> other)
    {
        assert(other.rows() == this->rows());
        assert(other.cols() == this->cols());

        if (other.storageOrder() == this->storageOrder())
        {
            std::memcpy(this->m_storage,
                        other.data(),
                        this->rows() * this->cols() * sizeof(element_type));

            return *this;
        }

        for (index_type i = 0; i < this->rows(); i++)
        {
            for (index_type j = 0; j < this->cols(); j++)
            {
                this->operator()(i, j) = other(i, j);
            }
        }

        return *this;
    }

    /**
     * @name Matrix interface methods.
     * Methods exposing a matrix-like interface to MatrixView.
     *
     */
    ///@{
    reference operator()(index_type row, const index_type col) const
    {
        assert(row < this->rows());
        assert(col < this->cols());
        return this->m_storage[rawIndex(row, col)];
    }

    index_type rows() const noexcept
    {
        return this->m_rows;
    }

    index_type cols() const noexcept
    {
        return this->m_cols;
    }
    ///@}

    MatrixView
    block(index_type startingRow, index_type startingColumn, index_type rows, index_type cols) const
    {
        assert(rows <= this->rows());
        assert(cols <= this->cols());

        const index_type offset = rawIndex(startingRow, startingColumn);
        assert(offset < this->rows() * this->cols());

        MatrixView block;
        block = *this;
        block.m_rows = rows;
        block.m_cols = cols;
        block.m_storage = this->m_storage + offset;

        return block;
    }
};

template <class ElementType>
constexpr MatrixView<ElementType>
make_matrix_view(ElementType* ptr,
                 typename MatrixView<ElementType>::index_type rows,
                 typename MatrixView<ElementType>::index_type cols,
                 MatrixStorageOrdering order = MatrixStorageOrdering::RowMajor)
{
    return MatrixView<ElementType>(ptr, rows, cols, order);
}

template <class Container,
          std::enable_if_t<
              MatrixViewInternal::has_IsRowMajor<Container>::value
                  || std::is_same<MatrixView<typename Container::value_type>, Container>::value,
              int> = 0>
constexpr MatrixView<typename Container::value_type> make_matrix_view(Container& cont)
{
    return MatrixView<typename Container::value_type>(cont);
}

template <class Container,
          std::enable_if_t<MatrixViewInternal::has_IsRowMajor<Container>::value
                               || std::is_same<MatrixView<const typename Container::value_type>,
                                               Container>::value,
                           int> = 0>
constexpr MatrixView<const typename Container::value_type> make_matrix_view(const Container& cont)
{
    return MatrixView<const typename Container::value_type>(cont);
}

template <class Container,
          std::enable_if_t<
              !MatrixViewInternal::has_IsRowMajor<Container>::value
                  && !std::is_same<MatrixView<typename Container::value_type>, Container>::value,
              int> = 0>
constexpr MatrixView<typename Container::value_type>
make_matrix_view(Container& cont,
                 typename MatrixView<typename Container::value_type>::MatrixStorageOrdering order
                 = MatrixView<typename Container::value_type>::MatrixStorageOrdering::RowMajor)
{
    return MatrixView<typename Container::value_type>(cont, order);
}

template <class Container,
          std::enable_if_t<
              !MatrixViewInternal::has_IsRowMajor<Container>::value
                  && !std::is_same<MatrixView<typename Container::value_type>, Container>::value,
              int> = 0>
constexpr MatrixView<const typename Container::value_type> make_matrix_view(
    const Container& cont,
    typename MatrixView<const typename Container::value_type>::MatrixStorageOrdering order
    = MatrixView<const typename Container::value_type>::MatrixStorageOrdering::RowMajor)
{
    return MatrixView<const typename Container::value_type>(cont, order);
}

} // namespace MeshcatCpp

#endif /* IDYNTREE_MATRIX_MATRIX_VIEW_H */
