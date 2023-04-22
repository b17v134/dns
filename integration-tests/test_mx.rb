require 'json'
require 'open3'
require "test/unit"

class TestMx < Test::Unit::TestCase
    def test_mx
        stdout, stderr, status = Open3.capture3('./integration-tests-dns/bin/resolv -s 10.23.0.7 example.test-bind -t mx -o json')
 
        assert_equal(0, status)
        assert_equal("", stderr)
        data = JSON.parse(stdout)
        
        answers = data['answers']
        answers.sort_by! { |value| value['rdata']}
        assert_equal([{
            'name'=> 'example.test-bind.', 
            'ttl'=> 300, 
            'class'=> 'IN', 
            'type'=> 'MX', 
            'rdata'=> '1 mail1.example.test-bind.'
        },
        {
            'name'=> 'example.test-bind.', 
            'ttl'=> 300, 
            'class'=> 'IN', 
            'type'=> 'MX', 
            'rdata'=> '10 mail10.example.test-bind.'
        },
        {
            'name'=> 'example.test-bind.', 
            'ttl'=> 300, 
            'class'=> 'IN', 
            'type'=> 'MX', 
            'rdata'=> '5 mail5.example.test-bind.'
        }], answers)
    end
end