#ifndef JRAFNECK_SELECTION_SELECTION_HPP_
#define JRAFNECK_SELECTION_SELECTION_HPP_

#include "event/vertex.hpp"

namespace jraf {

class selection {

public:

    virtual ~selection() = default;

    virtual bool is_in(const jraf::vertex& vtx) const = 0;

};

} // namespace jraf

#endif // JRAFNECK_SELECTION_SELECTION_HPP_