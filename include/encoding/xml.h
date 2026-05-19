#pragma once

#include <result.h>
#include <reader.h>

namespace kinetic {

class XmlTag {

};

class XmlReader : public kinetic::BufReader {
public:
  kinetic::Result<kinetic::XmlTag> read_tag() {
    using ResultT = kinetic::Result<kinetic::XmlTag>;



    return ResultT::err(kinetic::ErrorKind::SourceTodo, "unimplemented");
  }
};

}
