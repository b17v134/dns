require 'open3'
require "test/unit"

class TestHelp < Test::Unit::TestCase
    def test_Help
        usage_string =  <<-TEXT
Usage: resolv [options...] <value>
 -A <file>                  CA certificate
 -E, --cert <certificate>   Certificate
 -c, --class                Query class [default: IN]
 -h, --help                 Show help and exit
 -s, --server <ip>          Server ip
 -o  <output_format>        Formatting output (json, plain) [default: plain]
 -p, --port <number>        Port number [default: 53]
 -r, --protocol <name>      Protocol (https, tcp, tls, udp) [default: udp]
 -t, --type                 Query type [default: A]
 -v, --verbose              Verbose mode
 -V, --version              Show version number and exit

TEXT
        for help_param in ['-h', '--help']
            stdout, stderr, status = Open3.capture3("./integration-tests-dns/bin/resolv " + help_param)
 
            assert_equal(0, status)
            assert_equal("", stderr)
            assert_equal(usage_string, stdout)
        end
    end
end