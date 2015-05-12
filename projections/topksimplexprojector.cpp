#include <algorithm>
#include <cassert>
#include <numeric>
#include <utility>
#include <vector>

#include "topksimplexprojector.hpp"

namespace sdca {

template <typename RealType>
void TopKSimplexProjector<RealType>::ComputeThresholds(
    std::vector<RealType> &x,
    RealType &t,
    RealType &lo,
    RealType &hi) {

  typename std::vector<RealType>::iterator m_begin, l_begin;
  switch (top_k_cone_.CheckSpecialCases(x, t, lo, hi)) {
    case Projection::Zero:
      break;
    case Projection::Constant:
      if (top_k_cone_.get_k_real() * hi > knapsack_.get_rhs()) {
        knapsack_.PartitionAndComputeThresholds(x, t, lo, hi, m_begin, l_begin);
      }
      break;
    case Projection::General:
      knapsack_.PartitionAndComputeThresholds(x, t, lo, hi, m_begin, l_begin);
      if (CheckOnTopKCone(x, t, m_begin)) {
        top_k_cone_.ComputeGeneralCase(x, t, lo, hi);
      }
      break;
  }

}

template <typename RealType>
bool TopKSimplexProjector<RealType>::CheckOnTopKCone(
    std::vector<RealType> &x,
    RealType &t,
    typename std::vector<RealType>::iterator &m_begin) {

  // Check if the corresponding lambda is negative
  auto size = std::distance(x.begin(), m_begin);
  if (size) {
    RealType u = static_cast<RealType>(size);
    RealType sum_k_largest = std::accumulate(x.begin(), m_begin,
      static_cast<RealType>(0));
    RealType k = top_k_cone_.get_k_real();
    return k * ( sum_k_largest + (k - u) * t) < u;
  } else {
    return t < static_cast<RealType>(0);
  }
}

template class TopKSimplexProjector<float>;
template class TopKSimplexProjector<double>;

}
