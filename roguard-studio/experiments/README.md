# Experiment artifacts

Durable snapshots live in the ignored owner-only `../var/studio.sqlite3`; private engine run artifacts live in `../var/runs/<run-id>/`. This tracked directory intentionally contains no subscriber keys or generated datasets.

Use the Experiments page to download immutable JSON, CSV or ZIP snapshots. Restoring a snapshot creates a fresh stopped design and preserves the original snapshot. It does not resume an old process or recycle its authentication material.
