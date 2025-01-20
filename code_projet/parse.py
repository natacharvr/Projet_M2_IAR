import re
import numpy as np

def extract_values_from_file(file_path):
    # Lists to store the extracted values
    count_one_third_values = []
    count_two_third_values = []
    count_max_values = []

    # Regular expression to match the line format
    pattern = re.compile(r"timepoint: \d+, count_one_third: (\d+), count_two_third: (\d+), count_max: (\d+)")

    # Open the file and process each line
    with open(file_path, 'r') as file:
        for line in file:
            match = pattern.search(line)
            if match:
                # Extract values and append them to the respective lists
                count_one_third_values.append(int(match.group(1)))
                count_two_third_values.append(int(match.group(2)))
                count_max_values.append(int(match.group(3)))

    return count_one_third_values, count_two_third_values, count_max_values

# Example usage
file_path = '/home/natacha/Documents/IAR/Projet/pogobot-sdk/code_projet/0cmLucky.txt'
count_one_third, count_two_third, count_max = extract_values_from_file(file_path)

print("Mean One Third Values:", np.mean(count_one_third))
print("Mean Two Third Values:", np.mean(count_two_third))
print("Mean Max Values:", np.mean(count_max))

