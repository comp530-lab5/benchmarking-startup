import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import sem, t

data_file = "results/final_data.csv"  # Replace with the actual path to your file
data = pd.read_csv(data_file, header=None, names=["test_type", "device", "block_size", "stride_size", "throughput"])

# Ensure block_size and stride_size are numeric
data['block_size'] = pd.to_numeric(data['block_size'], errors='coerce')
data['stride_size'] = pd.to_numeric(data['stride_size'], errors='coerce')

def calculate_statistics(group):
    """Calculates the mean and 95% confidence interval for data points of a specific x value."""
    mean = group['throughput'].mean()
    ci = t.ppf(0.975, len(group) - 1) * sem(group['throughput'])  # 95% confidence interval
    return pd.Series({'mean': mean, 'ci': ci})

def plot_with_ci_overlayed(df, x, y, hue, title, xlabel, ylabel, filename):
    """Plotting function with overlays for different devices."""
    fig, ax = plt.subplots()
    for key, group in df.groupby(hue):
        ax.errorbar(group[x], group[y], yerr=group['ci'], label=f"{key}", capsize=5, marker='o', linestyle='-')
    ax.set_xscale('log')  # Ensure x-axis is logarithmic
    ax.set_title(title)
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.legend(title=hue)
    plt.tight_layout()
    plt.savefig(filename)
    plt.show()

def plot_stride_with_ci(df, x, y, title, xlabel, ylabel, filename):
    """Plotting function for stride tests."""
    fig, ax = plt.subplots()
    for key, group in df.groupby('device'):
        ax.errorbar(group[x], group[y], yerr=group['ci'], label=f"Device {key}", capsize=5, marker='o', linestyle='-')
    ax.set_xscale('log')  # Logarithmic scale for stride size
    ax.set_title(title)
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.legend()
    plt.tight_layout()
    plt.savefig(filename)
    plt.show()

# Generate plots for each test type
for test_type, test_name in [(0, "IO_Size_Read"), (1, "IO_Size_Write"), (4, "Random_IO_Read"), (5, "Random_IO_Write")]:
    test_data = data[data['test_type'] == test_type]
    if test_data.empty:
        print(f"No data found for test type {test_type}: {test_name}")
        continue

    # I/O Size tests and Random I/O tests
    if test_type in [0, 1, 4, 5]:
        results = test_data.groupby(['device', 'block_size']).apply(calculate_statistics).reset_index()
        print(f"Generating overlayed plot for test type {test_type}: {test_name}")
        plot_with_ci_overlayed(results, 'block_size', 'mean', 'device',
                               f"{test_name} Throughput by Block Size",
                               "Block Size (bytes)", "Throughput (MB/s)",
                               f"graphs/{test_name}_Overlayed.png")

    # I/O Stride tests
    elif test_type in [2, 3]:
        results = test_data.groupby(['device', 'stride_size']).apply(calculate_statistics).reset_index()
        for device in results['device'].unique():
            device_data = results[results['device'] == device]
            print(f"Generating stride plot for device {device}, test type {test_type}: {test_name}")
            plot_stride_with_ci(device_data, 'stride_size', 'mean',
                                f"{test_name} Throughput by Stride Size (Device {device})",
                                "Stride Size (bytes)", "Throughput (MB/s)",
                                f"graphs/{test_name}_Device_{device}_Stride.png")

