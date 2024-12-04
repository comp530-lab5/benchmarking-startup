import pandas as pd
import numpy as np
from scipy.stats import t

data_file = "results/test.csv" # replace with path to file with data
data = pd.read_csv(data_file, header=None, names=["test_type", "device", "block_size", "stride_size", "throughput"])
# add headers since there are none

## Key Steps in Calculating Required Samples:

# 1. Calculate Mean and Standard Deviation:
#    - For each data point, compute the mean and standard deviation.

# 2. Determine Confidence Level:
#    - Define a confidence level (we will use 95%) and margin of error (E- we will use 5%).

# 3. Calculate Required Samples:
#    - Use the formula for margin of error: E = t_(α/2) * (s / sqrt(n)), where:
#      * t_(α/2): T-score for confidence level
#      * s: Standard deviation
#      * n: Number of samples
#    - Rearrange to solve for n: n = (t_(α/2) * s / E)^2.

# 4. Iterate Over Data Points:
#    - For each data point, calculate the required samples and ensure it’s at least the initial number of runs (e.g., 5).

def calculate_required_samples(group, confidence_level=0.95, margin_of_error_ratio=0.05):
    """Calculate required samples using Student's T-Test."""
    mean = group['throughput'].mean()
    std_dev = group['throughput'].std()
    margin_of_error = margin_of_error_ratio * mean
    n = len(group)
    degrees_of_freedom = n - 1
    t_score = t.ppf(1 - (1 - confidence_level) / 2, degrees_of_freedom)
    required_samples = (t_score * std_dev / margin_of_error) ** 2
    return max(required_samples, n)  # ensure at least n samples

# calculate the required samples for each data point
grouped = data.groupby(["test_type", "device", "block_size", "stride_size"])
results = grouped.apply(lambda group: calculate_required_samples(group)).reset_index()
results.columns = ["test_type", "device", "block_size", "stride_size", "required_samples"]

# save the results to a file
results['required_samples'] = results['required_samples'].apply(np.ceil).astype(int)  # rounds up to nearest int
results_table = results[["test_type", "device", "block_size", "stride_size", "required_samples"]]
results_table.to_csv("results/required_samples.csv", index=False)