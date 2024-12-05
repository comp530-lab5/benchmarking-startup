import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import sem, t

data_file = "results/final_data.csv" # replace with path to file with data
data = pd.read_csv(data_file, header=None, names=["test_type", "device", "block_size", "stride_size", "throughput"])
# add headers since there are none

def calculate_statistics(group):
    """Calculates the mean and 95% confidence interval for data points of a specific x value."""
    mean = group['throughput'].mean()
    ci = t.ppf(0.975, len(group) - 1) * sem(group['throughput']) # calculates t-score for 95% two-tailed test, sem is standard
        # error of mean, therefore ci is the range of uncertainty around the mean
    return pd.Series({'mean': mean, 'ci': ci})

def plot_with_ci(df, x, y, hue, title, xlabel, ylabel, filename):
    """Plotting function without overlays for different devices"""
    fig, ax = plt.subplots()
    for key, group in df.groupby(hue):
        ax.errorbar(group[x], group[y], yerr=group['ci'], label=f"{hue}={key}", capsize=5, marker='o', linestyle='-')
    ax.set_xscale('log')  # Set x-axis to logarithmic scale
    ax.set_title(title)
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.legend()
    plt.tight_layout()
    plt.savefig(filename)
    plt.show()

def plot_with_ci_overlayed(df, x, y, hue, title, xlabel, ylabel, filename):
    """Plotting function with overlays for different devices."""
    fig, ax = plt.subplots()
    for key, group in df.groupby(hue):
        ax.errorbar(group[x], group[y], yerr=group['ci'], label=f"{key}", capsize=5, marker='o', linestyle='-')
    ax.set_title(title)
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.legend(title=hue)
    plt.tight_layout()
    plt.savefig(filename)
    plt.show()


## Plots graphs based on tests with only one device per graph
# prepares the data for each test type
for test_type, test_name in [(2, "IO_Stride_Read"), (3, "IO_Stride_Write")]:
    test_data = data[data['test_type'] == test_type]

    # I/O Size tests
    if test_type in [0,1,4,5]:
        results = test_data.groupby(['device', 'block_size']).apply(calculate_statistics).reset_index()
        for device in results['device'].unique():
            device_data = results[results['device'] == device]
            plot_with_ci(device_data, 'block_size', 'mean', 'device',
                         f"{test_name} Throughput by Block Size",
                         "Block Size (bytes)", "Throughput (MB/s)",
                         f"graphs/{test_name}_Device_{device}.png")

    # I/O Stride tests
    elif test_type in [2, 3]:
        results = test_data.groupby(['device', 'stride_size', 'block_size']).apply(calculate_statistics).reset_index()
        for device in results['device'].unique():
            device_data = results[results['device'] == device]
            plot_with_ci(device_data, 'stride_size', 'mean', 'block_size',
                         f"{test_name} Throughput by Stride Size",
                         "Stride Size (bytes)", "Throughput (MB/s)",
                         f"graphs/{test_name}_Device_{device}.png")
