#include "File.hpp"

std::string GetFilePath(std::string file_name)
{
    
    NSString *resDir = [[NSBundle mainBundle] resourcePath];
    NSString *filePath = [resDir stringByAppendingPathComponent:
                          [NSString stringWithUTF8String:file_name.c_str()]];
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        // show error message
        return "";
    }
	
    return [filePath UTF8String];
}
