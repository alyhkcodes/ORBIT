#ifndef MATCHING_H
#define MATCHING_H

#include <vector>
#include "item.h"

struct MatchResult {
    int itemId;
    double score;
};

std::vector<MatchResult> findMatches(const Item& lostItem,
                                     const std::vector<Item>& allItems,
                                     int maximumResults = 10);

#endif
