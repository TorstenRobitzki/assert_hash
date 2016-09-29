#define BOOST_TEST_MODULE
#include <boost/test/included/unit_test.hpp>

#include <assert_hash.hpp>

namespace ah = assert_hash;

BOOST_AUTO_TEST_CASE( file_base_0 )
{
    BOOST_CHECK_EQUAL( ah::file_base( "a" ), "a" );
    BOOST_CHECK_EQUAL( ah::file_base( "a.txt" ), "a.txt" );
    BOOST_CHECK_EQUAL( ah::file_base( ".a.txt" ), ".a.txt" );
    BOOST_CHECK_EQUAL( ah::file_base( "c:\\asd\\.a.txt" ), "c:\\asd\\.a.txt" );
}

BOOST_AUTO_TEST_CASE( end_of_string )
{
    const char* const c = "asdasd";
    BOOST_CHECK_EQUAL( ah::details::end_of_string( c ) - c, 6 );
    BOOST_CHECK_EQUAL( ah::details::end_of_string( c + 6 ) - c, 6 );
}

BOOST_AUTO_TEST_CASE( file_base_1 )
{
    BOOST_CHECK_EQUAL( ah::file_base( "a", 1 ), "a" );
    BOOST_CHECK_EQUAL( ah::file_base( "a.txt", 1 ), "a.txt" );
    BOOST_CHECK_EQUAL( ah::file_base( ".a.txt", 1 ), ".a.txt" );
    BOOST_CHECK_EQUAL( ah::file_base( "dev/null", 1 ), "/null" );
    BOOST_CHECK_EQUAL( ah::file_base( "/Users/todi/assert_hash/build", 1 ), "/build" );
    BOOST_CHECK_EQUAL( ah::file_base( "c:\\asda\\asdasd\\asd\\.a.txt", 1 ), "\\.a.txt" );
}

BOOST_AUTO_TEST_CASE( file_base_2 )
{
    BOOST_CHECK_EQUAL( ah::file_base( "a", 2 ), "a" );
    BOOST_CHECK_EQUAL( ah::file_base( "a.txt", 2 ), "a.txt" );
    BOOST_CHECK_EQUAL( ah::file_base( ".a.txt", 2 ), ".a.txt" );
    BOOST_CHECK_EQUAL( ah::file_base( "dev/null", 2 ), "dev/null" );
    BOOST_CHECK_EQUAL( ah::file_base( "/Users/todi/assert_hash/build", 2 ), "/assert_hash/build" );
    BOOST_CHECK_EQUAL( ah::file_base( "c:\\asda\\asdasd\\asd\\.a.txt", 2 ), "\\asd\\.a.txt" );
}

BOOST_AUTO_TEST_CASE( file_hash_16 )
{
    BOOST_CHECK_EQUAL( ah::file_hash< std::uint16_t >( "" ), 0 );
    BOOST_CHECK_EQUAL( ah::file_hash< std::uint16_t >( "a" ), 'a' );
    BOOST_CHECK_EQUAL( ah::file_hash< std::uint16_t >( "ab" ), 'a' + ( 'b' << 8 ) );
    BOOST_CHECK_EQUAL( ah::file_hash< std::uint16_t >( "abc" ), ( 'a' + ( 'b' << 8 ) + 'c' ) & 0xffff );
    BOOST_CHECK_EQUAL( ah::file_hash< std::uint16_t >( "abcd" ),
        ( 'a' + ( 'b' << 8 ) + 'c' + ( 'd' << 8 ) ) & 0xffff );
}

BOOST_AUTO_TEST_CASE( file_hash_8 )
{
    BOOST_CHECK_EQUAL( ah::file_hash< std::uint8_t >( "" ), 0 );
    BOOST_CHECK_EQUAL( ah::file_hash< std::uint8_t >( "a" ), 'a' );
    BOOST_CHECK_EQUAL( ah::file_hash< std::uint8_t >( "ab" ), 'a' + 'b' );
    BOOST_CHECK_EQUAL( ah::file_hash< std::uint8_t >( "abc" ), ( 'a' + 'b' + 'c' ) & 0xff );
}

BOOST_AUTO_TEST_CASE( line_hash_8 )
{
    BOOST_CHECK_EQUAL( ah::line_hash< std::uint8_t >( 0 ), 0 );
    BOOST_CHECK_EQUAL( ah::line_hash< std::uint8_t >( 0x12 ), 0x12 );
    BOOST_CHECK_EQUAL( ah::line_hash< std::uint8_t >( 0x12345678 ), ( 0x78 + 0x56 + 0x34 + 0x12 ) & 0xff );
}

BOOST_AUTO_TEST_CASE( line_hash_16 )
{
    BOOST_CHECK_EQUAL( ah::line_hash< std::uint16_t >( 0 ), 0 );
    BOOST_CHECK_EQUAL( ah::line_hash< std::uint16_t >( 0x12 ), 0x12 );
    BOOST_CHECK_EQUAL( ah::line_hash< std::uint16_t >( 0x12345678 ), ( 0x5678 + 0x1234 ) & 0xffff );
}

BOOST_AUTO_TEST_CASE( line_hash_32 )
{
    BOOST_CHECK_EQUAL( ah::line_hash< std::uint32_t >( 0 ), 0 );
    BOOST_CHECK_EQUAL( ah::line_hash< std::uint32_t >( 0x12 ), 0x12 );
    BOOST_CHECK_EQUAL( ah::line_hash< std::uint32_t >( 0x12345678 ), 0x12345678 );
}

BOOST_AUTO_TEST_CASE( file_and_line_hash )
{
    constexpr std::uint16_t hash1 = ah::file_and_line_hash< std::uint16_t >( __FILE__, __LINE__ );
    constexpr std::uint16_t hash2 = ah::file_and_line_hash< std::uint16_t >( __FILE__, __LINE__ );

    BOOST_CHECK_NE( hash1, hash2 );
}

template < const int >
struct force_const {};

BOOST_AUTO_TEST_CASE( check_const_expr )
{
    static constexpr const char* c = "test";
    force_const< ah::file_base( c, 1 ) - c >();
    force_const< ah::file_hash< unsigned >( c ) >();
    force_const< ah::file_and_line_hash< std::uint16_t >( __FILE__, __LINE__ ) >();
}
