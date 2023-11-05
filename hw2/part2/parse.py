import os
import re
import matplotlib.pyplot as plt
import pandas as pd
import argparse

def parse_log_file(file_path):
    with open(file_path, 'r') as file:
        content = file.readlines()
    serial_time = float(re.search(r'\[mandelbrot serial\]:\s+\[(\d+\.\d+)\] ms', content[0]).group(1))
    thread_time = float(re.search(r'\[mandelbrot thread\]:\s+\[(\d+\.\d+)\] ms', content[-3]).group(1))
    reduction_time = serial_time / thread_time
    thread_id = re.search(r'(\d+)\.log', os.path.basename(file_path)).group(1)
    thread_latencies = {f'thread_{m.group(1)}': float(m.group(2)) for m in re.finditer(r'\[thread (\d+)\]:\s+\[(\d+\.\d+)\] ms', '\n'.join(content[1:-3]))}
    return {'experiment': f'thread_{thread_id}', 'serial_time': serial_time, 'thread_time': thread_time, 'reduction_time': reduction_time, **thread_latencies}

parser = argparse.ArgumentParser(description='Process some log files.')
parser.add_argument('--logs_dir', required=True, help='The directory containing the log files')
parser.add_argument('--output_dir', required=True, help='The directory to save the output image')

args = parser.parse_args()

log_files = [os.path.join(args.logs_dir, f) for f in os.listdir(args.logs_dir) if f.endswith('.log')]
log_files.sort()
results = [parse_log_file(f) for f in log_files]

df = pd.DataFrame(results)

ax = df.plot(x='experiment', y=['serial_time', 'thread_time'], kind='bar')
plt.xticks(rotation='horizontal')
plt.savefig(f'{args.output_dir}/time.png', dpi=300)

ax = df.plot(x='experiment', y='reduction_time', kind='bar')
plt.xticks(rotation='horizontal')
plt.savefig(f'{args.output_dir}/reduction.png', dpi=300)

thread_cols = [col for col in df.columns if 'thread_' in col and col != 'thread_time']
ax = df.plot(x='experiment', y=thread_cols, kind='bar')
plt.xticks(rotation='horizontal')
plt.savefig(f'{args.output_dir}/latency.png', dpi=300)