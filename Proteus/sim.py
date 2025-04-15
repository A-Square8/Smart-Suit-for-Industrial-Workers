import matplotlib.pyplot as plt
import seaborn as sns
from collections import Counter

# Set seaborn style
sns.set(style="darkgrid")

# Hazard mapping
hazard_map = {
    "High Temperature": 1,
    "Gas Leak": 2,
    "Low Oxygen": 3,
    "Magnetic Field": 4,
    "Radiation": 5
}

hazard_labels = {
    0: "Normal",
    1: "High Temp",
    2: "Gas Leak",
    3: "Low Oxygen",
    4: "Magnetic Field",
    5: "Radiation"
}

# Time and hazard list
time_list = []
hazard_list = []
binary_status = []  # 0 = Normal, 1 = Hazard

# Read from log
with open('log.txt', 'r') as file:
    logs = file.readlines()

# Time simulation
current_time = 0
for log in logs:
    log = log.strip()
    if "Everything is normal" in log:
        for _ in range(5):
            time_list.append(current_time)
            hazard_list.append(0)
            binary_status.append(0)
            current_time += 1
    else:
        for keyword, code in hazard_map.items():
            if keyword in log:
                time_list.append(current_time)
                hazard_list.append(code)
                binary_status.append(1)
                current_time += 1
                break

# Calculate frequencies
hazard_counter = Counter(hazard_list)
total_time = len(time_list)
normal_time = hazard_counter[0]
abnormal_time = total_time - normal_time

# Create a dashboard figure
fig, axs = plt.subplots(2, 2, figsize=(14, 10))
fig.suptitle("Smart Suit System Monitoring Dashboard", fontsize=16, fontweight='bold')

# 1. Hazard Timeline
axs[0, 0].scatter(time_list, hazard_list, c=hazard_list, cmap='Set1', s=100, edgecolors='k')
axs[0, 0].set_yticks(list(hazard_labels.keys()))
axs[0, 0].set_yticklabels(list(hazard_labels.values()))
axs[0, 0].set_title("Hazard Events vs Time")
axs[0, 0].set_xlabel("Time (s)")
axs[0, 0].set_ylabel("Hazard Type")

# 2. Hazard Frequency Bar Chart
hazard_only = {k: v for k, v in hazard_counter.items() if k != 0}
axs[0, 1].bar(
    [hazard_labels[k] for k in hazard_only.keys()],
    hazard_only.values(),
    color=sns.color_palette("Set1", len(hazard_only))
)
axs[0, 1].set_title("Hazard Frequency")
axs[0, 1].set_ylabel("Count")

# 3. Normal vs Abnormal Pie Chart
axs[1, 0].pie(
    [normal_time, abnormal_time],
    labels=["Normal", "Alert"],
    colors=["green", "red"],
    autopct='%1.1f%%',
    startangle=140,
    explode=(0.05, 0.1)
)
axs[1, 0].set_title("Normal vs Alert Time Distribution")

# 4. Status Line Chart
axs[1, 1].plot(time_list, binary_status, drawstyle='steps-post', color='purple')
axs[1, 1].set_ylim(-0.1, 1.1)
axs[1, 1].set_yticks([0, 1])
axs[1, 1].set_yticklabels(["Normal", "Alert"])
axs[1, 1].set_xlabel("Time (s)")
axs[1, 1].set_title("System Status Over Time")

plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.show()
