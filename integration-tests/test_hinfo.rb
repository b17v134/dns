require 'json'
require 'open3'
require "test/unit"

class TestHinfo < Test::Unit::TestCase
    def test_hinfo
        stdout, stderr, status = Open3.capture3('./integration-tests-dns/bin/resolv -s 10.23.0.7 example.test-bind -t hinfo -o json')
 
        assert_equal(0, status)
        assert_equal("", stderr)
        data = JSON.parse(stdout)
        assert_equal([{
            'name'=> 'example.test-bind.', 
            'ttl'=> 300, 
            'class'=> 'IN', 
            'type'=> 'HINFO', 
            'rdata'=> '"i686" "unix"'
        }], data['answers'])
    end
end