#include <boost/program_options.hpp>

#include <ostream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <iomanip>

#include "assert_hash.hpp"

namespace po = boost::program_options;
namespace ah = assert_hash;

static void usage( const po::options_description& options )
{
    std::cout << "usage: assert_hashes [options]" << std::endl;
    std::cout << options << std::endl;

    std::cout << "\nexamples:\n";
    std::cout << "    assert_hashes -f /lala/foo/bar.cpp -a assert -a ASSERT -d 1 -h 4\n";
    std::cout << std::endl;
}

template < typename T >
void create_output( const std::string& file, ah::line_number_t line, unsigned directories, std::ostream& output )
{
    const T hash = ah::file_and_line_hash< T >( file.c_str(), line, directories );

    std::cout << "#0x" << std::hex << std::setw( sizeof( T ) * 2 ) << std::setfill( '0' ) << hash
        << ' ' << file << ':' << std::dec << line << '\n';
}

template <>
void create_output< std::uint8_t >( const std::string& file, ah::line_number_t line, unsigned directories, std::ostream& output )
{
    const int hash = ah::file_and_line_hash< std::uint8_t >( file.c_str(), line, directories );

    std::cout << "#0x" << std::hex << std::setw( 2 ) << std::setfill( '0' ) << hash
        << ' ' << file << ':' << std::dec << line << '\n';
}

using hash_outputter = std::function< void ( const std::string&, ah::line_number_t, unsigned, std::ostream& ) >;

static void create_assert_hashes( const std::string& file, const hash_outputter& outputter, unsigned directories, const std::vector< std::string >& asserts, std::ostream& output )
{
    std::ifstream input( file );

    if ( !input )
        throw std::runtime_error( std::string( "unable to open: \"" ) + file + "\"." );

    const std::string file_name = ah::file_base( file.c_str(), directories );
    std::string line;

    for ( ah::line_number_t line_number = 1; std::getline( input, line ); ++line_number )
    {
        for ( const auto& assert_name : asserts )
        {
            if ( line.find( assert_name ) != std::string::npos )
                outputter( file_name, line_number, directories, output );
        }
    }
}

static const std::map< unsigned, hash_outputter > all_available_outputters = {
    { sizeof( std::uint8_t ),  create_output< std::uint8_t > },
    { sizeof( std::uint16_t ), create_output< std::uint16_t > },
    { sizeof( std::uint32_t ), create_output< std::uint32_t > },
    { sizeof( std::uint64_t ), create_output< std::uint64_t > }
};

int main( int argc, const char** argv )
{
    po::options_description options("options");
    options.add_options()
        ( "help,h", "produce help message" )
        ( "assert,a", po::value< std::vector< std::string> >(), "names of the assert function(s) D=\"assert\"")
        ( "directories,d", po::value< unsigned >()->default_value( 0 ), "max. number of directories to be included in file name (0=all)" )
        ( "hash_size,s", po::value< unsigned >()->default_value( 4 ), "sizeof hash value in bytes (1, 2, 4 or 8)" )
        ( "file,f", po::value< std::string >(), "input file name" )
    ;

    po::variables_map variables;
    std::vector< std::string > asserts = { "assert" };

    try
    {
        po::store( po::parse_command_line( argc, argv, options ), variables );
        po::notify(variables);

        if ( variables.count( "help" ) )
        {
            usage( options );
        }
        else
        {
            if ( variables.count( "assert" ) )
                asserts = variables[ "assert" ].as< std::vector< std::string> >();

            if ( variables.count( "file" ) == 0 )
                throw std::runtime_error( "the option '--file' is required but missing" );

            const auto outputter = all_available_outputters.find( variables[ "hash_size" ].as< unsigned >() );
            if ( outputter == all_available_outputters.end() )
                throw std::runtime_error( "unsupported hash size: \"" + std::to_string( variables[ "hash_size" ].as< unsigned >() ) + "\"" );

            create_assert_hashes( variables[ "file" ].as< std::string >(), outputter->second, variables[ "directories" ].as< unsigned >(), asserts, std::cout );

            return EXIT_SUCCESS;
        }
    }
    catch ( const std::exception& error )
    {
        std::cerr << "error: " << error.what() << std::endl;
    }
    catch ( ... )
    {
        std::cerr << "unknow error!" << std::endl;
    }

    return EXIT_FAILURE;
}