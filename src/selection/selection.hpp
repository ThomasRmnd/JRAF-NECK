#ifndef JRAFNECK_SELECTION_SELECTION_HPP_
#define JRAFNECK_SELECTION_SELECTION_HPP_

#include "event/vertex.hpp"

class selection_base {

public:

    virtual ~selection_base() = default;

    virtual bool is_in(const vertex& vtx) const = 0;

};

#endif // JRAFNECK_SELECTION_SELECTION_HPP_