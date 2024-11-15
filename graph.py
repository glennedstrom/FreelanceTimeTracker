import json
import matplotlib.pyplot as plt
import datetime
import numpy as np

# Load data from JSON file
with open('freelancer_data.json', 'r') as f:  # Replace 'your_data.json'
    data = json.load(f)

freelancer_data = data['freelancer']
companies = freelancer_data['companies']

# Prepare data for plotting
events = []
colors = {}
color_list = plt.cm.tab10.colors

# Collect earnings events for all companies
for idx, company in enumerate(companies):
    company_name = company['companyName']
    colors[company_name] = color_list[idx % len(color_list)]  # Assign color

    for session in company['sessions']:
        start_timestamp = datetime.datetime.strptime(session['startTimestamp'], "%a %b %d %H:%M:%S %Y")
        end_timestamp = datetime.datetime.strptime(session['endTimestamp'], "%a %b %d %H:%M:%S %Y")
        
        # Calculate the total hours worked
        hours_worked = (end_timestamp - start_timestamp).total_seconds() / 3600.0  # Convert seconds to hours
        
        # Calculate the earnings based on pay rate (in dollars per hour)
        pay_rate = session['pay']  # Pay is in dollars per hour
        earnings = hours_worked * pay_rate  # Total earnings for this session
        
        # Add to events
        events.append((end_timestamp, earnings, company_name))

# Sort events by timestamp
events.sort(key=lambda x: x[0])

# Calculate cumulative earnings
cumulative_earnings = 0
timestamps = []
earnings = []
company_colors = []

for timestamp, earnings_amount, company_name in events:
    cumulative_earnings += earnings_amount
    timestamps.append(timestamp)
    earnings.append(cumulative_earnings)
    company_colors.append(colors[company_name])  # Color for the current company

# Create the plot
plt.figure(figsize=(10, 6))

plt.plot(timestamps, earnings, color='gray', linestyle='-', linewidth=1, alpha=0.5)

# Scatter points for each earning event
for timestamp, earning, company_color in zip(timestamps, earnings, company_colors):
    plt.scatter(timestamp, earning, color=company_color, edgecolor='black')

plt.xlabel("Time")
plt.ylabel("Total Earnings (USD)")
plt.title("Freelance Earnings Over Time")
plt.grid(True)
plt.xticks(rotation=45)  # Rotate timestamps for better readability
plt.tight_layout()  # Adjust layout to prevent labels from being cut off

# Save the plot
plt.savefig("earnings_plot.png")

print("Earnings plot saved as earnings_plot.png")
plt.show()
