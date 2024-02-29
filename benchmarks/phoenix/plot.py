import os
import sys
import matplotlib.pyplot as plt
import numpy as np

import seaborn as sb
palette = sb.color_palette("pastel")
colour = [palette[0], palette[2], palette[1], palette[3], palette[4], palette[5]]
hatch = ["", "//", "..", "", "++", ""]
my_fontsize = 5

versions_map = {
  "vanilla" :   "PMDK",
  "safepm"  :   "SafePM",
  "spp"     :   "SPP"
}

benchmarks_map = {
  "histogram" :   "histogram",    
  "kmeans"    :   "kmeans",
  "lr"        :   "linear\nregression",
  "mm"        :   "matrix\nmultiply",
  "pca"       :   "pca",
  "sm"        :   "string\nmatch",
  "wc"        :   "word\ncount"
}

# Function to read the average time from a file
def read_avg_time(file_path):
  with open(file_path, 'r') as file:
    line = file.readline().strip()
    time_str = line.split()[2]
    return float(time_str)

# Function to create a bar plot
def create_bar_plot(benchmarks, times, ylabel, save_path):
  fig, ax = plt.subplots()
  w = 0.25
  x = np.arange(len(benchmarks))

  # Convert times from us to s
  us_to_s = 1000000
  vanilla_times = [ float(t / us_to_s) for t in times['vanilla']]
  safepm_times = [ float(t / us_to_s) for t in times['safepm']]
  spp_times = [ float(t / us_to_s) for t in times['spp']]

  bar1 = ax.bar(x - w, vanilla_times, width = w , color = colour[0], hatch = hatch[0],
                                      edgecolor = 'black', align='center', label = versions_map['vanilla'])
  bar2 = ax.bar(x, safepm_times, width = w , color = colour[1], hatch = hatch[1],
                                      edgecolor = 'black', align='center', label = versions_map['safepm'])
  bar3 = ax.bar(x + w, spp_times, width = w , color = colour[2], hatch = hatch[2],
                                      edgecolor = 'black', align='center', label = versions_map['spp'])

  ax.set_title("Phoenix benchmark suite", fontsize = my_fontsize)
  ax.set_ylabel(ylabel, fontsize = my_fontsize)
  ax.tick_params(axis='y', labelsize = my_fontsize)
  ax.set_xticks(x)
  xlabels = [benchmarks_map[benchmark] for benchmark in benchmarks]
  ax.set_xticklabels(xlabels, fontsize = my_fontsize)
  lgd = ax.legend(fontsize = my_fontsize)
  
  fig.set_size_inches(3.2, 1.0)
  fig.savefig(save_path + ".pdf", dpi=300, format='pdf', bbox_extra_artists=(lgd,), bbox_inches='tight')
  fig.savefig(save_path + ".png", dpi=300, format='png', bbox_extra_artists=(lgd,), bbox_inches='tight')

# Function to create a percentage overhead bar plot
def create_percentage_overhead_plot(benchmarks, times, ylabel, save_path):
  fig, ax = plt.subplots()
  w = 0.25
  x = np.arange(len(benchmarks))

  vanilla_times = times['vanilla']
  safepm_times = times['safepm']
  spp_times = times['spp']

  percentage_overhead_safepm = [(s / v) for s, v in zip(safepm_times, vanilla_times)]
  percentage_overhead_spp = [(s / v) for s, v in zip(spp_times, vanilla_times)]

  print("SafePM overheads:")
  print(percentage_overhead_safepm)
  print("SPP overheads:")
  print(percentage_overhead_spp)
  bar1 = ax.bar(x - w/2, percentage_overhead_safepm, width = w , color = colour[0], hatch = hatch[0],
                                      edgecolor = 'black', align='center', label = versions_map['safepm'])
  bar2 = ax.bar(x + w/2, percentage_overhead_spp, width = w, color = colour[1], hatch = hatch[1],
                                      edgecolor = 'black', align='center', label = versions_map['spp'])

  ax.set_title("Phoenix benchmark suite", fontsize = my_fontsize)
  ax.set_ylabel(ylabel, fontsize = my_fontsize)
  ax.tick_params(axis='y', labelsize = my_fontsize)
  ax.set_xticks(x)
  xlabels = [benchmarks_map[benchmark] for benchmark in benchmarks]
  ax.set_xticklabels(xlabels, fontsize = my_fontsize)
  lgd = ax.legend(fontsize = my_fontsize)

  fig.set_size_inches(3.2, 1.0)
  fig.savefig(save_path + ".pdf", dpi=300, format='pdf', bbox_extra_artists=(lgd,), bbox_inches='tight')
  fig.savefig(save_path + ".png", dpi=300, format='png', bbox_extra_artists=(lgd,), bbox_inches='tight')

# Main script
if len(sys.argv) != 2:
  print("Usage: python script.py <path_to_files>")
  sys.exit(1)

file_path = sys.argv[1]
benchmarks = ['histogram', 'kmeans', 'lr', 'mm', 'pca', 'sm', 'wc']
variants = ['vanilla', 'safepm', 'spp']
times = {variant: [read_avg_time(os.path.join(file_path, f'{variant}_{benchmark}_avg.txt')) for benchmark in benchmarks] for variant in variants}

# Plot 1: Bar plot with all variants
create_bar_plot(benchmarks, times, 'Average Time (s)', os.path.join(file_path, 'phoenix_bar_plot_all_variants'))

# Plot 2: Percentage overhead bar plot
create_percentage_overhead_plot(benchmarks, times, 'Slowdown w.r.t. native PMDK', os.path.join(file_path, 'phoenix_bar_plot_overhead'))
