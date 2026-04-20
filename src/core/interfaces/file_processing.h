
#include <string>
class IAudioFileDecoder
{
public:

    virtual bool openFile(std::string filePath) = 0;

    virtual void reset() = 0;
    
};