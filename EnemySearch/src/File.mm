#include "File.hpp"
#include <iostream>

std::string GetFilePath(std::string file_name)
{
    
    NSString *resDir = [[NSBundle mainBundle] resourcePath];
    NSString *filePath = [resDir stringByAppendingPathComponent:
                          [NSString stringWithUTF8String:file_name.c_str()]];
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        // show error message
        std::cout << "Not found." << std::endl;
        return "";
    }
	
    return [filePath UTF8String];
}
