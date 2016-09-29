#ifndef ASSERT_HASH_HPP_INCLUDE_GUARD
#define ASSERT_HASH_HPP_INCLUDE_GUARD

#include <climits>

namespace assert_hash {

    using line_number_t = unsigned;

    namespace details {

        constexpr const char* end_of_string( const char* const str )
        {
            return *str == 0
                ? str
                : end_of_string( str + 1 );
        }

        constexpr const char* file_base_impl( const char* const file, const char* const end_of_file, const unsigned directories )
        {
            return directories == 0 || end_of_file == file
                ? end_of_file
                : ( *end_of_file == '\\' || *end_of_file == '/'
                    ? (
                        directories == 1
                            ? end_of_file
                            : file_base_impl( file, end_of_file - 1, directories - 1 )
                    )
                    : file_base_impl( file, end_of_file - 1, directories )
                );
        }

        template < typename T >
        struct check_checksum_type
        {
            static_assert( std::is_integral< T >::value, "hash type must be integral" );
            static_assert( std::is_unsigned< T >::value, "hash type must be unsigned" );
        };

        template < typename T >
        constexpr T add_char( const T value, const char* const file, unsigned exponent )
        {
            return value + ( *file << ( CHAR_BIT * exponent ) );
        }

        template < typename T >
        constexpr unsigned next_exponent( unsigned exponent )
        {
            return exponent + 1 == sizeof( T )
                ? 0
                : exponent + 1;
        }

        template < typename T >
        constexpr T file_hash_impl( const T value, const char* const file, unsigned exponent )
        {
            return *file == 0
                ? value
                : file_hash_impl( add_char< T >( value, file, exponent ), file + 1, next_exponent< T >( exponent ) );
        }

        // to prevent "warning: shift count >= width of type [-Wshift-count-overflow]"
        template < typename T >
        constexpr line_number_t shift_line( const line_number_t line )
        {
            return sizeof( line ) <= sizeof( T )
                ? 0
                : line >> ( CHAR_BIT * sizeof( T ) );
        }

        template < typename T >
        constexpr T line_hash_impl( const T value, const line_number_t line )
        {
            return line == 0
                ? value
                : line_hash_impl< T >( value + static_cast< T >( line ), shift_line< T >( line ) );
        }
    }

    constexpr const char* file_base( const char* const file, const unsigned directories = 0 )
    {
        return directories == 0
            ? file
            : details::file_base_impl( file, details::end_of_string( file ), directories );
    }

    template < typename T >
    constexpr T file_hash( const char* const file )
    {
        return static_cast< void >( details::check_checksum_type< T >{} ),
            details::file_hash_impl< T >( 0, file, 0 );
    }

    template < typename T >
    constexpr T line_hash( const line_number_t line )
    {
        return static_cast< void >( details::check_checksum_type< T >{} ),
            details::line_hash_impl< T >( 0 , line );
    }

    template < typename T >
    constexpr T file_and_line_hash( const char* const file, const line_number_t line, const unsigned directories = 0 )
    {
        return file_hash< T >( file_base( file, directories ) ) + line_hash< T >( line );
    }
}

#endif
