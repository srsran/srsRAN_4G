
#ifndef ENB_PHY_METRICS_H
#define ENB_PHY_METRICS_H


namespace srsenb {

// PHY metrics per user 

struct ul_metrics_t
{
  float n;
  float sinr;
  float rssi;
  float turbo_iters;
  float mcs;
  int n_samples;
};

struct dl_metrics_t
{
  float mcs;
  int n_samples;
};

struct phy_metrics_t
{
  dl_metrics_t   dl;
  ul_metrics_t   ul;
};

} // namespace srsenb

#endif // ENB_PHY_METRICS_H
