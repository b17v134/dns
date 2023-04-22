require 'json'
require 'open3'
require "test/unit"

class TestSoa < Test::Unit::TestCase
    def test_soa
        stdout, stderr, status = Open3.capture3('./integration-tests-dns/bin/resolv -s 10.23.0.7 example.test-bind -t soa -o json')
 
        assert_equal(0, status)
        assert_equal("", stderr)
        data = JSON.parse(stdout)
        assert_equal([{
            'name'=> 'example.test-bind.', 
            'ttl'=> 300, 
            'class'=> 'IN', 
            'type'=> 'SOA', 
            'rdata'=> 'ns.example.test-bind. mail.example.test-bind. 20230409 604800 86400 86400 86400'
        }], data['answers'])
    end
end