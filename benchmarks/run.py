#!/usr/bin/env python3
"""Run repeatable BVH comparisons; preserve CSV timings and verify PNG equality."""
import argparse
import csv
import io
import os
from pathlib import Path
import statistics
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--executable', type=Path, default=Path('buildVCPkg/main'))
parser.add_argument('--output', type=Path, default=Path('buildVCPkg/benchmark'))
parser.add_argument('--repeats', type=int, default=3)
parser.add_argument('--threads', type=int, default=1)
args = parser.parse_args()
if args.repeats < 1 or args.threads < 1:
    parser.error('repeats and threads must be positive')
executable = args.executable.resolve()
args.output.mkdir(parents=True, exist_ok=True)
rows = []
for trial in range(1, args.repeats + 1):
    with (args.output / f'progress-{trial}.log').open('w') as log:
        result = subprocess.run(
            [str(executable), '--benchmark', '--width', '1000', '--height', '1000', '--rpp', '10'],
            cwd=args.output, env={**os.environ, 'OMP_NUM_THREADS': str(args.threads)},
            stdout=subprocess.PIPE, stderr=log, text=True, check=True)
    trial_rows = list(csv.DictReader(io.StringIO(result.stdout)))
    if len(trial_rows) != 8:
        raise RuntimeError(f'Expected eight timing rows, got {result.stdout!r}')
    for row in trial_rows:
        row['trial'] = trial
        row['threads'] = args.threads
        rows.append(row)
    for scene in {row['scene'] for row in trial_rows}:
        stem = Path(scene).stem
        if (args.output / f'{stem}_flat.png').read_bytes() != (args.output / f'{stem}_bvh.png').read_bytes():
            raise RuntimeError(f'BVH and flat PNGs differ for {scene}, trial {trial}')
    print(f'Trial {trial}: all four image pairs are byte-identical.', flush=True)
with (args.output / 'timings.csv').open('w', newline='') as file:
    writer = csv.DictWriter(file, fieldnames=list(rows[0]))
    writer.writeheader()
    writer.writerows(rows)
for scene in dict.fromkeys(row['scene'] for row in rows):
    times = {mode: statistics.median(float(row['render_seconds']) for row in rows
                                    if row['scene'] == scene and row['mode'] == mode)
             for mode in ('flat', 'bvh')}
    print(f"{scene}: flat {times['flat']:.3f}s, BVH {times['bvh']:.3f}s, "
          f"speedup {times['flat'] / times['bvh']:.2f}x")
