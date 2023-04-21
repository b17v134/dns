testFiles = File.dirname(File.absolute_path(__FILE__)) + '/test_*.rb' 

Dir.glob(testFiles).each {
    |testFile| require testFile 
}