require 'json'
require 'open3'
require "test/unit"

class TestAaaa < Test::Unit::TestCase
    def test_aaaa
        stdout, stderr, status = Open3.capture3('./integration-tests-dns/bin/resolv -s 10.23.0.7 example.test-bind -t aaaa -o json')
 
        assert_equal(0, status)
        assert_equal("", stderr)
        data = JSON.parse(stdout)
        
        answers = data['answers']
        answers.sort_by! { |value| value['rdata']}
        assert_equal([{
            'name'=> 'example.test-bind.', 
            'ttl'=> 300, 
            'class'=> 'IN', 
            'type'=> 'AAAA', 
            'rdata'=> 'fde3:0b2a:7ba2:f133:e679:29a8:3e53:f438'
        },
        {
            'name'=> 'example.test-bind.', 
            'ttl'=> 300, 
            'class'=> 'IN', 
            'type'=> 'AAAA', 
            'rdata'=> 'fde3:0b2a:7ba2:f133:e679:29a8:3e53:f439'
        },
        {
            'name'=> 'example.test-bind.', 
            'ttl'=> 300, 
            'class'=> 'IN', 
            'type'=> 'AAAA', 
            'rdata'=> 'fde3:0b2a:7ba2:f133:e679:29a8:3e53:f43a'
        },
        {
            'name'=> 'example.test-bind.', 
            'ttl'=> 300, 
            'class'=> 'IN', 
            'type'=> 'AAAA', 
            'rdata'=> 'fde3:0b2a:7ba2:f133:e679:29a8:3e53:f43b'
        }], answers)
    end
end