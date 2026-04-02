#ifndef JRAFNECK_EVENT_IBD_HPP_
#define JRAFNECK_EVENT_IBD_HPP_

#include "event/vertex.hpp"

namespace jraf {

struct ibd {

    int run_id;
    jraf::vertex prompt;
    jraf::vertex delayed;

};

} // namespace jraf

inline bool operator<(const jraf::ibd& lhs, const jraf::ibd& rhs) {
    return lhs.prompt < rhs.prompt;
}

#endif // JRAFNECK_EVENT_IBD_HPP_