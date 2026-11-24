import matplotlib.pyplot as plt
import pandas as pd

# List of posture CSV files
posture_files = ['supine.csv', 'side.csv', 'prone.csv']

# Plot each posture data
for posture_file in posture_files:
    # Read CSV file into a pandas DataFrame
    df = pd.read_csv(posture_file)

    # Extract accelerometer data
    ax = df['Ax']
    ay = df['Ay']
    az = df['Az']

    # Create a time axis based on the index (assuming data was collected with a constant interval)
    time = df.index * 1.0  # Adjust if necessary

    # Create subplots for each accelerometer axis
    plt.figure(figsize=(10, 6))

    # Plot Ax
    plt.subplot(3, 1, 1)
    plt.plot(time, ax, label='Ax')
    plt.xlabel('Time')
    plt.ylabel('Acceleration (Ax)')
    plt.legend()

    # Plot Ay
    plt.subplot(3, 1, 2)
    plt.plot(time, ay, label='Ay')
    plt.xlabel('Time')
    plt.ylabel('Acceleration (Ay)')
    plt.legend()

    # Plot Az
    plt.subplot(3, 1, 3)
    plt.plot(time, az, label='Az')
    plt.xlabel('Time')
    plt.ylabel('Acceleration (Az)')
    plt.legend()

    # Set a title for the subplot
    plt.suptitle(f'Posture Data: {posture_file}')

    # Show the plots
    plt.tight_layout()
    plt.show()