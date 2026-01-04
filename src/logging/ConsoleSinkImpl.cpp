#include "ConsoleSinkImpl.hpp"
#include <iostream>  

namespace logging {

    void ConsoleSinkImpl::write(const LogMessage & msg){
    std::cout << msg << std::endl; 
}

}

