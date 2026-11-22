# Advanced network architecture

Implementation boundary: preserve the legacy direct/co-channel adapter; add `matrix-iq-v2` as an explicit, bounded extension. Diagrams describe the implemented execution and evidence boundaries. Native status comes from the evidence registry, not these drawings.

## Topology and capability admission

```mermaid
flowchart LR
  T[Saved topology: EPCs, owned eNodeBs/cells, UEs] --> C[Central compatibility registry]
  C --> V[Validation API and UI diagnostics]
  C --> G[Configuration generation admission]
  C --> R[Runtime start admission]
  C --> P[Versioned templates]
  E[Exact native evidence: fingerprint/version/implementation hash] --> C
```

Each eNodeB and UE references its core explicitly when more than one exists. A UE selects a fixed EARFCN and a matching logical RF environment. No neighbor observation is manufactured from this assignment.

## Native supervision and isolation

```mermaid
flowchart TD
  B[Studio Engine] --> S[Owned unshare supervisor]
  S --> A[Core A private network]
  S --> Z[Core B separate network namespace]
  A --> E1[srsEPC A]
  A --> N1[srsENB processes]
  A --> U1[srsUE 1: own config/log/CSV]
  Z --> E2[srsEPC B]
  Z --> N2[srsENB processes]
  Z --> U3[srsUE 2: own config/log/CSV]
  U1 --> T1[UE 1 private TUN namespace]
  U3 --> T3[UE 2 private TUN namespace]
```

Initial code ceilings are two EPCs, two total UEs, four total cells/eNodeBs, three cells per eNodeB. These are not claims of measured capacity. A native failure fails the whole experiment: retain the failed role identity, stop healthy dependents as stopped, clear current attachment, preserve their historical measurements. Full experiment stop is always available; core-scoped stop uses environment ownership checks.

## RF matrix: one carrier environment

```mermaid
flowchart LR
  C1[Cell 1 actual TX samples] --> D[Aligned downlink matrix]
  C2[Cell 2 actual TX samples] --> D
  D -->|independent weighted receive stream| R1[UE 1 RX]
  D -->|independent weighted receive stream| R2[UE 2 RX]
  U1[UE 1 actual TX samples] --> M[Aligned uplink matrix]
  U2[UE 2 actual TX samples] --> M
  M -->|weighted sum of independent UE signals| B1[Cell 1 RX]
  M -->|weighted sum of independent UE signals| B2[Cell 2 RX]
```

For UE u and cell c use reciprocal amplitude gain `10^(-attenuation[u,c]/20)`. Each source starts at sample index zero; queues retain variable frame boundaries and provide bounded backpressure. Outputs use one millisecond of the environment's sample rate. No live geometry, delay, Doppler or gain timeline is modeled.

## Multiple frequencies (one core, two fixed-frequency UEs)

```mermaid
flowchart LR
  subgraph A[Core environment A]
    E1[eNB/cells EARFCN 3350] <--> B1[Broker A:3350] <--> U1[UE 1 fixed 3350]
    E2[eNB/cells EARFCN 3400] <--> B2[Broker A:3400] <--> U2[UE 2 fixed 3400]
  end

```

For the separate two-core profile, the native-supervision diagram above applies: one EPC/eNodeB/UE chain in each of two separate core network namespaces. These diagrams represent alternative profiles, not three simultaneous UEs.

There is no signal edge between different carrier brokers or independent core environments. Multi-frequency means independent fixed-frequency receivers, not inter-frequency scanning or retuning. Mixed bandwidth on one carrier is rejected. Different carriers can have separate transport/sample rates. Open5GS selection is a prerequisite boundary until isolated provisioning/readiness/attach are actually implemented and verified; existing external cores are never adopted or stopped.

## Evidence

```mermaid
flowchart LR
  N[Native NAS logs and per-component CSV] --> I[Engine component-aware ingestion]
  I --> S[(SQLite runs/metrics/events)]
  S --> J[Atomic ordered integration journal]
  J --> A[Authenticated snapshot/replay/SSE]
  A --> R[Roguard raw and normalized evidence]
  S --> U[UI: per-UE NAS, RRC, serving cell and metrics]
```

Keep legacy scalar fields as the first UE compatibility view, and put multi-component extensions inside the existing v1 `applied_topology` object. Every new metric carries a stable `component_id`. Ground truth remains experiment metadata. NAS attachment and RRC connection are tracked separately; current serving correlation requires fresh evidence and an unambiguous applied match.
